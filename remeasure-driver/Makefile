ifneq ($(KERNELRELEASE),)
	obj-m := remeasure.o
	ccflags-y := -Wall
else
# normal makefile
	KDIR ?= /usr/lib/modules/`uname -r`/build
default:
	$(MAKE) -C $(KDIR) M=$$PWD
client-test:
	gcc -Wall remeasure-client-test.c -lnl-genl-3 -I/usr/include/libnl3 -o remeasure-client-test
endif
