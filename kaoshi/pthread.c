#include <stdio.h>
#include <signal.h>
#include <stddef.h>
#include <pthread.h>

volatile sig_atomic_t flag = 0;

void handler(int signum)
{
    flag = 1;
}

int task(void * data)
{
    while(!flag)
    {   
        printf("222\n");
    }
    return 0;
}


int main(void)
{
    signal(SIGINT,handler);
    pthread_t tid;

    if(pthread_create(&tid,NULL,(void*)task,NULL)!=0)
    {
        printf("error\n");
    }

    while(!flag)
    {
        printf("111\n");
    }
    

    printf("exit\n");
    return 0;
}
