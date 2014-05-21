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

#define FND_MINOR 1	// fnd driver minor number

// fnd driver device
#define FND_GPL2CON 0x11000100	// fnd pin configuration
#define FND_GPL2DAT 0x11000104	// fnd pin data
#define FND_GPE3CON 0x11400140	// fnd pin configuration
#define FND_GPE3DAT 0x11400144	// fnd pin data

int dev_open(struct inode *, struct file *);
int dev_release(struct inode *, struct file *);
ssize_t dev_write(struct file *, const short *, size_t, loff_t *);
ssize_t dev_read(struct file *, char *, size_t, loff_t *);

// Global variable
static int dev_usage = 0;

// fnd global variable
static unsigned char *fnd_data;
static unsigned int *fnd_ctrl;
static unsigned char *fnd_data2;
static unsigned int *fnd_ctrl2;

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
	/* FND driver write */
	const short *tmp = gdata;
	unsigned short fnd_buff = 0;

	char fnd_sel;
	char fnd_dat;

	if(copy_from_user(&fnd_buff, tmp, length))
		return -EFAULT;

	fnd_sel = (char)(fnd_buff>>8);
	fnd_dat = (char)(fnd_buff&0x00FF);

	printk("FND Secect : %d\n", fnd_sel);
	printk("FND data : %d\n", fnd_dat);

	outb(fnd_sel, (unsigned int)fnd_data2);
	outb(fnd_dat, (unsigned int)fnd_data);

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

	/* FND driver initialization begin */
	struct class *fnd_dev_class = NULL;
	struct device *fnd_dev = NULL;

	fnd_data = ioremap(FND_GPL2DAT, 0x01);
	fnd_data2 = ioremap(FND_GPE3DAT, 0x01);
	if(fnd_data == NULL){
		printk("FND ioremap failed!\n");
		return -1;
	}

	fnd_ctrl = ioremap(FND_GPL2CON, 0x04);
	fnd_ctrl2 = ioremap(FND_GPE3CON, 0x04);
	if(fnd_ctrl == NULL){
		printk("FND ioremap failed!\n");
		return -1;
	} else{
		fnd_dev = device_create(fnd_dev_class, NULL, MKDEV(DEV_MAJOR, FND_MINOR), NULL, DEV_NAME);
		if(fnd_dev != NULL){
			outl(0x11111111, (unsigned int)fnd_ctrl);
			outl(0x10010110, (unsigned int)fnd_ctrl2);
		} else
			printk("FND device create : failed!\n");
	}
	outb(0xFF, (unsigned int)fnd_data);
	outb(0xFF, (unsigned int)fnd_data);
	/* FND driver initialization ended */

	printk("init module, /dev/%s major : %d\n", DEV_NAME, DEV_MAJOR);
	return 0;
}

void __exit dev_exit(void){
	/* FND driver free */
	outb(0xFF, (unsigned int)fnd_data);
	iounmap(fnd_data);	iounmap(fnd_data2);
	iounmap(fnd_ctrl);	iounmap(fnd_ctrl2);

	unregister_chrdev(DEV_MAJOR, DEV_NAME);
	printk("dev driver module removed.\n");
}

module_init(dev_init);
module_exit(dev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lee, Jun-Ho");
