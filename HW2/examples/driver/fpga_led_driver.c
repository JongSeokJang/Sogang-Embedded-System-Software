/* FPGA LED Ioremap Control
FILE : fpga_led_driver.c 
AUTH : largest@huins.com */

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <mach/gpio.h>
#include <mach/regs-gpio.h>
#include <plat/gpio-cfg.h>
#include <linux/platform_device.h>
#include <linux/delay.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/kernel.h>
#include <linux/ioport.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/version.h>


#define IOM_LED_MAJOR 260		// ioboard led device major number
#define IOM_LED_NAME "fpga_led"		// ioboard led device name

#define IOM_LED_ADDRESS 0x04000016 // pysical address
#define IOM_DEMO_ADDRESS 0x04000300 

#define UON 0x00 // IOM
#define UOFF 0x01 // IOM


//Global variable
static int ledport_usage = 0;
static unsigned char *iom_fpga_led_addr;
static unsigned char *iom_demo_addr;

// define functions...
ssize_t iom_led_write(struct file *inode, const char *gdata, size_t length, loff_t *off_what);
ssize_t iom_led_read(struct file *inode, char *gdata, size_t length, loff_t *off_what);
int iom_led_open(struct inode *minode, struct file *mfile);
int iom_led_release(struct inode *minode, struct file *mfile);

// define file_operations structure 
struct file_operations iom_led_fops =
{
	.owner		=	THIS_MODULE,
	.open		=	iom_led_open,
	.write		=	iom_led_write,	
	.read		=	iom_led_read,	
	.release	=	iom_led_release,
};

// when led device open ,call this function
int iom_led_open(struct inode *minode, struct file *mfile) 
{	
	if(ledport_usage != 0) return -EBUSY;

	ledport_usage = 1;

	return 0;
}

// when led device close ,call this function
int iom_led_release(struct inode *minode, struct file *mfile) 
{
	ledport_usage = 0;

	return 0;
}

// when write to led device  ,call this function
ssize_t iom_led_write(struct file *inode, const char *gdata, size_t length, loff_t *off_what) 
{
	unsigned char value;
	const char *tmp = gdata;

	if (copy_from_user(&value, tmp, 1))
		return -EFAULT;

	outb(value,(unsigned int)iom_fpga_led_addr);	    
	
	return length;
}

// when read to led device  ,call this function
ssize_t iom_led_read(struct file *inode, char *gdata, size_t length, loff_t *off_what) 
{
	unsigned char value;
	char *tmp = gdata;

	value = inb((unsigned int)iom_fpga_led_addr);	    

	if (copy_to_user(tmp, &value, 1))
		return -EFAULT;

	return length;
}

int __init iom_led_init(void)
{
	int result;

	result = register_chrdev(IOM_LED_MAJOR, IOM_LED_NAME, &iom_led_fops);
	if(result < 0) {
		printk(KERN_WARNING"Can't get any major\n");
		return result;
	}

	iom_fpga_led_addr = ioremap(IOM_LED_ADDRESS, 0x1);
	iom_demo_addr = ioremap(IOM_DEMO_ADDRESS, 0x1);
	
	outb(UON,(unsigned int)iom_demo_addr);	    

	printk("init module, %s major number : %d\n", IOM_LED_NAME, IOM_LED_MAJOR);

	return 0;
}

void __exit iom_led_exit(void) 
{
	iounmap(iom_fpga_led_addr);
	iounmap(iom_demo_addr);
	unregister_chrdev(IOM_LED_MAJOR, IOM_LED_NAME);
}

module_init(iom_led_init);
module_exit(iom_led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Huins");
