/*
 *	lab7
 *	by Ilgiz Yakhin
 *	hello.c – Простой загружаемый модуль ядра
 *	Comands:
 *	make					- компилирование модуля
 *	sudo insmod hello.ko 	- установка загружаемого модуля в систему
 *	lsmod | head -10		- проверка, что модуль установлен (вывод списка модулей зарегистрированных в системе)
 *	sudo rmmod hello		- удаление модуля
 *	sudo cat /dev/hello		- печать списка процессов в системе
*/

#include "circularBuff.h"

#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/ip.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/sched/signal.h>
#include <linux/tcp.h>
#include <linux/uaccess.h>
#include <linux/udp.h>
#include <linux/version.h>

#define LINE_SIZE 100
circularBuff_t caughtPackets;

static dev_t dev;
static struct cdev c_dev;
static struct class* cl;

void 	ip_handler_unregister(void);
int 	ip_handler_register(void);
static int my_open(	struct inode* inode, 
					struct file* file);
static int my_close(struct inode* inode, 
					struct file* file);
static ssize_t my_read(	struct file* filp, 
						char* buffer, 
						size_t length,
		       			loff_t* offset);
static ssize_t my_write(struct file* filp, 
						const char* buff, 
						size_t len,
						loff_t* off);

static struct file_operations hello_fops = {
	.owner 		= THIS_MODULE,
	.open 		= my_open,
	.release	= my_close,
	.read 		= my_read,
	.write 		= my_write
};

static int my_open(struct inode* inode, struct file* file) {
	initCircularBuf(&caughtPackets);
	ip_handler_register();
	return 0;
}

static int my_close(struct inode* inode, struct file* file) {
	ip_handler_unregister();
	return 0;
}

static ssize_t my_read(	struct file* filp, 
						char* buffer, 	/* буфер данных */
		       			size_t length, 	/* длина буфера */
		       			loff_t* offset) {
	size_t size = 0;
	char message[LINE_SIZE];
	if (!isEmptyBuff(&caughtPackets)) {
		popBuff(&caughtPackets, message);
		size = strlen(message);
		if (copy_to_user(buffer, message, size))
			return -EFAULT;
	}
	else {
		message[0] = '\0';
		if (copy_to_user(buffer, message, 1))
			return -EFAULT;
		return 1;
	}
	return size; /* количество байт возвращаемых драйвером в буфере */
}

static ssize_t my_write(struct file* filp, 
						const char* buff, 
						size_t len,
						loff_t* off) {
	return -EINVAL;
}

/* Структура, которая используется для регистрации обработчика. */
static struct nf_hook_ops nfho;

/*
 *	Функция обработчика
 *	upd:
 *	поменялась функция обработчика - 
 *	теперь в неё не передаётся сетевой интерфейс
 */
unsigned int hook_func(	void* priv, 
						struct sk_buff* skb,
		       			const struct nf_hook_state* state) {

	struct tcphdr* tcp_head;
	struct udphdr* udp_head;
	char packet_string[300];

	struct iphdr* ip_head = ip_hdr(skb);

	char dst_ip[16];
	char src_ip[16];
	snprintf(src_ip, 16, "%pI4", &ip_head->saddr);
	snprintf(dst_ip, 16, "%pI4", &ip_head->daddr);

	if (ip_head->protocol == IPPROTO_TCP) {
		tcp_head = tcp_hdr(skb);
		sprintf(packet_string, "Packet\tTCP\t%s:%i\t\t->\t%s:%i\n",
			src_ip, htons(tcp_head->source), dst_ip,
			htons(tcp_head->dest));
	}
	else if (ip_head->protocol == IPPROTO_UDP) {
		udp_head = udp_hdr(skb);
		sprintf(packet_string, "Packet\tUDP\t%s:%i\t\t->\t%s:%i\n",
			src_ip, htons(udp_head->source), dst_ip,
			htons(udp_head->dest));
	}
	else {
		sprintf(packet_string, "Packet\t%d\t%s\t\t->\t%s\n",
			ip_head->protocol, src_ip, dst_ip);
	}

	//printk(packet_string);
	pushBuff(&caughtPackets, packet_string);

	return NF_ACCEPT; /* отбрасывать все пакеты */
}

/* Процедура регистрирует обработчик сетевых пакетов */
int ip_handler_register(void) {

	struct net* n;
	/* Fill in our hook structure */
	nfho.hook = hook_func;
	/* Handler function */
	nfho.hooknum =
	    NF_INET_PRE_ROUTING; /* Первое сетевое событие для IPv4 */
	nfho.pf = PF_INET;
	nfho.priority =
	    NF_IP_PRI_FIRST; /* Назначаем обработчику 1-ый приоритет */
	/*
	 *	upd:
	 *	сам обработчик отдельно привязывается для выбранного сетевого интерфейса,
	 *	например, можно привязать его ко всем доступным
	 */
	for_each_net(n) nf_register_net_hook(n, &nfho);	//	upd: изменение названия имени функции
	printk("Hello: Netfilter hook registered!\n");
	return 0;
}

/* Процедура отменяет регистрацию обработчика сетевых пакетов */
void ip_handler_unregister(void) {
	/*
	 *	upd:
	 *	сам обработчик отдельно привязывается для выбранного сетевого интерфейса,
	 *	например, можно привязать его ко всем доступным,
	 *	аналогично, для отмены регистрации
	 */
	struct net* n;
	for_each_net(n) nf_unregister_net_hook(n, &nfho);	//	upd: изменение названия имени функции
	printk("Hello: Netfilter hook unregistered!\n");
}

/* Функция инициализации драйвера */
static int __init hello_init(void) {
	int retval;
	bool allocated = false;
	bool created = false;
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
	return 0;
}

/* Функция деинициализации драйвера. */
static void __exit hello_exit(void) {
	printk(KERN_INFO "Hello: unregistered\n");
	device_destroy(cl, dev);
	unregister_chrdev_region(dev, 1);
	class_destroy(cl);
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("MIlgiz Yakhin");