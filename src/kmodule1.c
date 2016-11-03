/*
 ============================================================================
 Name        : kmodule1.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

//#include <stdio.h>
//#include <stdlib.h>

//#include "/usr/src/linux-headers-3.16.0-4-common/arch/x86/include/asm/init.h"
//#include "/usr/src/linux-headers-3.16.0-4-common/arch/x86/include/asm/module.h"

//#include "/usr/src/linux-headers-3.16.0-4-common/include/linux/init.h"
//#include "/usr/src/linux-headers-3.16.0-4-common/include/linux/module.h"

#include	<linux/init.h>
#include	<linux/module.h>
#include	<linux/kernel.h>
#include	<linux/netdevice.h>
#include	<linux/timer.h>

#include	<linux/fs.h>
#include	<linux/syscalls.h>
#include	<linux/file.h>
#include	<linux/fcntl.h>
#include	<asm/uaccess.h>

static struct timer_list	tm_stat;					//timer struct

struct	file	*filp;									//file descriptor
mm_segment_t	old_fs;									//fs usr addr space holder
loff_t			pos = 0;								//initial offset for f I/O operation

static unsigned long	isfileopened = 0;				//is pipe opened flag

#define	BUF_SIZE	64									//buffer size
char	buf[BUF_SIZE];									//buffer for print operations
int		buf_len;										//actual string length in buffer


void enum_netdev(void);									//enumerate network interfaces and print statistics

int snprintf(char *buf, size_t size, const char *fmt, ...) {	//not used
	va_list	args;
	int	i;

	va_start(args,fmt);
	i = vsnprintf(buf, size, fmt, args);
	va_end(args);
	return i;
}


void	fnlog(char *format, ...) {						//easy print to pipe function
	va_list args;
	int i;

	if (isfileopened) {
		for (i = 0;i < BUF_SIZE; i++) buf[i] = 0;

		va_start(args,format);
		buf_len = vsnprintf(buf, BUF_SIZE, format, args);
		if (buf_len != -1) vfs_write(filp, buf, buf_len, &pos);
		va_end(args);
	}
}




void fnCb_tm_stat(unsigned long data) {
	static unsigned long counter = 0;

	printk(KERN_INFO"timer interrupt %d", (int)counter);

	//if (isfileopened) vfs_write(filp,"data1\n", 6, &pos);

	/*
	if (isfileopened) {
		buf_len = snprintf(buf, BUF_SIZE, "data: %lu\n", counter++);
		if (buf_len != -1)
			vfs_write(filp, buf, buf_len, &pos);
	}*/

	/*fnlog("data %lu\n", counter++);
	fnlog("data %lu\n", counter++);
	fnlog("data %lu\n", counter++);
	fnlog("data %lu\n", counter++);*/

	enum_netdev();


	mod_timer(&tm_stat, jiffies + msecs_to_jiffies(1000));		//set timer timeout


}
/*
 * intit.h
 * contain __init __exit macro
 * module.h
 * any kernel module must incluse that
 * kernel.h
 * contain printk
 * contain KERN_INFO macro
 */


//printk - message can be shown on non graphical console from where module was loaded
// in most of cases message shown in /var/log/messages

void enum_netdev(void) {
	struct net_device	*dev;

	dev = first_net_device(&init_net);
	while(dev) {

		//print network statistic  to /var/log/message
		printk(KERN_INFO"%s statistic",dev->name);
		printk(KERN_INFO"tx packets = %d\n", (int)(dev->stats.tx_packets));
		printk(KERN_INFO"rx packets = %d\n", (int)(dev->stats.tx_packets));
		printk(KERN_INFO"tx bytes = %d\n",   (int)(dev->stats.tx_bytes));
		printk(KERN_INFO"rx bytes = %d\n",   (int)(dev->stats.rx_bytes));


		//print network statistics to pipe
		fnlog("%s statistic\n", dev->name);
		fnlog("TX PACKETS = %lu\n", dev->stats.tx_packets);
		fnlog("RX PACKETS = %lu\n", dev->stats.rx_packets);
		fnlog("TX BYTES   = %lu\n", dev->stats.tx_bytes);
		fnlog("RX BYTES   = %lu\n", dev->stats.rx_bytes);


		/*if (isfileopened) {
			buf_len = snprintf(buf, BUF_SIZE, "data: %lu\n", counter++);
			if (buf_len != -1)
				vfs_write(filp, buf, buf_len, &pos);
		}*/


		dev = next_net_device(dev);		//get next network interface
	}
}

void init_fio(void) {					//init file operations
	//http://benninger.ca/posts/writing-to-a-file-from-the-kernel/
	//*****************************************************************
	//struct	file	*filp;
	//mm_segment_t	old_fs;
	//loff_t			pos = 0;


	old_fs = get_fs();
	set_fs(get_ds());

	filp = filp_open("/tmp/pipe_name", O_WRONLY/*|O_CREAT*/, 0777);

	if ((IS_ERR(filp)) )
		printk(KERN_INFO"Can't open file");
	else {
		printk(KERN_INFO"file opened success");
		isfileopened = 1;

		//vfs_write(filp,"data1\n", 6, &pos);
		//vfs_write(filp,"data2\n", 6, &pos);
		//filp->f_op->write(filp, "data from kern", 15, &filp->f_pos);
		//if (filp) filp_close(filp, NULL);
	}

	//set_fs(old_fs);
	//*****************************************************************
}

void deinit_fio(void) {					//deinit file operations
	if (filp) filp_close(filp, NULL);
	set_fs(old_fs);
}

//program access point if success must return 0
static int __init fninit(void) {
	int ret;

	printk(KERN_INFO"Kmodule 1 Loaded\n");

	//enum_netdev();


	setup_timer(&tm_stat, fnCb_tm_stat, 0);								//init timer
																		//jiffies - global tick counter
	ret = mod_timer(&tm_stat, jiffies + msecs_to_jiffies(1000));		//set timer timeout
	if (ret)	printk(KERN_INFO"mod_timer: can't set timeout");


	init_fio();


	return 0;
}

static void __exit fnexit(void) {
	int ret;

	ret = del_timer(&tm_stat);
	if (ret) printk(KERN_INFO"timer still in use");

	deinit_fio();

	printk(KERN_INFO"Kmodule1 terminated");
}

/*int main(void) {
	//puts("!!!Hello World!!!");
	//return EXIT_SUCCESS;
}*/

MODULE_LICENSE("GPL");
MODULE_AUTHOR("de lay lamer");
module_init(fninit);
module_exit(fnexit);
