#include <stdio.h>

extern int x;  // extern 声明  x 在某处定义过
extern void printHello(char *msg); // 声明 printHello 已经在某处定义过

int main(int argc, char *argv[]) {
    printf("%d\n", x);
    printHello("tom");
    return 0;
}