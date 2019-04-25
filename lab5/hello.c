/* 
 *	lab5 -
 *	by Ilgiz Yakhin
 *	hello.c – Простой загружаемый модуль ядра
 *	Comands:
 *	make					- компилирование модуля
 *	sudo insmod hello.ko 	- установка загружаемого модуля в систему
 *	lsmod | head -10		- проверка, что модуль установлен (вывод списка модулей зарегистрированных в системе)
 *	dmesg | tail			- вывод журнала системных сообщений
 *	sudo rmmod hello		- удаление модуля
 *	sudo cat /dev/hello		- печать списка процессов в системе
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/version.h>
#include <asm/uaccess.h> 
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 11, 0)
	#include <linux/signal.h>
#else
	#include <linux/sched/signal.h>
#endif

#define MAX_MESSAGE_LENGTH 1024

static dev_t dev;
static struct cdev c_dev;
static struct class* cl;

static int my_open(		struct inode* inode,struct file* file);
static int my_close(	struct inode* inode,struct file* file);
static ssize_t my_read(	struct file*  filp, char* buffer, 		size_t length, 	loff_t* offset);
static ssize_t my_write(struct file*  filp,	const char* buff, 	size_t len, 	loff_t* off);

char message[MAX_MESSAGE_LENGTH];
int read_idx 	= 0;
int write_idx 	= 0;

/*
 * 	структура hello_fops содержит указатели 
 *	на функции обработчики операций ввода/вывода драйвера
 */
static struct file_operations hello_fops = {
	/*
	 *	Каждое из полей структуры, кроме owner, содержит 
	 *	указатель на функцию обработчик. 
	 *	В структуре инициализируются только указатели на обработчики, 
	 *	реализованные в драйвере. 
	 *	Например, если драйвер не поддерживает запись, 
	 *	не надо инициализировать поле write.
	 */
	.owner 		= THIS_MODULE,
	.open 		= my_open,
	.release	= my_close,
	.read 		= my_read,
	.write 		= my_write
};

/* 
 *	Инициализация
 */
static int __init hello_init(void) {
	int retval;
	bool allocated 	= false;
	bool created 	= false;
	cl = NULL;

	retval = alloc_chrdev_region(&dev, 0, 1, "hello");
	if (retval)
		goto err;

	allocated = true;
	printk(KERN_INFO "Major number = %d Minor number = %d\n", MAJOR(dev), MINOR(dev));
	cl = class_create(THIS_MODULE, "teach_devices");
	if (!cl) {
		retval = -1;
		goto err;
	}

	if (device_create(cl, NULL, dev, NULL, "hello") == NULL) {
		retval = -1;
		goto err;
	}

	created = true;
	cdev_init(&c_dev, &hello_fops);
	retval = cdev_add(&c_dev, dev, 1);
	if (retval)
		goto err;

	printk(KERN_INFO "Hello: regisered");
	return 0;

err:
	printk("Hello: initialization failed with code %08x\n", retval);
	if (created)
		device_destroy(cl, dev);

	if (allocated)
		unregister_chrdev_region(dev, 1);

	if (cl)
		class_destroy(cl);

	return retval;
}

static int my_open(struct inode* inode, struct file* file) {
    int comm_len;
    int written;
    struct task_struct* task_list;

    write_idx 	= 0;
    read_idx 	= 0;
    for_each_process(task_list) {
        pr_info("== %s [%d]\n", task_list -> comm, task_list -> pid);
        comm_len = strlen(task_list -> comm);
        if (write_idx + comm_len >= MAX_MESSAGE_LENGTH) {
            return 0;
        }
        memcpy(message + write_idx, task_list -> comm, comm_len);
        write_idx += comm_len;
        if (write_idx + 17 >= MAX_MESSAGE_LENGTH) {
            return 0;
        }
        message[write_idx++] = ' ';
        written = snprintf(message + write_idx, 16, "%d", task_list -> pid);
        write_idx += written;
        if (write_idx + 1 >= MAX_MESSAGE_LENGTH) {
            return 0;
        }
        message[write_idx++] = '\n';

    }
    return 0;
}

static int my_close(struct inode* inode, struct file* file) {
	return 0;
}

static ssize_t my_read( struct file* 	filp,  
                        char*			buffer, 	// буфер данных
                        size_t 			length, 	// длина буфера
                        loff_t* 		offset) {
    int return_idx;
    char ch;
    return_idx = read_idx++;
    if (return_idx >= MAX_MESSAGE_LENGTH) {
        return 0;
    }
    ch = message[return_idx];
    if (raw_copy_to_user(buffer, &ch, sizeof(ch)))
        return -EFAULT;
    return sizeof(ch); /* количество байт возвращаемых драйвером в буфере */
}

static ssize_t my_write(struct file* filp, const char* buff, size_t len, loff_t* off) {
        return -EINVAL;
}

/* 
 *	Деинициализаия 
 */
static void __exit hello_exit(void) {
    printk(KERN_INFO "Hello: unregistered\n");
    device_destroy (cl, dev);
    unregister_chrdev_region (dev, 1);
    class_destroy (cl);
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ilgiz Yalhin");
MODULE_DESCRIPTION("Simple loadable kernel module");

