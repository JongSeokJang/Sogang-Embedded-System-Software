/********************************************
  2014 Sogang Univ. Embedded System Software
  Assignment #2 (Due date. '14. 05. 24.)
  Made by Lee Jun-Ho (dangercloz@gmail.com)
 ********************************************/

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/ioport.h>
#include <linux/version.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/ioctl.h>
#include <mach/gpio.h>
#include <mach/regs-gpio.h>
#include <plat/gpio-cfg.h>

#include "./fpga_dot_font.h"

#define DEV_MAJOR 242	// dev driver major number
#define DEV_MINOR 0	// dev driver minor number
#define DEV_NAME "dev_driver"	// dev driver name

#define FND_MINOR 1	// gpio fnd driver minor number
#define LED_MINOR 2	// gpio led driver minor number
#define FPGA_FND_MINOR 3	// fpga fnd driver minor number
#define FPGA_LED_MINOR 4	// fpga led driver minor number
#define FPGA_DOT_MINOR 5	// fpga dot driver minor number
#define FPGA_TEXT_MINOR 6	// fpga text driver minor number
#define TIMER_MINOR 7	// timer module minor number

#define UON 0x00	// IOM
#define UOFF 0x01	// IOM
#define IOM_DEMO_ADDRESS 0x04000300

// fnd driver device address
#define FND_GPL2CON 0x11000100	// fnd pin configuration
#define FND_GPL2DAT 0x11000104	// fnd pin data
#define FND_GPE3CON 0x11400140	// fnd pin configuration
#define FND_GPE3DAT 0x11400144	// fnd pin data

// led driver device address
#define LED_GPBCON 0x11400040	// GPBCON register physical addr
#define LED_GPBDAT 0x11400044	// GPBDAT register physical addr

// fpga driver device address
#define IOM_FND_ADDRESS 0x04000004	// fnd physical address
#define IOM_LED_ADDRESS 0x04000016	// led physical address
#define IOM_FPGA_DOT_ADDRESS 0x04000210	// dot physical address
#define IOM_FPGA_TEXT_LCD_ADDRESS 0x04000100 // text lcd physical address

int dev_open(struct inode *, struct file *);
int dev_release(struct inode *, struct file *);
ssize_t dev_write(struct file *, const long *, size_t, loff_t *);
ssize_t dev_read(struct file *, char *, size_t, loff_t *);

int close_devices(void);
unsigned short fnd_write(const unsigned short *);
ssize_t led_write(const char *);
ssize_t fpga_led_write(const char *);
ssize_t fpga_fnd_write(const int *);
ssize_t fpga_dot_write(const char *);
int fpga_text_calculate(int);
ssize_t fpga_text_write(const char *);

// Global variable
static int dev_usage = 0;
static unsigned char *iom_demo_addr;

// fnd global variable
static unsigned char *fnd_data;
static unsigned int *fnd_ctrl;
static unsigned char *fnd_data2;
static unsigned int *fnd_ctrl2;

// led global variable
static char *led_buffer = NULL;
static unsigned char *led_data;
static unsigned int *led_ctrl;

// fpga global variable
static unsigned char *iom_fpga_fnd_addr;	// addr of fpga fnd
static unsigned char *iom_fpga_led_addr;	// addr of fpga led
static unsigned char *iom_fpga_dot_addr;	// addr of fpga dot
static unsigned char *iom_fpga_text_lcd_addr;	// addr of fpga text lcd

static struct file_operations dev_fops =
{
	.open = dev_open,
	.release = dev_release,
	.write = dev_write,
	.read = dev_read,
};

static struct struct_timer{
	struct timer_list timer;
	int count;	// start from 0
	int end_count;	// expire count
	int time;	// time interval
	unsigned short data;	// data of input
	unsigned char id[16];	// student id
	unsigned char name[16];	// student name
	int id_flag;	// direction flag of id text
	int name_flag;	// direction flag of name text
};

// timer global variable
struct struct_timer mytimer;

// open device driver
int dev_open(struct inode *minode, struct file *mfile){
	if(dev_usage != 0)
		return -EBUSY;

	dev_usage = 1;

	return 0;
}

// release device driver
int dev_release(struct inode *minode, struct file *mfile){
	dev_usage = 0;

	return 0;
}

// turn off devices after finished (set to default)
int close_devices(){
	// set device values to default
	fnd_write(0);
	led_write(0);
	fpga_led_write(0);
	fpga_fnd_write(0);
	fpga_dot_write(0);
	fpga_text_calculate(0);

	return 0;
}

