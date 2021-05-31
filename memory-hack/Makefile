obj-m += module_memory.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean

pagemap: pagemap.c
	gcc -o pagemap pagemap.c

huge_page: huge_page.c
	gcc -o huge_page huge_page.c

test:
	./test.sh
