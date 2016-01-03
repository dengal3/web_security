/**
    buffer overflow
    password: 123456892483684782379573 // 超过十多位即可
    output: welcome back

    access: 0x7fffffffdc7c
    password: 0x7fffffffdc70
    我们让程序从password那里overflow，溢出到access那里
    在实验中发现，一般在字符串的后面会预留一段空间来防止溢出，因此如果在按变量出现顺序分配时，会出现后出现的字符串的起始地址在先出现的变量的起始地址的前面，但是当字符串定长足够短时，起始地址的先后顺序就会与变量出现的先后顺序一致，同时两者之间会有预留空间出现防止溢出。
    所以在实验中不能简单地输入12345来实验溢出的目的，要输入足够长的字符串使得空出的段也全部填上。
**/
#include <stdio.h>
#include <string.h>

void main() {
    int access;
    char password[3];

    while (1) {
        access = 0;
        scanf("%s", password);
        if (strcmp(password, "2016") == 0) {
            access = 1;
        }
        if (access != 0)
            printf("Welcome back\n");
        else
            printf("error");
    }
}