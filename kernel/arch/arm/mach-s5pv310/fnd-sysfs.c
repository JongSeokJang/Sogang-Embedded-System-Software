//[*]--------------------------------------------------------------------------------------------------[*]
//
//
// 
//  Hardkernel Dev Board : FND(7-Segment) sysfs driver (charles.park)
//  2011.10.07
// 
//
//[*]--------------------------------------------------------------------------------------------------[*]
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/sysfs.h>
#include <linux/input.h>
#include <linux/gpio.h>

#include <mach/gpio.h>
#include <mach/regs-gpio.h>
#include <plat/gpio-cfg.h>

//[*]--------------------------------------------------------------------------------------------------[*]
#define	DEBUG_PM_MSG

//[*]--------------------------------------------------------------------------------------------------[*]
//   Control GPIO Define
//[*]--------------------------------------------------------------------------------------------------[*]
// GPIO Index Define
enum	{
	SEGMENT_A,			//   7-Segment LED Decription
	SEGMENT_B,			//        +- [A] -+        
	SEGMENT_C,			//        |       |        
	SEGMENT_D,			//    [F] |       | [B]    
	SEGMENT_E,			//        +- [G] -+        
	SEGMENT_F,			//    [E] |       | [C]    
	SEGMENT_G,			//        |       |        
	SEGMENT_DP,			//        +- [D] -+  . [DP]          
	
	GPIO_INDEX_END
};

static struct {
	int		gpio_index;		// Control Index
	int 	gpio;			// GPIO Number
	char	*name;			// GPIO Name == sysfs attr name (must)
	bool 	output;			// 1 = Output, 0 = Input
	int 	value;			// Default Value(only for output)
	int		pud;			// Pull up/down register setting : S3C_GPIO_PULL_DOWN, UP, NONE
} sControlGpios[] = {
	{	SEGMENT_A,	S5PV310_GPL2(0), "segment_a",	1, 1, S3C_GPIO_PULL_UP	},
	{	SEGMENT_B,	S5PV310_GPL2(1), "segment_b",	1, 1, S3C_GPIO_PULL_UP	},
	{	SEGMENT_C,	S5PV310_GPL2(2), "segment_c",	1, 1, S3C_GPIO_PULL_UP	},
	{	SEGMENT_D,	S5PV310_GPL2(3), "segment_d",	1, 1, S3C_GPIO_PULL_UP	},
	{	SEGMENT_E,	S5PV310_GPL2(4), "segment_e",	1, 1, S3C_GPIO_PULL_UP	},
	{	SEGMENT_F,	S5PV310_GPL2(5), "segment_f",	1, 1, S3C_GPIO_PULL_UP	},
	{	SEGMENT_G,	S5PV310_GPL2(6), "segment_g",	1, 1, S3C_GPIO_PULL_UP	},
	{	SEGMENT_DP,	S5PV310_GPL2(7), "segment_dp",	1, 1, S3C_GPIO_PULL_UP	},
};

//[*]--------------------------------------------------------------------------------------------------[*]
//[*]--------------------------------------------------------------------------------------------------[*]
//   function prototype define
//[*]--------------------------------------------------------------------------------------------------[*]
static 	int		fnd_sysfs_resume	(struct platform_device *dev);
static 	int		fnd_sysfs_suspend	(struct platform_device *dev, pm_message_t state);
static	int		fnd_sysfs_probe		(struct platform_device *pdev);
static	int		fnd_sysfs_remove	(struct platform_device *pdev);	

static 	int 	__init fnd_sysfs_init(void);
static 	void 	__exit fnd_sysfs_exit(void);

//[*]--------------------------------------------------------------------------------------------------[*]
static struct platform_driver fnd_sysfs_driver = {
	.driver = {
		.name = "fnd-sysfs",
		.owner = THIS_MODULE,
	},
	.probe 		= fnd_sysfs_probe,
	.remove 	= fnd_sysfs_remove,
	.suspend	= fnd_sysfs_suspend,
	.resume		= fnd_sysfs_resume,
};

//[*]--------------------------------------------------------------------------------------------------[*]
module_init(fnd_sysfs_init);
module_exit(fnd_sysfs_exit);

//[*]--------------------------------------------------------------------------------------------------[*]

