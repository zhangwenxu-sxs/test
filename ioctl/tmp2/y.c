#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define IOCTL_HI3516_REBOOT       _IO('L',0x1110)
#define IOCTL_WDG_LOAD                      _IOW('L',0x1111,unsigned int)
#define IOCTL_WDG_CONTROL_DISABLE_ALL       _IO('L',0x1112)
#define IOCTL_WDG_CONTROLL_DISABLE_REBOOT   _IO('L',0x1113)
#define IOCTL_WDG_CONTROL_ENBALE_ALL        _IO('L',0x1114)
#define IOCTL_WDG_FEED_DOG                  _IO('L',0x1115)
#define IOCTL_WDG_WHO_FEED_DOG              _IOW('L',0x1116,int)

int main(void)
{

    int fd;

    fd = open("/proc/ctrl_hi3616_reboot",O_RDWR);
    if(fd < 0){
        perror("open");
        exit(1);
    }

    ioctl(fd,IOCTL_HI3516_REBOOT);
   // while(1){
        //ioctl(fd,IOCTL_WDG_LOAD,4);
        //sleep(3);

        
        //ioctl(fd,IOCTL_WDG_CONTROL_DISABLE_ALL,2);
        //sleep(3);

        //ioctl(fd,IOCTL_WDG_CONTROLL_DISABLE_REBOOT);
        //sleep(3);

        //ioctl(fd,IOCTL_WDG_CONTROL_ENBALE_ALL);
       // ioctl(fd,IOCTL_WDG_FEED_DOG);
    //    sleep(3);
 //   }

    close(fd);
    return 0;
}
