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

#define DEV_NAME "stopwatch"	// stopwatch module name
#define DEV_MAJOR 245		// stopwatch module major number

#define FND_GPL2CON 0x11000100	// fnd pin configuration
#define FND_GPL2DAT 0x11000104	// fnd pin data
#define FND_GPE3CON 0x11400140	// fnd pin configuration
#define FND_GPE3DAT 0x11400144	// fnd pin data

wait_queue_head_t wq_write;
DECLARE_WAIT_QUEUE_HEAD(wq_write);
irqreturn_t inter_handler(int irq, void *dev_id, struct pt_regs *reg);

int stopwatch_open(struct inode *, struct file *);
int stopwatch_release(struct inode *, struct file *);
ssize_t stopwatch_write(struct file *, const short *, size_t, loff_t *);

static struct file_operations stopwatch_fops =
{
	.owner = THIS_MODULE,
	.open = stopwatch_open,
	.write = stopwatch_write,
	.release = stopwatch_release,
};

static struct struct_mydata{
	struct timer_list timer;
	int count;
};

// Global variables
static int stopwatch_usage = 0;
static int quit_flag = 1;

// gpio fnd global variables
static unsigned char *fnd_data;
static unsigned int *fnd_ctrl;
static unsigned char *fnd_data2;
static unsigned int *fnd_ctrl2;

// timer module global variable
struct struct_mydata mydata;
struct struct_mydata quit_timer;

// interrupt module global variable
int interruptCount = 0;

static void kernel_timer_blink(unsigned long timeout){
	struct struct_mydata *p_data = (struct struct_mydata *)timeout;

	// increase timer count
	p_data->count++;

	// set timer
	mydata.timer.expires = get_jiffies_64() + (1 * HZ);
	mydata.timer.data = (unsigned long)&mydata;
	mydata.timer.function = kernel_timer_blink;

	add_timer(&mydata.timer);
}

// function for terminating program
static void kernel_quit_timer(unsigned long timeout){
	printk("program terminated!\n");

	// set quit flag
	quit_flag = 0;
}

// start stop watch when SW1 button pressed (interrupt)
irqreturn_t inter_handler1(int irq, void *dev_id, struct pt_regs *reg){
	printk("stopwatch started\n");

	// start stopwatch increase every 1 second
	del_timer_sync(&mydata.timer);
	mydata.timer.expires = jiffies + (1 * HZ);
	mydata.timer.data = (unsigned long)&mydata;
	mydata.timer.function = kernel_timer_blink;

	add_timer(&mydata.timer);

	return IRQ_HANDLED;
}

// pause stop watch when SW2 button pressed (interrupt)
irqreturn_t inter_handler2(int irq, void *dev_id, struct pt_regs *reg){
	printk("stopwatch paused\n");

	// remove timer handler
	del_timer_sync(&mydata.timer);

	return IRQ_HANDLED;
}

// reset stop watch when SW3 button pressed (interrupt)
irqreturn_t inter_handler3(int irq, void *dev_id, struct pt_regs *reg){
	printk("stopwatch reseted\n");

	// remove timer handler
	del_timer_sync(&mydata.timer);
	mydata.count = 0;

	return IRQ_HANDLED;
}

// terminate program when SW4 button pressed for 3 seconds (interrupt)
irqreturn_t inter_handler4(int irq, void *dev_id, struct pt_regs *reg){
	printk("stopwatch quiting\n");

	// check if user is pressing button for 3 seconds
	if(gpio_get_value(S5PV310_GPX2(4)))
		del_timer_sync(&quit_timer.timer);
	else{
		// add timer for terminating program
		quit_timer.timer.expires = jiffies + (3 * HZ);
		quit_timer.timer.function = kernel_quit_timer;
		add_timer(&quit_timer.timer);
	}

	return IRQ_HANDLED;
}

int stopwatch_open(struct inode *minode, struct file *mfile){
	int ret;

	if(stopwatch_usage != 0)
		return -EBUSY;

	// set to default value
	stopwatch_usage = 1;
	quit_flag = 1;
	mydata.count = 0;

	/*
	   *	SW2 : GPX2(0)
	   *	SW3 : GPX2(1)
	   *	SW4 : GPX2(2)
	   *	SW6 : GPX2(4)
	   *
	   *	PRESS-FALLING - 0
	   *	RELEASE_RISING - 1
	*/

	// set interrupt requests
	ret = request_irq(gpio_to_irq(S5PV310_GPX2(0)), &inter_handler1, IRQF_TRIGGER_FALLING, "X2.0", NULL);	// SW2
	ret = request_irq(gpio_to_irq(S5PV310_GPX2(1)), &inter_handler2, IRQF_TRIGGER_FALLING, "X2.1", NULL);	// SW3
	ret = request_irq(gpio_to_irq(S5PV310_GPX2(2)), &inter_handler3, IRQF_TRIGGER_FALLING, "X2.2", NULL);	// SW4
	ret = request_irq(gpio_to_irq(S5PV310_GPX2(4)), &inter_handler4, IRQF_TRIGGER_RISING|IRQF_TRIGGER_FALLING, "X2.3", NULL);	// SW6

	printk("stopwatch module open\n");

	return 0;
}

