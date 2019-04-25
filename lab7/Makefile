# пример Makefile драйвера
# если объявлена переменная среды окружения KERNELRELEASE
# запуски из среды компиляции ядра, которой надо указать компилируемый файл.
ifneq (${KERNELRELEASE},)
    obj-m := hello.o
# Иначе вызов из командной строки.
# Тогда вызываем среду компиляции ядра.
else
    KERNEL_SOURCE 	:= /lib/modules/$(shell uname -r)/build
    PWD 			:= $(shell pwd)
default:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} modules
check:
	sudo insmod hello.ko
	echo
	lsmod | head -10
	echo
	sudo cat /dev/hello
clean:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} clean
endif