MODULE_DESCRIPTION("SYSFS driver for Hardkernel-Dev board");
MODULE_AUTHOR("Hard-Kernel");
MODULE_LICENSE("GPL");

//[*]--------------------------------------------------------------------------------------------------[*]
//[*]--------------------------------------------------------------------------------------------------[*]
const	unsigned char	HEX_DISP_DATA[] = {
//  '0'   '1'   '2'   '3'   '4'   '5'   '6'   '7'   '8'   '9'   'a'   'b'   'c'   'd'   'e'   'f'
	0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xD8, 0x80, 0x90, 0x08, 0x03, 0x46, 0x21, 0x06, 0x0E
};

typedef struct	segment__t	{
	unsigned char	a	:1;	//lsb
	unsigned char	b	:1;
	unsigned char	c	:1;
	unsigned char	d	:1;
	unsigned char	e	:1;
	unsigned char	f	:1;
	unsigned char	g	:1;
	unsigned char	dp	:1;
}	segment_t;

typedef union	segment__u	{
	unsigned char	byte;
	segment_t		bits;
}	segment_u;


//[*]--------------------------------------------------------------------------------------------------[*]
#define	RUNNING_STATE_DATA_SIZE			6
#define	RUNNING_STATE_UPDATE_TIME		(HZ/5)

const	unsigned char	RUNNING_STATE_DATA[RUNNING_STATE_DATA_SIZE] = {	0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF };

static	unsigned char		RunStateDisplay = true;
static	struct timer_list	RunStateTimer;

//[*]--------------------------------------------------------------------------------------------------[*]
//
//	Function prototype define
//
//[*]--------------------------------------------------------------------------------------------------[*]
static 	void	run_state_timer_handler	(unsigned long data);
static	void	set_fnd_byte_data		(unsigned char	byte_data);

//[*]--------------------------------------------------------------------------------------------------[*]
//
//   sysfs function prototype define
//
//[*]--------------------------------------------------------------------------------------------------[*]
static	ssize_t show_gpio		(struct device *dev, struct device_attribute *attr, char *buf);
static 	ssize_t set_gpio		(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);
static	ssize_t show_fnd_data	(struct device *dev, struct device_attribute *attr, char *buf);
static 	ssize_t set_fnd_data	(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);
//[*]--------------------------------------------------------------------------------------------------[*]
//[*]--------------------------------------------------------------------------------------------------[*]
static	DEVICE_ATTR(segment_a,	S_IRWXUGO, show_gpio, set_gpio);
static	DEVICE_ATTR(segment_b,	S_IRWXUGO, show_gpio, set_gpio);
static	DEVICE_ATTR(segment_c,	S_IRWXUGO, show_gpio, set_gpio);
static	DEVICE_ATTR(segment_d,	S_IRWXUGO, show_gpio, set_gpio);
static	DEVICE_ATTR(segment_e,	S_IRWXUGO, show_gpio, set_gpio);
static	DEVICE_ATTR(segment_f,	S_IRWXUGO, show_gpio, set_gpio);
static	DEVICE_ATTR(segment_g,	S_IRWXUGO, show_gpio, set_gpio);
static	DEVICE_ATTR(segment_dp,	S_IRWXUGO, show_gpio, set_gpio);

static	DEVICE_ATTR(fnd_data,	S_IRWXUGO, show_fnd_data, set_fnd_data);
//[*]--------------------------------------------------------------------------------------------------[*]
//static	DEVICE_ATTR(hexdisp,	S_IRWXUGO, show_hdmi, NULL);
//[*]--------------------------------------------------------------------------------------------------[*]
//[*]--------------------------------------------------------------------------------------------------[*]
static struct attribute *fnd_sysfs_entries[] = {
	&dev_attr_segment_a.attr,
	&dev_attr_segment_b.attr,
	&dev_attr_segment_c.attr,
	&dev_attr_segment_d.attr,
	&dev_attr_segment_e.attr,
	&dev_attr_segment_f.attr,
	&dev_attr_segment_g.attr,
	&dev_attr_segment_dp.attr,
	&dev_attr_fnd_data.attr,
	NULL
};

static struct attribute_group fnd_sysfs_attr_group = {
	.name   = NULL,
	.attrs  = fnd_sysfs_entries,
};

