#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <signal.h>
int count = 0;
void set_timer()
{
        struct itimerval itv, oldtv;
        itv.it_interval.tv_sec = 5;
        itv.it_interval.tv_usec = 0;
        itv.it_value.tv_sec = 5;
        itv.it_value.tv_usec = 0;
  
 
        setitimer(ITIMER_REAL, &itv, &oldtv);
}
 
void sigalrm_handler(int sig)
{
        count++;
        printf("timer signal.. %d\n", count);
}
 
int main()
{
        signal(SIGALRM, sigalrm_handler);
        set_timer();
        while (count < 1000)
        {}
        exit(0);
}
