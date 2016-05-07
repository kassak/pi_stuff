ifneq ($(KERNELRELEASE),)
	obj-m := remeasure.o
	ccflags-y := -Wall
else
# normal makefile
	KDIR ?= /usr/lib/modules/`uname -r`/build
default:
	$(MAKE) -C $(KDIR) M=$$PWD
endif