static void kernel_timer_blink(unsigned long timeout){
	struct struct_timer *p_data = (struct sturct_timer *)timeout;
	char position, value;
	unsigned short temp_value;

	p_data->count++;	// increase count

	// pass data to send as parameter
	position = (char)(p_data->data>>8);
	value = (char)(p_data->data&0x00FF);

	// pass data to fpga devices
	fpga_fnd_write(p_data->end_count - p_data->count + 1);
	fpga_dot_write(value);
	fpga_led_write(value);
	fpga_text_calculate(1);

	// pass data to gpio device
	led_write(position);
	p_data->data = fnd_write(p_data->data);

	// check if count has reached limit
	if(p_data->count > p_data->end_count){
		close_devices();
		return;
	}

	// decode changed data to store
	temp_value = p_data->data;
	position = (char)(temp_value>>8);
	value = (char)(temp_value&0x00FF);

	// change value if maximum value is reached
	if(value >56){ // change value to 1 if value is over 8
		value = 49;
		position += 1;	// increase position

		// change position if it reaches at the end
		if(position > 52)
			position = 49;
	}

	// set modified timer data
	mytimer.data = position;		// new position
	mytimer.data = (mytimer.data<<8)|value;	// new value

	mytimer.timer.expires = get_jiffies_64() + (p_data->time * HZ)/10;
	mytimer.timer.data = (unsigned long)&mytimer;
	mytimer.timer.function = kernel_timer_blink;

	add_timer(&mytimer.timer);
}

unsigned short fnd_write(const unsigned short *gdata){
	const unsigned short *tmp = gdata;
	unsigned short fnd_buff = tmp;
	char fnd_sel, fnd_dat;
	unsigned char sel_bak = 0, dat_bak = 0;

	// decode data
	fnd_sel = (char)(fnd_buff>>8);
	fnd_dat = (char)(fnd_buff&0x00FF);

	// position of value to print
	switch(fnd_sel){
		case '1':
			sel_bak = 0x02;
			break;
		case '2':
			sel_bak = 0x04;
			break;
		case '3':
			sel_bak = 0x10;
			break;
		case '4':
			sel_bak = 0x80;
			break;
		default: // fnd off
			sel_bak = 0x00;
			break;
	}

	// data value to write
	switch(fnd_dat){
		case '1':
			dat_bak = 0x8C;
			break;
		case '2':
			dat_bak = 0x70;
			break;
		case '3':
			dat_bak = 0x8E;
			break;
		case '4':
			dat_bak = 0x72;
			break;
		case '5':
			dat_bak = 0x8E;
			break;
		case '6':
			dat_bak = 0xF2;
			break;
		case '7':
			dat_bak = 0xDE;
			break;
		case '8':
			dat_bak = 0xFA;
			break;
	}

	// encode data
	fnd_dat++;
	fnd_buff = fnd_sel;
	fnd_buff = (fnd_buff<<8)|fnd_dat;

	// print data to device
	outb(sel_bak, (unsigned int)fnd_data2);
	outb(dat_bak, (unsigned int)fnd_data);

	return fnd_buff;
}

ssize_t led_write(const char *gdata){
	const char led_buff = gdata;
	unsigned char tmp = 0;

	// select position of led to display
	switch(led_buff){
		case '1':
			tmp = 0xE0;
			break;
		case '2':
			tmp = 0xD0;
			break;
		case '3':
			tmp = 0xB0;
			break;
		case '4':
			tmp = 0x70;
			break;
		default: // led off
			tmp = 0xFF;
			break;
	}

	// print on led device
	outb(tmp, (unsigned int)led_data);

	return 0;
}

ssize_t fpga_led_write(const char *gdata){
	const char led_buff = gdata;
	unsigned char tmp = 0;

	// position of fpga led 1~8
	switch(led_buff){
		case '1':
			tmp = 128;	// 2^8 (D1)
			break;
		case '2':
			tmp = 64;	// 2^7 (D2)
			break;
		case '3':
			tmp = 32;	// 2^6 (D3)
			break;
		case '4':
			tmp = 16;	// 2^5 (D4)
			break;
		case '5':
			tmp = 8;	// 2^4 (D5)
			break;
		case '6':
			tmp = 4;	// 2^3 (D6)
			break;
		case '7':
			tmp = 2;	// 2^2 (D7)
			break;
		case '8':
			tmp = 1;	// 2^1 (D8)
			break;
		default:
			tmp = 0;	// 2^0
			break;
	}

	// print on fpga led device
	outb(tmp, (unsigned int)iom_fpga_led_addr);

	return 0;
}

ssize_t fpga_fnd_write(const int *gdata){
	int i;
	const int fnd_buff = gdata;
	unsigned char value[4] = {0,};

	// change integer to string
	sprintf(value, "%4d", fnd_buff);

	// print decreasing count on fpga fnd device
	for(i=0;i<4;i++)
		outb(value[i], (unsigned int)iom_fpga_fnd_addr + i);

	return 0;
}

