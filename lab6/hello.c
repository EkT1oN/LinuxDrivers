/* 
 *	lab6 -
 *	by Ilgiz Yakhin
 *	hello.c – Простой загружаемый модуль ядра
 *	Comands:
 *	make					- компилирование модуля
 *	sudo insmod hello.ko 	- установка загружаемого модуля в систему
 *	lsmod | head -10		- проверка, что модуль установлен (вывод списка модулей зарегистрированных в системе)
 *	dmesg | tail			- вывод журнала системных сообщений
 *	sudo rmmod hello		- удаление модуля
 *	sudo ./a.out on/off 2
 *	sudo ./a.out ledstate
 *	sudo ./a.out reset
 */
#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/sched/signal.h>
#include <linux/uaccess.h>

typedef struct led {
    int pin;
    unsigned char value;
} led_t;
/*
 	Реализовать LEDCMD_RESET_STATE, LEDCMD_GET_STATE, LEDCMD_SET_LED_STATЕ
 	Для записи в принтерный порт используйте макроопределение - outb(<значение>, PARALLEL_PORT);
 *	--
 	В команде LEDCMD_RESET_STATE с помощью макроопределения outb запишите в порт 0 (Лучше для этого состояния объявить константу, например
	#define INITIAL_STATE 0x00
 *	--
	В команде LEDCMD_GET_STATE достаточно считать байт из порта и скопировать его в выходной буфер.
 *	--
	В команде LEDCMD_SET_LED_STATE сначала надо считать содержимое порта, 
	затем в зависимости от значения led.value установить или сбросить 
	заданный (в led.pin) бит порта и записать полученное значение обратно в порт.
 *	--
	Используйте соглашение по которому первый светодиод имеет номер 0 
	и подключен к младшему разряду порта. Всего должно быть не больше 8 светодиодов.
 *	--
	На вашем компьютере может не быть принтерного порта. 
	Для эмуляции порта переопределите макроопределения inb и outb. 
 */
/*
 *	возвращают 32-х разрядное число, составленное из четырех битовых полей.
 *	31 - 30	- направление передачи данных: чтение (_IOR), запись (_IOW), чтение и запись одновременно (_IOWR) или данные отсутствуют (_IO)
 *	29 - 16	- результат вычисления оператора sizeof() над типом указанным в последнем аргументе макроопределений _IOR, _IOW или _IOWR (для _IO эти биты содержат 0)
 *	15 - 8	- число, указанное в первом аргументе макрооопределения, это число выбирают уникальным для драйвера (можно, например, указать MAJOR код драйвера)
 *	7  - 0	- номер команды, который указывается во втором аргументе макроопределений. Этот номер должен быть уникальным в списке команд драйвера. В последнем аргументе всех макроопределений, кроме _IO, передается тип указателя на параметры, передаваемые между драйвером и приложением. 
 *	
 */
#define LEDCMD_RESET_STATE _IO(HELLO_MAJOR, 1)                 	/* погасить все индикаторы */
#define LEDCMD_GET_LED_STATE _IOWR(HELLO_MAJOR, 3,  led_t *) 	/* вернуть значение индикатора с указанным номером */
#define LEDCMD_SET_LED_STATE _IOW(HELLO_MAJOR, 4, led_t *) 		/* установить значение индикатора с указанным номером */
#define PORT_EMULATION
#define PARALLEL_PORT 	0x378
#define HELLO_MAJOR 	250
#define INITIAL_STATE 	0x00

#ifdef PORT_EMULATION
#define inb(port) prn_data_em
#define outb(data, port) prn_data_em = ((unsigned char)data);
#define prn_state() printk(KERN_INFO "PRINTER STATE: %d\n", prn_data_em)
unsigned char prn_data_em;
#endif

static dev_t dev;
static struct cdev c_dev;
static struct class* cl;

static long hello_ioctl(struct file* f, unsigned int cmd, unsigned long arg);
static int my_open(		struct inode* inode, struct file* file);
static int my_close(	struct inode* inode, struct file* file);


static struct file_operations hello_fops = {
    .owner 			= THIS_MODULE,
    .open 			= my_open,
    .release 		= my_close,
	.unlocked_ioctl	= hello_ioctl,
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
    printk(KERN_INFO
    "Major number = %d Minor number = %d\n", MAJOR(dev), MINOR(dev));
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

    printk(KERN_INFO
    "Hello ioctl: regisered");
    return 0;

    err:
    printk("Hello ioctl: initialization failed with code %08x\n", retval);
    if (created)
        device_destroy(cl, dev);

    if (allocated)
        unregister_chrdev_region(dev, 1);

    if (cl)
        class_destroy(cl);

    return retval;
}

static int my_open(struct inode* inode, struct file* file) {
    return 0;
}

static int my_close(struct inode* inode, struct file* file) {
    return 0;
}

static long hello_ioctl(struct file* f, unsigned int cmd, unsigned long arg) {
    led_t led;

    /* Обработка команды чтения состояния светодиода */
    if (cmd == LEDCMD_GET_LED_STATE) {
        if (copy_from_user(&led, (led_t*) arg, sizeof(led_t))) {/* копируем параметры из пользовательского буфера в локальную переменную */
            return -EACCES;
        }

        if ((led.pin < 0) || (led.pin > 7)) {/* Номер светодиода должен быть в диапазоне от 0 до 7 */
            return -EINVAL;
        } else {
            led.value = (inb(PARALLEL_PORT) >> led.pin) & 1;     /* Считываем состояния разрядов параллельного порта и сдвигаем    */
            if (copy_to_user((led_t*) arg, &led, sizeof(led_t))) {  /*  вправо на led.pin разрядов и маскируем 1. Считаем, что светодиод 0 подключен */
                return -EACCES;
            }/* к младшему разряду порта.  Затем копируем всю структуру обратно в пользовательский буфер */
            else {
                return 0;
            }
        }
    } else if (cmd == LEDCMD_RESET_STATE) {
        outb(INITIAL_STATE, PARALLEL_PORT);
        prn_state();
        return 0;
    } else if (cmd == LEDCMD_SET_LED_STATE) {
        if (copy_from_user(&led, (led_t*) arg, sizeof(led_t))) {
            return -EACCES;
        }
        printk(KERN_INFO "v: %d, p: %d", led.value, led.pin);
        if (led.value == 1) {
            outb((1 << led.pin) | inb(PARALLEL_PORT), PARALLEL_PORT);
        } else {
            outb(~(1 << led.pin) & inb(PARALLEL_PORT), PARALLEL_PORT);
        }
        prn_state();
        if (copy_to_user((led_t*) arg, &led, sizeof(led_t))) {
            return -EACCES;
        }
        return 0;
    }
    return -EINVAL;
}

/*
 *	Деинициализаия
 */
static void __exit hello_exit(void) {
    printk(KERN_INFO "Hello ioctl: unregistered\n");
    device_destroy(cl, dev);
    unregister_chrdev_region(dev, 1);
    class_destroy(cl);
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ilgiz Yakhin");
MODULE_DESCRIPTION("Simple loadable kernel module");
