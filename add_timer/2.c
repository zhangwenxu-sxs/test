#include <stdio.h>
#include <signal.h> 
#include <unistd.h>

void SigFun(int signo){
    printf("SigFun is running\n");
}

int main(){
    if(signal(SIGALRM, SigFun) == SIG_ERR){
        perror("signal\n");
　　　　 return -1;
    }
    alarm(5);
    pause();
    return 0;
}