//[*]--------------------------------------------------------------------------------------------------[*]
static void	run_state_timer_handler(unsigned long data)
{
    unsigned long 			flags;
    static	unsigned char	state_cnt = 0;

    local_irq_save(flags);

	if(RunStateDisplay)		{
		set_fnd_byte_data(RUNNING_STATE_DATA[state_cnt]);
		
		if(state_cnt)	{	state_cnt++;	state_cnt %= RUNNING_STATE_DATA_SIZE;	}
		else				state_cnt++;
	}

	mod_timer(&RunStateTimer, jiffies + RUNNING_STATE_UPDATE_TIME);
    local_irq_restore(flags);
}

//[*]--------------------------------------------------------------------------------------------------[*]
//[*]--------------------------------------------------------------------------------------------------[*]
//[*]--------------------------------------------------------------------------------------------------[*]
static 	ssize_t show_gpio		(struct device *dev, struct device_attribute *attr, char *buf)
{
	int	i;

	for (i = 0; i < ARRAY_SIZE(sControlGpios); i++) {
		if(!strcmp(sControlGpios[i].name, attr->attr.name))
			return	sprintf(buf, "%d\n", (gpio_get_value(sControlGpios[i].gpio) ? 1 : 0));
	}
	
	return	sprintf(buf, "ERROR! : Not found gpio!\n");
}

//[*]--------------------------------------------------------------------------------------------------[*]
static 	ssize_t set_gpio		(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    unsigned int	val, i;

    if(!(sscanf(buf, "%d\n", &val))) 	{
		RunStateDisplay = true;   	return	-EINVAL;
    }

	RunStateDisplay = false;

	for (i = 0; i < ARRAY_SIZE(sControlGpios); i++) {
		if(!strcmp(sControlGpios[i].name, attr->attr.name))	{
			if(sControlGpios[i].output)
				gpio_set_value(sControlGpios[i].gpio, ((val != 0) ? 1 : 0));
			else
				printk("This GPIO Configuration is INPUT!!\n");
		    return count;
		}
	}

	printk("ERROR! : Not found gpio!\n");
    return 0;
}

//[*]--------------------------------------------------------------------------------------------------[*]
static 	ssize_t show_fnd_data		(struct device *dev, struct device_attribute *attr, char *buf)
{
    segment_u		segment;

	RunStateDisplay = false;

	segment.byte = 0;
	segment.bits.a  = (gpio_get_value(sControlGpios[SEGMENT_A].gpio)  ? 1 : 0);
	segment.bits.b  = (gpio_get_value(sControlGpios[SEGMENT_B].gpio)  ? 1 : 0);
	segment.bits.c  = (gpio_get_value(sControlGpios[SEGMENT_C].gpio)  ? 1 : 0);
	segment.bits.d  = (gpio_get_value(sControlGpios[SEGMENT_D].gpio)  ? 1 : 0);
	segment.bits.e  = (gpio_get_value(sControlGpios[SEGMENT_E].gpio)  ? 1 : 0);
	segment.bits.f  = (gpio_get_value(sControlGpios[SEGMENT_F].gpio)  ? 1 : 0);
	segment.bits.g  = (gpio_get_value(sControlGpios[SEGMENT_G].gpio)  ? 1 : 0);
	segment.bits.dp = (gpio_get_value(sControlGpios[SEGMENT_DP].gpio) ? 1 : 0);
		
	return	sprintf(buf, "FND [A = %d] [B = %d] [C = %d] [D = %d] [E = %d] [F = %d] [G = %d] [DP = %d]\n",
				segment.bits.a,	segment.bits.b,	segment.bits.c,	segment.bits.d,	segment.bits.e,	segment.bits.f,	segment.bits.g,	segment.bits.dp);
}

