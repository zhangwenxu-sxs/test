#include <stdio.h>
#include <unistd.h>
#include <signal.h>
void sigalrm_fn(int sig)
{
    printf("alarm!\n");
    alarm(2);
    return;
}
int main(void)
{
    signal(SIGALRM,sigalrm_fn);
    alarm(2);
    while(1);
    //pause();
}
