#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define LED_NUM_ON        _IOW('L',0x1122,int)
#define LED_NUM_OFF        _IOW('L',0x3344,int)
#define LED_ALL_ON        _IO('L',0x1234)
#define LED_ALL_OFF        _IO('L',0x5678)


int main(void)
{

    int fd;

    fd = open("/dev/led",O_RDWR);
    if(fd < 0){
    perror("open");
    exit(1);
    }

    //闪灯
    while(1){
    //第一个灯闪
    ioctl(fd,LED_NUM_ON,1);
    sleep(1);
    ioctl(fd,LED_NUM_OFF,1);
    sleep(1);
    
    //第二个灯闪
    ioctl(fd,LED_NUM_ON,2);
    sleep(1);
    ioctl(fd,LED_NUM_OFF,2);
    sleep(1);

    //两个灯同时闪
    ioctl(fd,LED_ALL_ON);
    sleep(1);
    ioctl(fd,LED_ALL_OFF);
    sleep(1);

    }

    close(fd);
    return 0;
}
