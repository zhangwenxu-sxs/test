#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>


#define IOCTL_USB_HUB_SET_HIGH       _IO('L',0x1120)
#define IOCTL_USB_HUB_SET_LOW        _IO('L',0x1122)

int main(void)
{
    int fd;
    
    fd = open("/proc/usb_hub_reset",O_RDWR);
    if(fd < 0){
        perror("open");
        exit(1);
    }

    ioctl(fd,IOCTL_USB_HUB_SET_HIGH);

    close(fd);
    return 0;
}
