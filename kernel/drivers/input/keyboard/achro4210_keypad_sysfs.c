//  ACHRO4210 Board : ACHRO4210 Keypad Interface driver
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

#include <mach/regs-gpio.h>
#include <mach/regs-clock.h>

#include <asm/gpio.h>
#include <plat/gpio-cfg.h>
#include <plat/regs-keypad.h>


#include "achro4210_keypad.h"


int		achro4210_keypad_sysfs_create		(struct platform_device *pdev);
void	achro4210_keypad_sysfs_remove		(struct platform_device *pdev);	

static	ssize_t show_system_off			(struct device *dev, struct device_attribute *attr, char *buf);
static	ssize_t set_system_off			(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);
static	ssize_t show_sampling_rate		(struct device *dev, struct device_attribute *attr, char *buf);
static	ssize_t set_sampling_rate		(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);
static	ssize_t show_poweroff_control	(struct device *dev, struct device_attribute *attr, char *buf);
static 	ssize_t set_poweroff_control	(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);
static	ssize_t show_gps_onoff	(struct device *dev, struct device_attribute *attr, char *buf);
static 	ssize_t set_gps_onoff	(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);
static	ssize_t show_gps_wakeup	(struct device *dev, struct device_attribute *attr, char *buf);
static 	ssize_t set_gps_wakeup	(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);

static	DEVICE_ATTR(system_off, S_IRWXUGO, show_system_off, set_system_off);
static	DEVICE_ATTR(sampling_rate, S_IRWXUGO, show_sampling_rate, set_sampling_rate);
static	DEVICE_ATTR(poweroff_control, S_IRWXUGO, show_poweroff_control, set_poweroff_control);
static	DEVICE_ATTR(gps_onoff, S_IRWXUGO, show_gps_onoff, set_gps_onoff);
static	DEVICE_ATTR(gps_wakeup, S_IRWXUGO, show_gps_wakeup, set_gps_wakeup);

typedef	struct	combo_module__t	{
	unsigned char		status_wifi;
	unsigned char		status_wifi_wakeup;
	unsigned char		status_gps;
	unsigned char		status_gps_wakeup;
	unsigned char		status_bt;
	unsigned char		status_bt_wakeup;

#if defined(CONFIG_HAS_WAKELOCK)
	#if defined(COMBO_MODULE_WAKELOCK)
		struct wake_lock	wakelock;
	#endif
#endif
}	combo_module_t	;
static	combo_module_t	combo_module;

static struct attribute *achro4210_keypad_sysfs_entries[] = {
	&dev_attr_system_off.attr,
	&dev_attr_sampling_rate.attr,
	&dev_attr_poweroff_control.attr,
	&dev_attr_gps_onoff.attr,
	&dev_attr_gps_wakeup.attr,
	NULL
};

static struct attribute_group achro4210_keypad_attr_group = {
	.name   = NULL,
	.attrs  = achro4210_keypad_sysfs_entries,
};

static 	ssize_t show_system_off			(struct device *dev, struct device_attribute *attr, char *buf)
{
	return	sprintf(buf, "%s : unsupport this function!\n", __FUNCTION__);
}


static 	ssize_t set_system_off			(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    unsigned long flags;

    local_irq_save(flags);

	if(!strcmp(buf, "on\n"))	{
		// PS_HOLD Enable (LOW-> HIGH)
		printk(KERN_EMERG "%s : setting GPIO_PDA_PS_HOLD low.\n", __func__);
		(*(unsigned long *)(S5PV310_VA_PMU + 0x330C)) = 0x5200;
	}

    local_irq_restore(flags);

    return count;
}


static 	ssize_t show_sampling_rate		(struct device *dev, struct device_attribute *attr, char *buf)
{
	switch(achro4210_keypad.sampling_rate)	{
		default	:
			achro4210_keypad.sampling_rate = 0;
		case	0:
			return	sprintf(buf, "10 msec\n");
		case	1:
			return	sprintf(buf, "20 msec\n");
		case	2:
			return	sprintf(buf, "50 msec\n");
	}
}

static 	ssize_t set_sampling_rate		(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    unsigned long 	flags;
    unsigned int	val;

    if(!(sscanf(buf, "%u\n", &val))) 	return	-EINVAL;
    
    local_irq_save(flags);
    
    if		(val > 20)
    	achro4210_keypad.sampling_rate = 2;
    else if	(val > 10)
    	achro4210_keypad.sampling_rate = 1;
    else
    	achro4210_keypad.sampling_rate = 0;

    local_irq_restore(flags);

    return count;
}

static 	ssize_t show_poweroff_control	(struct device *dev, struct device_attribute *attr, char *buf)
{
	switch(achro4210_keypad.poweroff_time)	{
		default	:
			achro4210_keypad.poweroff_time = 0;
		case	0:
			return	sprintf(buf, "1 sec\n");
		case	1:
			return	sprintf(buf, "3 sec\n");
		case	2:
			return	sprintf(buf, "5 sec\n");
	}
}


static 	ssize_t set_poweroff_control	(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    unsigned long 	flags;
    unsigned int	val;

    if(!(sscanf(buf, "%u\n", &val)))	return	-EINVAL;

    local_irq_save(flags);
    
    if		(val > 3)
    	achro4210_keypad.sampling_rate = 2;
    else if	(val > 1)
    	achro4210_keypad.sampling_rate = 1;
    else
    	achro4210_keypad.sampling_rate = 0;

    local_irq_restore(flags);

    return count;
}

static 	ssize_t show_gps_wakeup		(struct device *dev, struct device_attribute *attr, char *buf)
{
	return	sprintf(buf, "%d\n", combo_module.status_gps_wakeup);
}

static 	ssize_t set_gps_wakeup		(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    unsigned int	val;

    if(!(sscanf(buf, "%u\n", &val))) 	return	-EINVAL;

	printk(KERN_ERR "%s(%x)\n", __FUNCTION__, val);	

	if(val != 0)
		combo_module.status_gps_wakeup = 1;
	else
		combo_module.status_gps_wakeup = 0;

    return count;
}

static 	ssize_t show_gps_onoff		(struct device *dev, struct device_attribute *attr, char *buf)
{
	return	sprintf(buf, "%d\n", combo_module.status_gps);
}


static 	ssize_t set_gps_onoff		(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    unsigned int	val;

    if(!(sscanf(buf, "%u\n", &val))) 	return	-EINVAL;

	printk(KERN_ERR "%s(%x)\n", __FUNCTION__, val);	

	if(val != 0)
		combo_module.status_gps = 1;
	else
		combo_module.status_gps = 0;

	if(combo_module.status_gps)	{
		gpio_set_value(S5PV310_GPE3(0), 1);
		mdelay(1);
		printk("%s : gps reset high!!\n", __FUNCTION__);
	} else {
		gpio_set_value(S5PV310_GPE3(0), 0);
		printk("%s : gps reset low!!\n", __FUNCTION__);
	}

    return count;
}

int		achro4210_keypad_sysfs_create		(struct platform_device *pdev)	
{
	achro4210_keypad.sampling_rate		= 0;	// 10 msec sampling
	achro4210_keypad.poweroff_time		= 1;	// 3 sec

	combo_module.status_gps 		= 0;
	combo_module.status_gps_wakeup	= 0;
	return	sysfs_create_group(&pdev->dev.kobj, &achro4210_keypad_attr_group);
}

void	achro4210_keypad_sysfs_remove		(struct platform_device *pdev)	
{
    sysfs_remove_group(&pdev->dev.kobj, &achro4210_keypad_attr_group);
}