int stopwatch_release(struct inode *minode, struct file *mfile){
	stopwatch_usage = 0;

	// remove timers
	del_timer_sync(&mydata.timer);
	del_timer_sync(&quit_timer.timer);

	// fnd device off
	outb(0x00, (unsigned int)fnd_data2);
	
	// release interrupt
	free_irq(gpio_to_irq(S5PV310_GPX2(0)), NULL);
	free_irq(gpio_to_irq(S5PV310_GPX2(1)), NULL);
	free_irq(gpio_to_irq(S5PV310_GPX2(2)), NULL);
	free_irq(gpio_to_irq(S5PV310_GPX2(4)), NULL);

	printk("stopwatch module release\n");

	return 0;
}

char convertChar(int num){
	char chr = 0;

	switch(num){
		case 0:
			chr = 0x03;
			break;
		case 1:
			chr = 0x9F;
			break;
		case 2:
			chr = 0x25;
			break;
		case 3:
			chr = 0x0D;
			break;
		case 4:
			chr = 0x99;
			break;
		case 5:
			chr = 0x49;
			break;
		case 6:
			chr = 0xC1;
			break;
		case 7:
			chr = 0x1F;
			break;
		case 8:
			chr = 0x01;
			break;
		case 9:
			chr = 0x09;
			break;
	}
	return chr;
}

ssize_t stopwatch_write(struct file *inode, const short *gdata, size_t length, loff_t *off_what){
	char sel, dat;
	int min, sec;

	printk("stopwatch write entered\n");

	while(quit_flag){
		// count minutes and seconds
		min = mydata.count / 60;
		sec = mydata.count % 60;

		// if min reaches max value set to 0
		if(min == 60){
			mydata.count = 0;
			min = 0;
		}

		// print values on fnd device
		sel = 0x80;
		outb(sel, (unsigned int)fnd_data2);
		dat = convertChar(sec%10);
		outb(dat, (unsigned int)fnd_data);
		msleep(5);

		sel = 0x10;
		outb(sel, (unsigned int)fnd_data2);
		dat = convertChar(sec/10);
		outb(dat, (unsigned int)fnd_data);
		msleep(5);

		sel = 0x04;
		outb(sel, (unsigned int)fnd_data2);
		dat = convertChar(min%10);
		outb(dat, (unsigned int)fnd_data);
		msleep(5);

		sel = 0x02;
		outb(sel, (unsigned int)fnd_data2);
		dat = convertChar(min/10);
		outb(dat, (unsigned int)fnd_data);
		msleep(5);
	}

	return 0;
}

int __init stopwatch_init(void){
	int result;

	// gpio fnd driver local variables
	struct class *fnd_dev_class = NULL;
	struct device *fnd_dev = NULL;

	printk("stopwatch module init\n");

	// register device driver
	result = register_chrdev(DEV_MAJOR, DEV_NAME, &stopwatch_fops);
	if(result < 0){
		printk(KERN_WARNING"Can't get any major!\n");
		return result;
	}

	// FND driver initialization begin
	fnd_data = ioremap(FND_GPL2DAT, 0x01);
	fnd_data2 = ioremap(FND_GPE3DAT, 0x01);
	if(fnd_data == NULL){
		printk("FND ioremap failed\n");
		return -1;
	}

	// mapping address of fnd driver
	fnd_ctrl = ioremap(FND_GPL2CON, 0x04);
	fnd_ctrl2 = ioremap(FND_GPE3CON, 0x04);
	if(fnd_ctrl == NULL){
		printk("FND ioremap failed\n");
		return -1;
	} else{
		fnd_dev = device_create(fnd_dev_class, NULL, MKDEV(DEV_MAJOR, 0), NULL, DEV_NAME);
		if(fnd_dev != NULL){
			outl(0x11111111, (unsigned int)fnd_ctrl);
			outl(0x10010110, (unsigned int)fnd_ctrl2);
		} else
			printk("FND device create : failed\n");
	}

	outb(0xFF, (unsigned int)fnd_data);
	outb(0xFF, (unsigned int)fnd_data);
	// FND driver initialization ended

	// initialize timers
	init_timer(&(mydata.timer));
	init_timer(&(quit_timer.timer));

	printk("init module, /dev/stopwatch major : %d\n", DEV_MAJOR);

	return 0;
}

void __exit stopwatch_exit(void){
	// unregister device driver
	unregister_chrdev(DEV_MAJOR, DEV_NAME);
	printk("Stopwatch module removed.\n");
}

module_init(stopwatch_init);
module_exit(stopwatch_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("DangerCloz");
