
VERSION_NUM :=$(shell uname -r)

KERNELDIR ?= /usr/src/linux-headers-$(VERSION_NUM)
PWD := $(shell pwd)

all:
	make -C $(KERNELDIR) M=$(PWD) modules
	gcc app.c -o app
clean:
	make -C $(KERNELDIR) M=$(PWD) clean
	rm app

obj-m += k.o

