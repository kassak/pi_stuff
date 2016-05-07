ifneq ($(KERNELRELEASE),)
	obj-m := remeasure.o
	ccflags-y := -Wall
endif
# normal makefile
        KERNELRELEASE ?= `uname -r`
	KDIR ?= /usr/lib/modules/$(KERNELRELEASE)/build
default:
	$(MAKE) -C $(KDIR) M=$$PWD
install:
	install -D -t "$(PREFIX)/etc/modprobe.d/" remeasure.conf && \
	install -D -t "$(PREFIX)/usr/lib/modules/$(KERNELRELEASE)/kernel/drivers/gpio" remeasure.ko

