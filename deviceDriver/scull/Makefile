# Before call the "make", download and build a raspbian kernel along with toolchain setup
# This make file need to access the kernel file addressed by "KERNELDIR"
#
# Comment/uncomment the following line to disable/enable debugging
DEBUG = y
CROSS_C := arm-linux-gnueabihf-
CC := arm-linux-gnueabihf-gcc

# Add your debugging flag (or not) to ccflags.
ifeq ($(DEBUG),y)
  DEBFLAGS = -O -g -DSCULL_DEBUG # "-O" is needed to expand inlines
else
  DEBFLAGS = -O2
endif

ccflags-y += $(DEBFLAGS)
ccflags-y += -I$(LDDINC)

ifneq ($(KERNELRELEASE),)
# call from kernel build system

scull-objs := main.o pipe.o access.o

obj-m	:= scull.o

else

#KERNELDIR ?= /lib/modules/$(shell uname -r)/build
KERNELDIR := ~/linux
PWD       := $(shell pwd)

modules:
	make ARCH=arm CROSS_COMPILE=$(CROSS_C) -C $(KERNELDIR) M=$(PWD) LDDINC=$(PWD)/../include modules
#	$(MAKE) -C $(KERNELDIR) M=$(PWD) LDDINC=$(PWD)/../include modules

endif


clean:
	make ARCH=arm CROSS_COMPILE=$(CROSS_C) -C $(KERNELDIR) M=$(PWD) LDDINC=$(PWD)/../include clean
#	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions

depend .depend dep:
	$(CC) -M *.c > .depend


ifeq (.depend,$(wildcard .depend))
include .depend
endif
