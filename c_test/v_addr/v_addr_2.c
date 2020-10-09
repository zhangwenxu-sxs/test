#include <stdio.h>

int a = 3;

int main(int argc, char* argv[])
{

    printf("&a=0x%x \n", &a);

    while (1) {
        printf("a=%d \n", a);
        sleep(1);
    }

    return 0;
}
