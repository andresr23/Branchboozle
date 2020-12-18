#---- Module
obj-m += pmc-module.o

#-------------------------------------------------------------------------------

.PHONY: intel amd clean

.SILENT:
intel:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	echo "  !!  pmc-module.ko built for Intel."

.SILENT:
amd:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) CFLAGS_MODULE=-DAMD modules
	echo "  !!  pmc-module.ko built for AMD's Zen."

.SILENT:
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
