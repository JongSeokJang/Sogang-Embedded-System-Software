#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <asm/irq.h>
#include <mach/gpio.h>
#include <mach/regs-gpio.h>
#include <linux/platform_device.h>
#include <asm/gpio.h>
#include <plat/gpio-cfg.h>
#include <linux/wait.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/ioport.h>
#include <linux/version.h>

#define DEV_MAJOR 246
#define DEV_NAME "inter"

wait_queue_head_t wq_write;
DECLARE_WAIT_QUEUE_HEAD(wq_write);
int inter_open(struct inode *, struct file *);
int inter_release(struct inode *, struct file *);
ssize_t inter_write(struct file *, const unsigned long *, size_t, loff_t *);

irqreturn_t inter_handler(int irq, void* dev_id, struct pt_regs* reg);

static int inter_usage=0;
int interruptCount=0;
static struct file_operations inter_fops =
{
	.open = inter_open,
	.write = inter_write,
	.release = inter_release,
};

irqreturn_t inter_handler1(int irq, void* dev_id, struct pt_regs* reg){
	printk("interrupt1!!! = %x\n", gpio_get_value(S5PV310_GPX2(0)));
	return IRQ_HANDLED;
}

irqreturn_t inter_handler2(int irq, void* dev_id, struct pt_regs* reg){
	printk("interrupt2!!! = %x\n", gpio_get_value(S5PV310_GPX2(1)));
	return IRQ_HANDLED;
}

irqreturn_t inter_handler3(int irq, void* dev_id, struct pt_regs* reg){
	printk("interrupt3!!! = %x\n", gpio_get_value(S5PV310_GPX2(2)));
	return IRQ_HANDLED;
}

irqreturn_t inter_handler4(int irq, void* dev_id, struct pt_regs* reg){
	printk("interrupt4!!! = %x\n", gpio_get_value(S5PV310_GPX2(3)));
	return IRQ_HANDLED;
}

irqreturn_t inter_handler5(int irq, void* dev_id, struct pt_regs* reg){
	printk("interrupt5!!! = %x\n", gpio_get_value(S5PV310_GPX2(4)));
	if(++interruptCount>=3){
		interruptCount=0;
		wake_up_interruptible(&wq_write);
		printk("wake_up\n");
	}
	return IRQ_HANDLED;
}

irqreturn_t inter_handler6(int irq, void* dev_id, struct pt_regs* reg){
	printk("interrupt6!!! = %x\n", gpio_get_value(S5PV310_GPX2(5)));
	return IRQ_HANDLED;
}

irqreturn_t inter_handler7(int irq, void* dev_id, struct pt_regs* reg){
	printk("interrupt7!!! = %x\n", gpio_get_value(S5PV310_GPX0(1)));
	return IRQ_HANDLED;
}

int inter_open(struct inode *minode, struct file *mfile){
	int ret;
	if(inter_usage!=0)
			return -EBUSY;
	inter_usage=1;
	/*
	*	SW2 : GPX2(0)
	*	SW3 : GPX2(1)
	*	SW4 : GPX2(2)
	*	SW6 : GPX2(4)
	*
	*	VOL+ : GPX2(5)
	*	POWER : GPX0(1)	
	*	VOL- : GPX0(1)
	*
	*	SW2,3,4,6 : PRESS-FALLING-0 / RELEASE-RISING-1
	*	VOL -, +  : PRESS-RISING-1 / RELEASE-FALLING-0		
	*	POWER	  : PRESS-FALLING-0 / RELEASE-RISING-1
	*/
	ret=request_irq(gpio_to_irq(S5PV310_GPX2(0)), &inter_handler1, IRQF_TRIGGER_RISING|IRQF_TRIGGER_FALLING, "X2.0", NULL);//SW2
	ret=request_irq(gpio_to_irq(S5PV310_GPX2(1)), &inter_handler2, IRQF_TRIGGER_RISING, "X2.1", NULL);//SW3
	ret=request_irq(gpio_to_irq(S5PV310_GPX2(2)), &inter_handler3, IRQF_TRIGGER_FALLING, "X2.2", NULL);//SW4
	ret=request_irq(gpio_to_irq(S5PV310_GPX2(3)), &inter_handler4, IRQF_TRIGGER_RISING,"X2.3", NULL);//VOL-
	ret=request_irq(gpio_to_irq(S5PV310_GPX2(4)), &inter_handler5, IRQF_TRIGGER_FALLING, "X2.4", NULL);//SW6
	ret=request_irq(gpio_to_irq(S5PV310_GPX2(5)), &inter_handler6, IRQF_TRIGGER_RISING|IRQF_TRIGGER_FALLING, "X2.5", NULL);//VOL+
	ret=request_irq(gpio_to_irq(S5PV310_GPX0(1)), &inter_handler7, IRQF_TRIGGER_RISING, "X0.1", NULL);//POWER
	return 0;
}

int inter_release(struct inode *minode, struct file *mfile){
	inter_usage=0;
	
	free_irq(gpio_to_irq(S5PV310_GPX2(0)), NULL);
	free_irq(gpio_to_irq(S5PV310_GPX2(1)), NULL);
	free_irq(gpio_to_irq(S5PV310_GPX2(2)), NULL);
	free_irq(gpio_to_irq(S5PV310_GPX2(3)), NULL);
	free_irq(gpio_to_irq(S5PV310_GPX2(4)), NULL);
	free_irq(gpio_to_irq(S5PV310_GPX2(5)), NULL);
	free_irq(gpio_to_irq(S5PV310_GPX0(1)), NULL);
	return 0;
}

ssize_t inter_write(struct file *inode, const unsigned long *gdata, size_t length, loff_t *off_what){
	if(interruptCount==0){
		printk("sleep on\n");
		interruptible_sleep_on(&wq_write);
	}
	printk("write\n");

	return length;
}

int __init inter_init(void){
		int result;
		result = register_chrdev(DEV_MAJOR,DEV_NAME, &inter_fops);
		interruptCount=0;

		if(result <0) {
			printk(KERN_WARNING"Can't get any major!\n");
			return result;
		}

		return 0;
}

void __exit inter_exit(void){
		unregister_chrdev(DEV_MAJOR,DEV_NAME);
}

module_init(inter_init);
module_exit(inter_exit);

MODULE_LICENSE("GPL");
