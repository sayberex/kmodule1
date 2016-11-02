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

/*
 * intit.h
 * contain __init __exit macro
 * module.h
 * any kernel module must incluse that
 * kernel.h
 * contain printk
 * contain KERN_INFO macro
 */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("de lay lamer");
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



		printk(KERN_INFO"\n%s:tx packets = %d\n",dev->name, dev->stats.tx_packets);
		dev = next_net_device(dev);
	}
}

//program access point if success must return 0
static int __init fninit(void) {

	printk(KERN_INFO"Kmodule 1 Loaded\n");

	enum_netdev();
	return 0;
}

static void __exit fnexit(void) {
	printk(KERN_INFO"Kmodule1 terminated");
}

/*int main(void) {
	//puts("!!!Hello World!!!");
	//return EXIT_SUCCESS;
}*/

module_init(fninit);
module_exit(fnexit);