//[*]--------------------------------------------------------------------------------------------------[*]
static	void	set_fnd_byte_data	(unsigned char	byte_data)
{
	segment_u		segment;
	
	segment.byte = byte_data;
	
	gpio_set_value(sControlGpios[SEGMENT_A].gpio,  ((segment.bits.a  != 0) ? 1 : 0));
	gpio_set_value(sControlGpios[SEGMENT_B].gpio,  ((segment.bits.b  != 0) ? 1 : 0));
	gpio_set_value(sControlGpios[SEGMENT_C].gpio,  ((segment.bits.c  != 0) ? 1 : 0));
	gpio_set_value(sControlGpios[SEGMENT_D].gpio,  ((segment.bits.d  != 0) ? 1 : 0));
	gpio_set_value(sControlGpios[SEGMENT_E].gpio,  ((segment.bits.e  != 0) ? 1 : 0));
	gpio_set_value(sControlGpios[SEGMENT_F].gpio,  ((segment.bits.f  != 0) ? 1 : 0));
	gpio_set_value(sControlGpios[SEGMENT_G].gpio,  ((segment.bits.g  != 0) ? 1 : 0));
	gpio_set_value(sControlGpios[SEGMENT_DP].gpio, ((segment.bits.dp != 0) ? 1 : 0));
}

//[*]--------------------------------------------------------------------------------------------------[*]
static 	ssize_t set_fnd_data		(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    unsigned int	val;

    if(!(sscanf(buf, "%d\n", &val))) 	goto error;

	if(val < 0 || val > 15)				goto error;
	
	RunStateDisplay = false;

	set_fnd_byte_data(HEX_DISP_DATA[val]);
	
	return	count;	

error:
	RunStateDisplay = true;

	printk("FND Display Data Error!. Fnd set data range [0 - 15]\n");	
	return	-EINVAL;
	
}

//[*]--------------------------------------------------------------------------------------------------[*]
//[*]--------------------------------------------------------------------------------------------------[*]
static int	fnd_sysfs_resume(struct platform_device *dev)
{
	#if	defined(DEBUG_PM_MSG)
		printk("%s\n", __FUNCTION__);
	#endif

    return  0;
}

//[*]--------------------------------------------------------------------------------------------------[*]
static int	fnd_sysfs_suspend(struct platform_device *dev, pm_message_t state)
{
	#if	defined(DEBUG_PM_MSG)
		printk("%s\n", __FUNCTION__);
	#endif
	
    return  0;
}

//[*]--------------------------------------------------------------------------------------------------[*]
static	int		fnd_sysfs_probe		(struct platform_device *pdev)	
{
	int	i;

	// Control GPIO Init
	for (i = 0; i < ARRAY_SIZE(sControlGpios); i++) {
		if(gpio_request(sControlGpios[i].gpio, sControlGpios[i].name))	{
			printk("%s : %s gpio reqest err!\n", __FUNCTION__, sControlGpios[i].name);
		}
		else	{
			if(sControlGpios[i].output)		gpio_direction_output	(sControlGpios[i].gpio, sControlGpios[i].value);
			else							gpio_direction_input	(sControlGpios[i].gpio);

			s3c_gpio_setpull		(sControlGpios[i].gpio, sControlGpios[i].pud);
		}
	}

	init_timer(&RunStateTimer);
	RunStateTimer.function 	= run_state_timer_handler;
	RunStateTimer.expires 	= jiffies + RUNNING_STATE_UPDATE_TIME;
	add_timer(&RunStateTimer);

	printk("--------------------------------------------------------\n");
	printk("\n%s : FND (7-Segment) SYSFS Registered.\n\n", __FUNCTION__);
	printk("--------------------------------------------------------\n");

	return	sysfs_create_group(&pdev->dev.kobj, &fnd_sysfs_attr_group);
}

//[*]--------------------------------------------------------------------------------------------------[*]
static	int		fnd_sysfs_remove		(struct platform_device *pdev)	
{
	int	i;
	
	for (i = 0; i < ARRAY_SIZE(sControlGpios); i++) 	gpio_free(sControlGpios[i].gpio);

    sysfs_remove_group(&pdev->dev.kobj, &fnd_sysfs_attr_group);
    
    del_timer(&RunStateTimer);
    
    return	0;
}
//[*]--------------------------------------------------------------------------------------------------[*]
static int __init fnd_sysfs_init(void)
{	
    return platform_driver_register(&fnd_sysfs_driver);
}

//[*]--------------------------------------------------------------------------------------------------[*]
static void __exit fnd_sysfs_exit(void)
{
    platform_driver_unregister(&fnd_sysfs_driver);
}

//[*]--------------------------------------------------------------------------------------------------[*]
