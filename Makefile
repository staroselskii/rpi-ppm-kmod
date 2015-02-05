obj-m += ppm.o

-include Make.defs

all: 
	make ARCH=arm CROSS_COMPILE=${CROSS_COMPILE} -C $(KERNEL_SRC) M=$(PWD) modules

clean: 
	make -C $(KERNEL_SRC) M=$(PWD) clean
