#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NLOOP 5000

static int g = 0;

pthread_mutex_t mutex_lock = PTHREAD_MUTEX_INITIALIZER;

void * read_write_mem(void * _val)
{
    int i = 0;
    for(;i < NLOOP;i++)
    {
        pthread_mutex_lock(&mutex_lock);
        printf("pthread id is :%x,count is: %d\n",(unsigned long)pthread_self(),g);
        g = g + 1;
        pthread_mutex_unlock(&mutex_lock);
    }

    return NULL;
}


int main()
{
    pthread_t tid1;
    pthread_t tid2;
    pthread_create(&tid1,NULL,read_write_mem,NULL);
    pthread_create(&tid2,NULL,read_write_mem,NULL);
    pthread_join(tid1,NULL);
    pthread_join(tid2,NULL);
    printf("count final val is: %d\n",g);
    return 0;
}
