#include <sys/types.h>
#include <unistd.h>

int a = 5;

int main(int argc, char* argv[])
{
    pid_t ret;

    ret = fork();
    if (ret == 0) {
        a = 3;
        while (1) {
            printf("child: &a=0x%x  a=%d \n", &a, a);
            sleep(1);
        }
    } else {
        while (1) {
            printf("parent: &a=0x%x  a=%d \n", &a, a);
            sleep(1);
        }
    }

    return 0;
}
