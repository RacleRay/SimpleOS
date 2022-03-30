#include <stdio.h>

// gcc -m32 test.c  -fno-pie -no-pie
// -fpie -pie 将可执行文件做成和动态链接库一样的效果 gcc 5.x 之后默认打开
int main() {
    printf("nonono! %p \n", main);
    return 0;
}