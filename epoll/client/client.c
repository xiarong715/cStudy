#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_LINE 1024

int main(int argc, char const *argv[])
{
    if (argc != 2) {
        printf("usage: ./client.exe ip\n");
        return 0;
    }
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket err");
    }

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);

    const char *ipstr = argv[1];
    if (argv[1] == NULL || strlen(argv[1]) == 0) {
        ipstr = "192.168.86.128";
    }
    if (inet_pton(AF_INET, ipstr, &server_addr.sin_addr.s_addr) <=0) {
        fprintf(stderr, "inet pton err: %s\n", ipstr);
        exit(0);
    }

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect err");
        exit(0);
    }

    char input[100];
    char recvline[MAX_LINE] = {0};
    int count = 0;
    while(fgets(input, 100, stdin) != NULL) {
        printf("input: %s\n", input);

        if (send(sockfd, input,strlen(input), 0) == -1) {
            perror("send err");
        }

        count = 0;
        while (1){
            int n = read(sockfd, recvline+count, MAX_LINE);
            if (n == MAX_LINE) {
                count += n;
                continue;
            }else if (n < 0) {
                perror("recv");
                break;
            }else {
                count += n;
                recvline[count] = '\0';
                printf("recv: %s\n", recvline);
                break;
            }
        }
    }
    return 0;
}
