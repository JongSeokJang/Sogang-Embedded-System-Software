#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/ioport.h>
#include <linux/version.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/slab.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/ioctl.h>
#include <mach/gpio.h>
#include <mach/regs-gpio.h>
#include <plat/gpio-cfg.h>

#define DEV_MAJOR 242	// dev driver major number
#define DEV_MINOR 0	// dev driver minor number
#define DEV_NAME "dev_driver"	// dev driver name

int dev_open(struct inode *, struct file *);
int dev_release(struct inode *, struct file *);
ssize_t dev_write(struct file *, const short *, size_t, loff_t *);
ssize_t dev_read(struct file *, char *, size_t, loff_t *);

// Global variable
static int dev_usage = 0;

static struct file_operations dev_fops =
{
	.open = dev_open,
	.release = dev_release,
	.write = dev_write,
	.read = dev_read,
};

int dev_open(struct inode *minode, struct file *mfile){
	if(dev_usage != 0)
		return -EBUSY;

	dev_usage = 1;

	return 0;
}

int dev_release(struct inode *minode, struct file *mfile){
	dev_usage = 0;
	return 0;
}

ssize_t dev_write(struct file *inode, const short *gdata, size_t length, loff_t *off_what){
	return length;
}

ssize_t dev_read(struct file *inode, char *gdata, size_t length, loff_t *off_what){
	return length;
}

int __init dev_init(void){
	int result;

	result = register_chrdev(DEV_MAJOR, DEV_NAME, &dev_fops);
	if(result < 0){
		printk(KERN_WARNING"Can't get any major!\n");
		return result;
	}

	printk("init module, /dev/%s major : %d\n", DEV_NAME, DEV_MAJOR);
	return 0;
}

void __exit dev_exit(void){
	unregister_chrdev(DEV_MAJOR, DEV_NAME);
	printk("dev driver module removed.\n");
}

module_init(dev_init);
module_exit(dev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lee, Jun-Ho");
