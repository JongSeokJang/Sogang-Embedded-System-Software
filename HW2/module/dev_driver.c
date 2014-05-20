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

#define FND_MINOR 1
#define LED_MINOR 2
#define FPGA_FND_MINOR 3

#define FND_GPLCON 0x11000100	// FND Pin Configuration
#define FND_GPLDAT 0x11000104	// FND Pin Data
#define FND_GPECON 0x11400140	// FND Pin Configuration
#define FND_GPEDAT 0x11400144	// FND Pin Data

#define LED_GPBCON 0x11400040	// LED
#define LED_GPBDAT 0x11400044	// LED

#define IOM_FND_ADDRESS 0x04000004	// FPGA FND Physical Address
#define IOM_DEMO_ADDRESS 0x04000300
#define UON 0x00	// IOM
#define UOFF 0x01	// IOM

int dev_open(struct inode *, struct file *);
int dev_release(struct inode *, struct file *);
ssize_t dev_write(struct file *, const short *, size_t, loff_t *);
ssize_t dev_read(struct file *, char *, size_t, loff_t *);

// Initialize device drivers already exist
int init_fnd(void);
int init_led(void);
int init_fpga_fnd(void);

// Global variable
static int dev_usage = 0;

// fnd driver variables
static unsigned char *fnd_data;
static unsigned int *fnd_ctrl;
static unsigned char *fnd_data2;
static unsigned int *fnd_ctrl2;

// led driver variables
static char *buffer = NULL;
static unsigned char * led_data;
static unsigned int *led_ctrl;

// fpga fnd driver variables
static unsigned char *iom_fpga_fnd_addr;
static unsigned char *iom_demo_addr;

static struct file_operations dev_fops =
{
	.open = dev_open,
	.write = dev_write,
	.release = dev_release,
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
	// FND driver local variables
	const short *tmp = gdata;
	unsigned short fnd_buff = 0;
	char fnd_sel;
	char fnd_dat;

	// LED driver local variables
	unsigned short led_buff = 0;

	// FPGA FND driver local variables
	int i;
	unsigned char value[4];

	if(copy_from_user(&fnd_buff, tmp, length))
		return -EFAULT;

	// FND driver write
	fnd_sel = (char)(fnd_buff>>8);
	fnd_dat = (char)(fnd_buff&0x00FF);

	printk("FND Secect : %d\n", fnd_sel);
	printk("FND Data : %d\n", fnd_dat);

	outb(fnd_sel, (unsigned int)fnd_data2);
	outb(fnd_dat, (unsigned int)fnd_data);


	// LED driver write
	if(copy_from_user(&led_buff, tmp, length))
		return -EFAULT;

	printk("DATA : %d\n", led_buff);
	outb(led_buff, (unsigned int)led_data);

	// FPGA FND driver write
	if(copy_from_user(&value, tmp, 4))
		return -EFAULT;

	for(i=0;i>length;i++)
		outb(value[i], (unsigned int)iom_fpga_fnd_addr+i);

	return length;
}

ssize_t dev_read(struct file *inode, char *gdata, size_t length, loff_t *off_what){
	int i;
	unsigned char value[4];
	char *tmp = gdata;

	for(i=0;i<length;i++)
		value[i] = inb((unsigned int)iom_fpga_fnd_addr + i);

	if(copy_to_user(tmp, value, 4))
		return -EFAULT;

	return length;
}

int __init dev_init(void){
	int result, ret;

	result = register_chrdev(DEV_MAJOR, DEV_NAME, &dev_fops);
	if(result < 0){
		printk(KERN_WARNING"Can't get any major!\n");
		return result;
	}

	// initialize fnd driver
	ret = init_fnd();
	if(ret == -1)
		return -1;

	// initialize led driver
	ret = init_led();
	if(ret == -1)
		return -1;

	// initalize fpga fnd driver
	ret = init_fpga_fnd();
	if(ret == -1)
		return -1;

	printk("init module, /dev/%s major : %d\n", DEV_NAME, DEV_MAJOR);
	return 0;
}

int init_fnd(void){
	struct class *fnd_dev_class = NULL;
	struct device *fnd_dev = NULL;


	fnd_data = ioremap(FND_GPLDAT, 0x01);
	fnd_data2 = ioremap(FND_GPEDAT, 0x01);
	if(fnd_data == NULL){
		printk("ioremap failed!\n");
		return -1;
	}

	fnd_ctrl = ioremap(FND_GPLCON, 0x04);
	fnd_ctrl2 = ioremap(FND_GPECON, 0x04);
	if(fnd_ctrl == NULL){
		printk("ioremap failed!\n");
		return -1;
	} else{
		fnd_dev = device_create(fnd_dev_class, NULL, MKDEV(DEV_MAJOR, FND_MINOR), NULL, DEV_NAME);
		if(fnd_dev != NULL){
			outl(0x11111111, (unsigned int)fnd_ctrl);
			outl(0x10010110, (unsigned int)fnd_ctrl2);
		} else
			printk("device_create : failed!\n");
	}

	return 0;
}

int init_led(void){
	unsigned int get_ctrl_io = 0;
	struct class *led_dev_class = NULL;
	struct device *led_dev = NULL;

	led_data = ioremap(LED_GPBDAT, 0x01);
	if(led_data == NULL){
		printk("ioremap failed!\n");
		return -1;
	}

	led_ctrl = ioremap(LED_GPBCON, 0x04);
	if(led_ctrl == NULL){
		printk("ioremap failed!\n");
		return -1;
	} else{
		get_ctrl_io = inl((unsigned int)led_ctrl);
		led_dev = device_create(led_dev_class, NULL, MKDEV(DEV_MAJOR, LED_MINOR), NULL, DEV_NAME);
		buffer = (char *)kmalloc(1024, GFP_KERNEL);

		if(buffer != NULL)
			memset(buffer, 0, 1024);

		get_ctrl_io |= (0x11110000);
		outl(get_ctrl_io, (unsigned int)led_ctrl);
	}

	return 0;
}

int init_fpga_fnd(void){
	iom_fpga_fnd_addr = ioremap(IOM_FND_ADDRESS, 0x4);
	iom_demo_addr = ioremap(IOM_DEMO_ADDRESS, 0x1);

	outb(UON, (unsigned int)iom_demo_addr);

	return 0;
}

void __exit dev_exit(void){
	// FND driver free
	outb(0xFF, (unsigned int)fnd_data);
	iounmap(fnd_data);
	iounmap(fnd_data2);
	iounmap(fnd_ctrl);
	iounmap(fnd_ctrl2);

	// LED driver free
	outb(0xF0, (unsigned int)led_data);
	iounmap(led_data);
	iounmap(led_ctrl);

	// FPGA FND driver free
	iounmap(iom_fpga_fnd_addr);
	iounmap(iom_demo_addr);

	unregister_chrdev(DEV_MAJOR, DEV_NAME);
	printk("dev driver module removed.\n");
}

module_init(dev_init);
module_exit(dev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lee, Jun-Ho");
