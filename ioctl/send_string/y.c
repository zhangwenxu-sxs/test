#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define LED_NUM_ON        _IOW('L',0x1122,unsigned long)


int main(void)
{

    int fd;

    fd = open("/dev/led",O_RDWR);
    if(fd < 0){
        perror("open");
        exit(1);
    }
    char v[256];

    char* addr = v;

    

  
    ioctl(fd,LED_NUM_ON,addr);


    printf(" addr = %d, v = %s \n",(int)addr,v);   
/*
    while(1)
    {
        sleep(1);
    }*/
    close(fd);
    return 0;
}
