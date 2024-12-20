#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/epoll.h>

#define MAX_EVENTS 1000
#define BUFFER_SIZE 1024

int main(int argc, char const *argv[])
{
    // tcp
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("socket err");
        return 1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(8080);

    if (bind(server_socket,(struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind err");
        close(server_socket);
        return 1;
    }

    if (listen(server_socket, 5) == -1) {
        perror("listen err");
        return 1;
    }

    // epoll
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = server_socket;

    int epoll_fd = epoll_create(100);
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_socket, &event) == -1) {
        perror("epoll ctl err");
        close(server_socket);
        close(epoll_fd);
    }

    struct epoll_event events[MAX_EVENTS];
    int nfds;
    while(1) {
        if (nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1), nfds == -1) {
            perror("epoll wait err");
            break;
        }

        for(int i = 0; i < nfds; i++) {
            if (events[i].data.fd == server_socket) {
                printf("accept\n");
                struct sockaddr_in client_addr;
                int sockaddr_in_len = sizeof(client_addr);
                int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &sockaddr_in_len);
                if (client_socket == -1) {
                    perror("accept errr");
                    continue;
                }

                event.events = EPOLLIN;
                event.data.fd = client_socket;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_socket, &event) == -1) {
                    perror("epoll ctl client socket err");
                    close(client_socket);
                    continue;
                }
            } else if (events[i].events & EPOLLIN) {
                printf("EPOLLIN\n");
                int client_socket = events[i].data.fd;
                char buffer[BUFFER_SIZE];
                int read_len = read(client_socket, buffer, sizeof(buffer));
                if (read_len == -1) {
                    perror("read err");
                } else if(read_len == 0) {
                    printf("client disconnect\n");
                    if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_socket, NULL) == -1) {
                        perror("epoll ctl del err");
                    }
                    close(client_socket);
                } else {
                    buffer[read_len] = '\0';
                    printf("recevied from client: %s\n", buffer);

                    char *resp = "message recevied successfully";
                    if (write(client_socket, resp, strlen(resp)) == -1)  {
                        perror("write err");
                    }
                }
            } else if (events[i].events & EPOLLOUT) {
                printf("EPOLLOUT\n");
            }
        }
    }

    close(server_socket);
    close(epoll_fd);
    return 0;
}
