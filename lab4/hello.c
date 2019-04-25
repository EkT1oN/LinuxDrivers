/* 
 *	lab4
 *	by Ilgiz Yakhin
 *	hello.c – Простой загружаемый модуль ядра
 *	Comands:
 *	make					- компилирование модуля
 *	sudo insmod hello.ko 	- установка загружаемого модуля в систему
 *	lsmod | head -10		- проверка, что модуль установлен (вывод списка модулей зарегистрированных в системе)
 *	dmesg | tail			- вывод журнала системных сообщений
 *	sudo rmmod hello		- удаление модуля
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,11,0)
	#include <linux/signal.h>
#else
	#include <linux/sched/signal.h>
#endif

/*
 *	Функция вывода информации о процессах
 */
static int print_process_info(void) {
	/*
	 *	The macro for_each_process(task)
	 *	iterates over the entire task list.
	 *	On each iteration, 
	 *	task points to the next task in the list:
	 */
	struct task_struct *task;
	for_each_process(task) {
    	/* this pointlessly prints the name and PID of each task */
    	printk("Hello: %s[%d]\n", task -> comm, task -> pid);
	}
	return 0;
}

/* 
 *  Инициализация 
 */ 
static int __init hello_init(void) {
    printk(KERN_INFO "Hello: registered\n");
	print_process_info();	// Output function of proccesing informations
	return 0;
}

/* 
 *  Деинициализаия 
 */
static void __exit hello_exit(void) {
    printk(KERN_INFO "Hello: unregistered\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ilgiz");
MODULE_DESCRIPTION("Simple loadable kernel module");