ssize_t fpga_dot_write(const char *gdata){
	int i, num;
	const char dot_buff = gdata;
	unsigned char value[10] = {0,};

	// copy fpga dot data to local char string
	switch(dot_buff){
		case '1':
			num = 1;
			break;
		case '2':
			num = 2;
			break;
		case '3':
			num = 3;
			break;
		case '4':
			num = 4;
			break;
		case '5':
			num = 5;
			break;
		case '6':
			num = 6;
			break;
		case '7':
			num = 7;
			break;
		case '8':
			num = 8;
			break;
		default: // fpga dot off
			num = 0;
			break;
	}

	// copy dot data to local string
	for(i=0;i<10;i++)
		value[i] = fpga_number[num][i];

	// print current type of char on fpga dot device
	for(i=0;i<10;i++)
		outb(value[i], (unsigned int)iom_fpga_dot_addr + i);

	return 0;
}

int fpga_text_calculate(int flag){
	unsigned char tmp[32];
	int i;

	if(flag == 1){	// print id and name if flag is 1
		for(i=0;i<16;i++){
			tmp[i] = mytimer.id[i];
			tmp[i+16] = mytimer.name[i];
		}
	} else{	// copy NULL value to string for clear
		for(i=0;i<32;i++)
			tmp[i] = '\0';
	}

	// pass data to text lcd device
	fpga_text_write(tmp);

	if(flag == 1){	// if timer is running
		if(mytimer.id_flag == 1){
			for(i=0;i<15;i++) // move id string to right
				mytimer.id[i+1] = tmp[i];
			mytimer.id[0] = ' ';

			if(mytimer.id[15] != ' ') // change direction
				mytimer.id_flag = 0;
		} else{
			for(i=0;i<15;i++) // move id string to left
				mytimer.id[i] = tmp[i+1];
			mytimer.id[15] = ' ';

			if(mytimer.id[0] != ' ') // change direction
				mytimer.id_flag = 1;
		}

		if(mytimer.name_flag == 1){
			for(i=0;i<15;i++) // move name string to right
				mytimer.name[i+1] = tmp[i+16];
			mytimer.name[0] = ' ';

			if(mytimer.name[15] != ' ') // change direction
				mytimer.name_flag = 0;
		} else{
			for(i=0;i<15;i++) // move name string to left
				mytimer.name[i] = tmp[i+17];
			mytimer.name[15] = ' ';

			if(mytimer.name[0] != ' ') // change direction
				mytimer.name_flag = 1;
		}
	}

	return 0;
}

ssize_t fpga_text_write(const char *gdata){
	unsigned char *text_buff = gdata;
	int i;

	// print current data on fpga text lcd device
	for(i=0;i<32;i++)
		outb(text_buff[i], (unsigned int)iom_fpga_text_lcd_addr + i);

	return 0;
}

ssize_t dev_write(struct file *inode, const long *gdata, size_t length, loff_t *off_what){
	const long *tmp = gdata;
	long kernel_timer_buff = 0;
	char position, value;
	int time, number, i;
	unsigned char id[16] = "20091648        ";
	unsigned char name[16] = "Lee Jun Ho      ";

	// copy user space data to kernel space
	if(copy_from_user(&kernel_timer_buff, tmp, 4))
		return -EFAULT;

	// decode given input (4 byte data stream) using shift operand
	position = kernel_timer_buff>>24;	// position
	value = kernel_timer_buff>>16;		// value
	time = kernel_timer_buff<<16;		// time
	time = time>>24;
	number = kernel_timer_buff<<24;		// number
	number = number>>24;

	// set timer data
	mytimer.count = 0;
	mytimer.end_count = number;		// set end time
	mytimer.time = time;			// set time interval
	mytimer.data = position;		// encode data
	mytimer.data = (mytimer.data<<8)|value;	// encode data
	for(i=0;i<16;i++){ // copy string
		mytimer.id[i] = id[i];
		mytimer.name[i] = name[i];
	}
	mytimer.id_flag = 1;	// right direction to move
	mytimer.name_flag = 1;	// right direction to move

	// add timer
	del_timer_sync(&mytimer.timer);
	mytimer.timer.data = (unsigned long)&mytimer;
	mytimer.timer.function = kernel_timer_blink;
	add_timer(&mytimer.timer);

	return length;
}

ssize_t dev_read(struct file *inode, char *gdata, size_t length, loff_t *off_what){
	return length;
}

