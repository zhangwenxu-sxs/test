obj-m += k.o
 
  
 
#generate the path
 
CURRENT_PATH:=$(shell pwd)
 
#the current kernel version number
 
LINUX_KERNEL:=$(shell uname -r)
 
#the absolute path
 
VERSION_NUM :=$(shell uname -r)

LINUX_KERNEL_PATH:=/usr/src/linux-headers-$(VERSION_NUM)
 
#complie object
 
all:
	make -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) modules
	gcc -o y y.c
	rm -rf modules.order Module.symvers .*.cmd *.o *.mod.c .tmp_versions *.unsigned
 
#clean
 
clean:
	rm -rf modules.order Module.symvers .*.cmd *.o *.mod.c *.ko .tmp_versions *.unsigned
