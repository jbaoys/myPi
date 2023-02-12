# Before call the "make", download and build a raspbian kernel along with toolchain setup
# This make file need to access the kernel file addressed by "KDIR"
#
# Comment/uncomment the following line to disable/enable debugging
DEBUG = y
TOOLCHAIN := arm-linux-gnueabihf-
CC := arm-linux-gnueabihf-gcc
#OUTTEST := outputTest
#INTEST := inputTest

# module name
MODULE_NAME = raspi_gpio_driver
KDIR = ~/linux
PWD       := $(shell pwd)

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
obj-m	:= $(MODULE_NAME).o

else
modules:
	make ARCH=arm CROSS_COMPILE=$(TOOLCHAIN) -C $(KDIR) M=$(PWD) LDDINC=$(PWD)/../include modules

endif

#$(OUTTEST): $(OUTTEST).c
#	$(CC) -o $(OUTTEST) $(OUTTEST).c
clean:
	make ARCH=arm CROSS_COMPILE=$(TOOLCHAIN) -C $(KDIR) M=$(PWD) LDDINC=$(PWD)/../include clean
	rm inputTest outputTest
#	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions

