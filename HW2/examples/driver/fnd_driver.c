/* FND Ioremap Control
FILE : fnd_driver.c 
AUTH : Huins, HSH*/

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
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

#define FND_MAJOR 241           // fnd device minor number
#define FND_MINOR 0         // fnd device minor number
#define FND_NAME "fnd_driver"   // fnd device name
#define FND_GPL2CON 0x11000100  // Pin Configuration
#define FND_GPL2DAT 0x11000104  // Pin Data
#define FND_GPE3CON 0x11400140  // Pin Configuration
#define FND_GPE3DAT 0x11400144  // Pin DAta

int fnd_open(struct inode *, struct file *);
int fnd_release(struct inode *, struct file *);
ssize_t fnd_write(struct file *, const short *, size_t, loff_t *);

//Global variable
static int fnd_usage = 0;
static unsigned char *fnd_data;
static unsigned int *fnd_ctrl;
static unsigned char *fnd_data2;
static unsigned int *fnd_ctrl2;
static struct file_operations fnd_fops =
{
    .open       = fnd_open,
    .write      = fnd_write,
    .release    = fnd_release,
};

int fnd_open(struct inode *minode, struct file *mfile)
{
    if(fnd_usage != 0)
        return -EBUSY;

    fnd_usage = 1;

    return 0;
}
int fnd_release(struct inode *minode, struct file *mfile)
{
    fnd_usage = 0;
    return 0;
}

ssize_t fnd_write(struct file *inode, const short *gdata, size_t length, loff_t *off_what)
{
    const short *tmp = gdata;
    unsigned short fnd_buff=0;

    char fnd_sel;
    char fnd_dat;

    if (copy_from_user(&fnd_buff, tmp, length))
        return -EFAULT;

    fnd_sel=(char)(fnd_buff>>8);
    fnd_dat=(char)(fnd_buff&0x00FF);

    printk("FND Secect : %d\n",fnd_sel);
    printk("Fnd Data : %d\n",fnd_dat);
	

    outb (fnd_sel,(unsigned int)fnd_data2);
    outb (fnd_dat, (unsigned int)fnd_data);
    return length;
}

int __init fnd_init(void)
{
    int result;
    struct class *fnd_dev_class=NULL;
    struct device *fnd_dev=NULL;

    result = register_chrdev(FND_MAJOR, FND_NAME, &fnd_fops);
    if(result <0) {
        printk(KERN_WARNING"Can't get any major!\n");
        return result;
    }

    fnd_data = ioremap(FND_GPL2DAT, 0x01);
    fnd_data2 = ioremap(FND_GPE3DAT, 0x01);
    if(fnd_data==NULL)
    {
        printk("ioremap failed!\n");
        return -1;
    }

    fnd_ctrl = ioremap(FND_GPL2CON, 0x04);
    fnd_ctrl2 = ioremap(FND_GPE3CON, 0x04);
    if(fnd_ctrl==NULL)
    {
        printk("ioremap failed!\n");
        return -1;
    } else {
        fnd_dev = device_create(fnd_dev_class,NULL,MKDEV(FND_MAJOR,0),NULL,FND_NAME);
        if(fnd_dev=!NULL)
        {
            outl(0x11111111,(unsigned int)fnd_ctrl);
	    outl(0x10010110,(unsigned int)fnd_ctrl2);
        }
        else {
            printk("device_reate : failed!\n");
        }
    }
    printk("init module, /dev/fnd_driver major : %d\n", FND_MAJOR);
    outb(0xFF, (unsigned int)fnd_data);
    outb(0xFF, (unsigned int)fnd_data);

    return 0;
}

void __exit fnd_exit(void)
{
    outb(0xFF, (unsigned int)fnd_data);
    iounmap(fnd_data);
    iounmap(fnd_data2);
    iounmap(fnd_ctrl);
    iounmap(fnd_ctrl2);
    unregister_chrdev(FND_MAJOR, FND_NAME);
    printk("FND module removed.\n");
}

module_init(fnd_init);
module_exit(fnd_exit);

MODULE_LICENSE ("GPL");
MODULE_AUTHOR ("Huins HSH");
