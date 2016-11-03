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

static struct timer_list	tm_stat;

void fnCb_tm_stat(unsigned long data) {
	static unsigned long counter = 0;

	printk(KERN_INFO"timer interrupt %d", (int)counter);
	mod_timer(&tm_stat, jiffies + msecs_to_jiffies(5000));		//set timer timeout
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
		//dev->stats.rx_bytes
		//dev->stats.tx_bytes
		//dev->stats.rx_packets
		//dev->stats.tx_packets


		printk(KERN_INFO"%s statistic",dev->name);
		printk(KERN_INFO"tx packets = %d\n", (int)(dev->stats.tx_packets));
		printk(KERN_INFO"rx packets = %d\n", (int)(dev->stats.tx_packets));
		printk(KERN_INFO"tx bytes = %d\n",   (int)(dev->stats.tx_bytes));
		printk(KERN_INFO"rx bytes = %d\n",   (int)(dev->stats.rx_bytes));
		dev = next_net_device(dev);
	}
}

void init_fio(void) {
	/*mm_segment_t old_fs;
	struct file	*file;

	old_fs = get_fs();
	set_fs(KERNEL_DS);
	loff_t	pos = 0;

	int fd;

	fd = sys_open("/tmp/pipe_name", O_WRONLY, 0644);

	if (fd > 0) {
		file = fget(fd);
		if (file) {
			vfs_write(file, "data", 4, &pos);
			fput(file);
		}
		sys_close(fd);
	}
	set_fs(old_fs);*/

	/*mm_segment_t old_fs;
	struct file *hfile;


	old_fs = get_fs();
	int	fd;

	set_fs(KERNEL_DS);
	fd = sys_open("/tmp/pipe_name", O_WRONLY, 0777);
	if (fd > 0) {
		sys_write(fd,"from module", 11);
		sys_close(fd);
	}
	set_fs(old_fs);*/

	struct	file	*filp;
	mm_segment_t	old_fs;

	filp = filp_open("/tmp/pipe_name", O_WRONLY|O_CREAT, 0777);

	if (filp != -1) {

	}
	else printk(KERN_INFO"Can't open file");

	if (IS_ERR(filp)) printk(KERN_INFO"Open error");

	old_fs = get_fs();
	set_fs(get_ds());

	filp->f_op->write(filp, "data from kern", 15, &filp->f_pos);

	set_fs(old_fs);
	if (filp) filp_close(filp, NULL);
}

//program access point if success must return 0
static int __init fninit(void) {
	int ret;

	printk(KERN_INFO"Kmodule 1 Loaded\n");

	//enum_netdev();


	setup_timer(&tm_stat, fnCb_tm_stat, 0);								//init timer
																		//jiffies - global tick counter
	ret = mod_timer(&tm_stat, jiffies + msecs_to_jiffies(5000));		//set timer timeout
	if (ret)	printk(KERN_INFO"mod_timer: can't set timeout");


	init_fio();


	return 0;
}

static void __exit fnexit(void) {
	int ret;

	ret = del_timer(&tm_stat);
	if (ret) printk(KERN_INFO"timer still in use");

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
