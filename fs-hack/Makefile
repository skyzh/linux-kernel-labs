obj-m += new_romfs.o
new_romfs-objs += super.o storage.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean
	rm source.img || true

romfs.img:
	rm romfs.img
	genromfs -d romfs_src -f romfs.img

mount:
	mount -v -o loop -t new_romfs romfs.img romfs_target

umount:
	umount romfs_target

insmod:
	insmod new_romfs.ko

rmmod:
	rmmod new_romfs.ko

.PHONY: romfs.img mount umount insmod rmmod
