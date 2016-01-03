/*
    利用溢出跳转到我们想执行的程序地址
    code最后一行的程序是下一个执行程序的入口地址
*/

#include <stdio.h>
#include <string.h>

char code[]= "\x41\x41\x41\x41"
            "\x41\x41\x41\x41"
            "\x41\x41\x41\x41"
            "\x41\x41\x41\x41"
            "\x41\x41\x41\x41"
            "\x41\x41\x41\x41"
            "\xaa\x05\x40\x00"; // target:0x 00 40 05 aa

void copy(const char *input) {
    char buf[10];
    strcpy(buf, input);
    printf("%s\n", buf);
}

void bug(void) {
    printf("I shouldn't have appeared\n");
}

int main() {
    copy(code);
    return 0;
}