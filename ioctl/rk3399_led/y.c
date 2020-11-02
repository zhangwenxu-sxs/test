#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define IOCTL_WHITE_LED_UP        _IO('L',0x1120)
#define IOCTL_RED_LED_UP       _IO('L',0x1121)
#define IOCTL_WHITE_LED_DOWN      _IO('L',0x1122)
#define IOCTL_RED_LED_DOWN     _IO('L',0x1123)

int main(void)
{

    int fd;

    fd = open("/proc/led_ctrl",O_RDWR);
    if(fd < 0){
        perror("open");
        exit(1);
    }

    ioctl(fd,IOCTL_WHITE_LED_DOWN);
    ioctl(fd,IOCTL_RED_LED_DOWN);

    while(1)
    {
        sleep(1);
        ioctl(fd,IOCTL_RED_LED_UP);
        sleep(1);
        ioctl(fd,IOCTL_RED_LED_DOWN);
        sleep(1);
        ioctl(fd,IOCTL_WHITE_LED_UP);
        sleep(1);
        ioctl(fd,IOCTL_WHITE_LED_DOWN);
    }    
    close(fd);
    return 0;
}
