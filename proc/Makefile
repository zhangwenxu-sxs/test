#obj-m:=kernel.o
#KDIR:=/lib/modules/5.4.0-47-generic/build
#PWD:=/home/zhangwenxu/work/test/netlink
#
#default:
#	$(MAKE) -C $(KDIR) M=$(PWD) modules
#
#clean:
#	rm -rf .*.cmd *.o *.mod.c *.ko .tmp_versions *.order *symvers *Module.markers

obj-m:=proc.o


CURRENT_PATH :=$(shell pwd)
VERSION_NUM :=$(shell uname -r)
LIUNX_PATH :=/usr/src/linux-headers-$(VERSION_NUM)

all:
	make -C $(LIUNX_PATH) M=$(CURRENT_PATH) modules
clean:
	make -C $(LIUNX_PATH) M=$(CURRENT_PATH) clean
