/********************************************
  2014 Sogang Univ. Embedded System Software
  Assignment #3 (Due date. '14. 06. 09.)
  Made by Lee Jun-Ho (dangercloz@gmail.com)
*********************************************/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/kernel.h>
#include <linux/ioport.h>
#include <linux/version.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <mach/gpio.h>
#include <mach/regs-gpio.h>
#include <plat/gpio-cfg.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/gpio.h>

#define DEV_NAME "stopwatch"
#define DEV_MAJOR 245

int stopwatch_open(struct inode *, struct file *);
int stopwatch_release(struct inode *, struct file *);
ssize_t stopwatch_write(struct file *, const short *, size_t, loff_t *);

static struct file_operations stopwatch_fops =
{
	.open = stopwatch_open,
	.release = stopwatch_release,
	.write = stopwatch_write,
};

// Global variables
static int stopwatch_usage = 0;

int stopwatch_open(struct inode *minode, struct file *mfile){
	if(stopwatch_usage != 0)
		return -EBUSY;

	stopwatch_usage = 1;

	return 0;
}

int stopwatch_release(struct inode *minode, struct file *mfile){
	stopwatch_usage = 0;

	return 0;
}

ssize_t stopwatch_write(struct file *inode, const short *gdata, size_t length, loff_t *off_what){
	return length;
}

int __init stopwatch_init(void){
	int result;

	result = register_chrdev(DEV_MAJOR, DEV_NAME, &stopwatch_fops);
	if(result < 0){
		printk(KERN_WARNING"Can't get any major!\n");
		return result;
	}

	printk("init module, /dev/stopwatch major : %d\n", DEV_MAJOR);

	return 0;
}

int __exit stopwatch_exit(void){
	unregister_chrdev(DEV_MAJOR, DEV_NAME);
	printk("Stopwatch module removed.\n");
}

module_init(stopwatch_init);
module_exit(stopwatch_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("DangerCloz");