int __init dev_init(void){
	int result;

	// register device driver
	result = register_chrdev(DEV_MAJOR, DEV_NAME, &dev_fops);
	if(result < 0){	// error handler for failture
		printk(KERN_WARNING"Can't get any major!\n");
		return result;
	}

	/* FND driver initialization begin */
	struct class *fnd_dev_class = NULL;
	struct device *fnd_dev = NULL;

	fnd_data = ioremap(FND_GPL2DAT, 0x01);
	fnd_data2 = ioremap(FND_GPE3DAT, 0x01);
	if(fnd_data == NULL){	// error handler for failure
		printk("FND ioremap failed!\n");
		return -1;
	}

	fnd_ctrl = ioremap(FND_GPL2CON, 0x04);
	fnd_ctrl2 = ioremap(FND_GPE3CON, 0x04);
	if(fnd_ctrl == NULL){	// error handler for failure
		printk("FND ioremap failed!\n");
		return -1;
	} else{
		fnd_dev = device_create(fnd_dev_class, NULL, MKDEV(DEV_MAJOR, FND_MINOR), NULL, DEV_NAME);
		if(fnd_dev != NULL){
			outl(0x11111111, (unsigned int)fnd_ctrl);
			outl(0x10010110, (unsigned int)fnd_ctrl2);
		} else	// error handler for failure
			printk("FND device create : failed!\n");
	}
	outb(0xFF, (unsigned int)fnd_data);
	outb(0xFF, (unsigned int)fnd_data);
	/* FND driver initialization ended */

	/* LED driver initialization begin */
	unsigned int get_ctrl_io = 0;
	struct class *led_dev_class = NULL;
	struct device *led_dev = NULL;

	// LED control data -> memory mapping
	led_data = ioremap(LED_GPBDAT, 0x01);
	if(led_data == NULL){	// error handler for failure
		printk("LED ioremap failed!\n");
		return -1;
	}

	// mapping physical address
	led_ctrl = ioremap(LED_GPBCON, 0x04);
	if(led_ctrl == NULL){	// error handler for failure
		printk("LED ioremap failed!\n");
		return -1;
	} else{	// change register value if mapping succeed
		// get GPBCON data
		get_ctrl_io = inl((unsigned int)led_ctrl);
		led_dev = device_create(led_dev_class, NULL, MKDEV(DEV_MAJOR, LED_MINOR), NULL, DEV_NAME);
		led_buffer = (char *)kmalloc(1024, GFP_KERNEL);

		if(led_buffer != NULL)
			memset(led_buffer, 0, 1024);

		// set 4 upper byte of GPB pin register
		get_ctrl_io |= (0x11110000);

		// apply value to register
		outl(get_ctrl_io, (unsigned int)led_ctrl);
	}
	outb(0xF0, (unsigned int)led_data);
	/* LED driver initialization ended */

	/* FPGA drivers initialization */
	iom_fpga_fnd_addr = ioremap(IOM_FND_ADDRESS, 0x4);	// fpga fnd mapping
	iom_fpga_led_addr = ioremap(IOM_LED_ADDRESS, 0x1);	// fpga led mapping
	iom_fpga_dot_addr = ioremap(IOM_FPGA_DOT_ADDRESS, 0x10);	// fpga dot mapping
	iom_fpga_text_lcd_addr = ioremap(IOM_FPGA_TEXT_LCD_ADDRESS, 0x20);	// fpga text mapping

	// fpga common factors
	iom_demo_addr = ioremap(IOM_DEMO_ADDRESS, 0x1);
	outb(UON, (unsigned int)iom_demo_addr);

	/* TIMER driver initialization begin */
	struct class *kernel_timer_dev_class = NULL;
	struct device *kernel_timer_dev = NULL;

	kernel_timer_dev = device_create(kernel_timer_dev_class, NULL, MKDEV(DEV_MAJOR, TIMER_MINOR), NULL, DEV_NAME);
	init_timer(&(mytimer.timer));
	/* TIMER driver initialization ended */

	printk("init module, /dev/%s major : %d\n", DEV_NAME, DEV_MAJOR);
	return 0;
}

void __exit dev_exit(void){
	/* FND driver free */
	outb(0xFF, (unsigned int)fnd_data);
	iounmap(fnd_data);	iounmap(fnd_data2);
	iounmap(fnd_ctrl);	iounmap(fnd_ctrl2);

	/* LED driver free */
	outb(0xF0, (unsigned int)led_data);
	iounmap(led_data);	iounmap(led_ctrl);

	/* FPGA drivers free */
	iounmap(iom_fpga_fnd_addr);	// FND
	iounmap(iom_fpga_led_addr);	// LED
	iounmap(iom_fpga_dot_addr);	// DOT
	iounmap(iom_fpga_text_lcd_addr);	// TEXT
	iounmap(iom_demo_addr);	// FPGA common factor

	/* TIMER driver free */
	del_timer_sync(&mytimer.timer);

	// unregister device driver
	unregister_chrdev(DEV_MAJOR, DEV_NAME);
	printk("dev driver module removed.\n");
}

module_init(dev_init);
module_exit(dev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lee, Jun-Ho");
