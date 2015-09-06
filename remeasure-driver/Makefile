ifneq ($(KERNELRELEASE),)
	obj-m := remeasure.o
	ccflags-y := -Wall
else
# normal makefile
	KDIR ?= /usr/lib/modules/`uname -r`/build
default:
	$(MAKE) -C $(KDIR) M=$$PWD
client-lib:
	gcc -Wall -c remeasure-client.c -I/usr/include/libnl3 -o remeasure-client.o
client-test: client-lib
	gcc -Wall -lnl-3 -lnl-genl-3 remeasure-client-test.c remeasure-client.o -o remeasure-client-test
switch-poller: client-lib
	gcc -Wall -lnl-3 -lnl-genl-3 remeasure-switch-poller.c remeasure-client.o -o remeasure-switch-poller
endif
