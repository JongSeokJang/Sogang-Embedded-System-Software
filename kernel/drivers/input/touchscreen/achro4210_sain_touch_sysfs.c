//[*]--------------------------------------------------------------------------------------------------[*]
//
//
// 
//  Huins Achro4210 Board : Touch Sensor Interface driver
// 
//
//[*]--------------------------------------------------------------------------------------------------[*]
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/input.h>
#include <linux/delay.h>
#include <linux/fs.h>

#include <mach/irqs.h>
#include <asm/system.h>

#include <asm/gpio.h>
#include <plat/gpio-cfg.h>
#include <mach/regs-gpio.h>
#include <mach/gpio.h>

//[*]----------------------------------------------------------------------------------------------[*]
#include "ep0700_touch_gpio_i2c.h"
#include "achro4210_touch.h"

//[*]--------------------------------------------------------------------------------------------------[*]
//
//   function prototype define
//
//[*]--------------------------------------------------------------------------------------------------[*]
int	achro4210_touch_sysfs_create		(struct platform_device *pdev);
void	achro4210_touch_sysfs_remove		(struct platform_device *pdev);	
		
//[*]--------------------------------------------------------------------------------------------------[*]
//
//   sysfs function prototype define
//
//[*]--------------------------------------------------------------------------------------------------[*]
//   touch sampling rate control (5, 10, 20 : unit msec)
//[*]--------------------------------------------------------------------------------------------------[*]
static	ssize_t show_sampling_rate		(struct device *dev, struct device_attribute *attr, char *buf);
static	ssize_t set_sampling_rate		(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);
static	DEVICE_ATTR(sampling_rate, S_IRWXUGO, show_sampling_rate, set_sampling_rate);

#if 0
//[*]--------------------------------------------------------------------------------------------------[*]
//   touch calibration
//[*]--------------------------------------------------------------------------------------------------[*]
static	ssize_t set_ts_cal		(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);
static	DEVICE_ATTR(ts_cal, S_IWUGO, NULL, set_ts_cal);

static	ssize_t set_ts_sense	(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);
static 	ssize_t show_ts_sense		(struct device *dev, struct device_attribute *attr, char *buf);
static	DEVICE_ATTR(ts_sense, S_IRWXUGO, show_ts_sense, set_ts_sense);

#endif

static	ssize_t set_ts_enable	(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);
static 	ssize_t show_ts_enable	(struct device *dev, struct device_attribute *attr, char *buf);

static	DEVICE_ATTR(ts_enable, S_IRWXUGO, show_ts_enable, set_ts_enable);

static	ssize_t set_ts_key_enable	(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);
static 	ssize_t show_ts_key_enable	(struct device *dev, struct device_attribute *attr, char *buf);

static	DEVICE_ATTR(ts_key_enable, S_IRWXUGO, show_ts_key_enable, set_ts_key_enable);

#if 0

static	ssize_t set_ts_upgrade		(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);
static 	ssize_t show_ts_upgrade		(struct device *dev, struct device_attribute *attr, char *buf);

static	DEVICE_ATTR(ts_upgrade, S_IRWXUGO, show_ts_upgrade, set_ts_upgrade);

static	ssize_t set_ts_upgrade_data	(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);
static 	ssize_t show_ts_upgrade_data(struct device *dev, struct device_attribute *attr, char *buf);

static	DEVICE_ATTR(ts_upgrade_data, S_IRWXUGO, show_ts_upgrade_data, set_ts_upgrade_data);

static	ssize_t set_ts_firmware_info(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);
static 	ssize_t show_ts_firmware_info(struct device *dev, struct device_attribute *attr, char *buf);

static	DEVICE_ATTR(ts_firmware_info, S_IRWXUGO, show_ts_firmware_info, set_ts_firmware_info);
#endif

achro4210_touch_t	achro4210_touch;


unsigned char	upgrade_status = 0;
struct  timer_list		upgrade_timer;

//[*]--------------------------------------------------------------------------------------------------[*]
//[*]--------------------------------------------------------------------------------------------------[*]
static struct attribute *achro4210_touch_sysfs_entries[] = {
	&dev_attr_sampling_rate.attr,

//	&dev_attr_ts_cal.attr,
//	&dev_attr_ts_sense.attr,
	&dev_attr_ts_enable.attr,
	&dev_attr_ts_key_enable.attr,
//	&dev_attr_ts_upgrade.attr,
//	&dev_attr_ts_upgrade_data.attr,
//	&dev_attr_ts_firmware_info.attr,
	NULL
};

static struct attribute_group achro4210_touch_attr_group = {
	.name   = NULL,
	.attrs  = achro4210_touch_sysfs_entries,
};

//[*]--------------------------------------------------------------------------------------------------[*]
//[*]--------------------------------------------------------------------------------------------------[*]
static 	ssize_t show_sampling_rate		(struct device *dev, struct device_attribute *attr, char *buf)
{
	switch(achro4210_touch.sampling_rate)	{
		default	:
			achro4210_touch.sampling_rate = 0;
		case	0:
			return	sprintf(buf, "10 msec\n");
		case	1:
			return	sprintf(buf, "20 msec\n");
		case	2:
			return	sprintf(buf, "50 msec\n");
	}
}

//[*]--------------------------------------------------------------------------------------------------[*]
static 	ssize_t set_sampling_rate		(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    unsigned long 	flags;
    unsigned int	val;

    if(!(sscanf(buf, "%u\n", &val)))	return	-EINVAL;

    local_irq_save(flags);

    if		(val > 20)		achro4210_touch.sampling_rate = 2;
    else if	(val > 10)		achro4210_touch.sampling_rate = 1;
    else					achro4210_touch.sampling_rate = 0;

    local_irq_restore(flags);

    return count;
}

#if 0
//[*]--------------------------------------------------------------------------------------------------[*]
static	ssize_t set_ts_cal		(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	unsigned char 	wdata;
    unsigned long 	flags;
    unsigned int	val;

    if(!(sscanf(buf, "%u\n", &val)))	return	-EINVAL;

    local_irq_save(flags);		local_irq_disable();

	// touch recalibration
	wdata =0x00;
	achro4210_touch_write(TS_RECALIBRATION, &wdata, 0);	// set mode
	
	mdelay(500);

    local_irq_restore(flags);

    return count;
}

//[*]--------------------------------------------------------------------------------------------------[*]
//[*]--------------------------------------------------------------------------------------------------[*]
static char	test = 0;

static	ssize_t set_ts_sense		(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
#if 0
	unsigned char 	wdata;
	unsigned long 	flags;
	unsigned int	val;

    if(!(sscanf(buf, "%u\n", &val)))	return	-EINVAL;

    local_irq_save(flags);		local_irq_disable();

	// touch sensitivity
	wdata = (unsigned char)(val&0xff);
	achro4210_touch_write(TS_SENSITIVITY_CTL, &wdata, 1);
	
    local_irq_restore(flags);
#else
	unsigned long 	flags;
	unsigned int	val;

    if(!(sscanf(buf, "%u\n", &val)))	return	-EINVAL;

    local_irq_save(flags);		local_irq_disable();

	if(val != 0)	{	achro4210_touch_write(0x0A, NULL, 0);	test = 1;	}
	else			{	achro4210_touch_write(0x0B, NULL, 0);	test = 0;	}

    local_irq_restore(flags);
#endif

	return count;
}

//[*]--------------------------------------------------------------------------------------------------[*]
//[*]--------------------------------------------------------------------------------------------------[*]
static 	ssize_t show_ts_sense		(struct device *dev, struct device_attribute *attr, char *buf)
{
#if 0	
	unsigned char 	rdata[4];
	unsigned long 	flags;

    local_irq_save(flags);		local_irq_disable();

	achro4210_touch_write(TS_SENSITIVITY_CTL, NULL, 0);	// set mode
	achro4210_touch_read( rdata, 2);

    local_irq_restore(flags);
    return	sprintf(buf, "touch sensitivity : 0x%02x\n", rdata[1]);
#else
    return	sprintf(buf, "touch adaptor status = %d\n", test);
#endif    
    
}
#endif

//[*]--------------------------------------------------------------------------------------------------[*]
//[*]--------------------------------------------------------------------------------------------------[*]
static	ssize_t set_ts_enable		(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	unsigned long 	flags;

    local_irq_save(flags);		local_irq_disable();

	if(!strcmp(buf, "on\n"))		achro4210_touch.enable = 1;
	else							achro4210_touch.enable = 0;

    local_irq_restore(flags);

	return count;
}

//[*]--------------------------------------------------------------------------------------------------[*]
//[*]--------------------------------------------------------------------------------------------------[*]
static 	ssize_t show_ts_enable		(struct device *dev, struct device_attribute *attr, char *buf)
{
	if(achro4210_touch.enable)		return	sprintf(buf, "on\n");
	else						return	sprintf(buf, "off\n");
}

//[*]--------------------------------------------------------------------------------------------------[*]
//[*]--------------------------------------------------------------------------------------------------[*]
static	ssize_t set_ts_key_enable		(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	unsigned long 	flags;

    local_irq_save(flags);		local_irq_disable();

	if(!strcmp(buf, "on\n"))		achro4210_touch.key_enable = 1;
	else							achro4210_touch.key_enable = 0;

    local_irq_restore(flags);

	return count;
}

//[*]--------------------------------------------------------------------------------------------------[*]
//[*]--------------------------------------------------------------------------------------------------[*]
static 	ssize_t show_ts_key_enable		(struct device *dev, struct device_attribute *attr, char *buf)
{
	if(achro4210_touch.key_enable)		return	sprintf(buf, "on\n");
	else							return	sprintf(buf, "off\n");
}

#if 0
//[*]--------------------------------------------------------------------------------------------------[*]
//[*]--------------------------------------------------------------------------------------------------[*]
void	achro4210_touch_fw_upgrade_check	(void)
{
	achro4210_touch.fw_size = DEFAULT_FW_SIZE;

	memcpy(achro4210_touch.fw, &DEFAULT_FW_CODE[0], achro4210_touch.fw_size);
	
	achro4210_touch_fw_upgrade();
}

//[*]--------------------------------------------------------------------------------------------------[*]
static void	upgrade_timer_function(unsigned long arg)
{
	unsigned long	flags;
	
	local_irq_save(flags);	local_irq_disable();

	if(achro4210_touch_fw_upgrade() != 0)	upgrade_status = 0;
	else								upgrade_status = 2;
		
	// change normal mode
	achro4210_touch_mode_change(TOUCH_MODE_NORMAL);

	local_irq_restore(flags);
}

//[*]--------------------------------------------------------------------------------------------------[*]
static	ssize_t set_ts_upgrade			(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	unsigned long 	flags;
    unsigned int	val;

    local_irq_save(flags);		local_irq_disable();

    if(!(sscanf(buf, "%u\n", &val)))	return	-EINVAL;

	switch(val)	{
		default	:	case	0:
			upgrade_status = 0;		achro4210_touch.fw_size = 0;	
			memset(achro4210_touch.fw, 0xFF, MAX_FW_SIZE);
			break;
			
		case	1:	case	2:
			if(upgrade_status != 1)	{
				if(val != 1)	{	// default f/w write
					achro4210_touch.fw_size = DEFAULT_FW_SIZE;
					memcpy(achro4210_touch.fw, &DEFAULT_FW_CODE[0], achro4210_touch.fw_size);
				}

				if(achro4210_touch.fw_size > MIN_FW_SIZE)	{
					// run_upgrade_timer
					del_timer_sync(&upgrade_timer);		init_timer(&upgrade_timer);
					
					upgrade_timer.data 		= (unsigned long)&upgrade_timer;
					upgrade_timer.function 	= upgrade_timer_function;
					upgrade_timer.expires 	= jiffies + HZ/10;
					
					add_timer(&upgrade_timer);
		
					upgrade_status = 1;	// start upgrading
				}
				else	{
					printk("\nNo Firmware Data (fw_size = 0)\n");
				}
			}
			break;
	}

    local_irq_restore(flags);

	return count;
}

//[*]--------------------------------------------------------------------------------------------------[*]
//[*]--------------------------------------------------------------------------------------------------[*]
static 	ssize_t show_ts_upgrade			(struct device *dev, struct device_attribute *attr, char *buf)
{
	switch(upgrade_status)	{
		default	:		upgrade_status = 0;
		case	0:		return	sprintf(buf, "No Firmware Upgrade.\n");
		case	1:		return	sprintf(buf, "Firmware Upgrading...\n");
		case	2:		return	sprintf(buf, "Firmware Upgrade Complete.\n");
	}
}

//[*]--------------------------------------------------------------------------------------------------[*]
//[*]--------------------------------------------------------------------------------------------------[*]
static	ssize_t set_ts_upgrade_data		(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	unsigned long 	flags;

    local_irq_save(flags);		local_irq_disable();

	if(achro4210_touch.fw != NULL)	{
		memcpy(&achro4210_touch.fw[achro4210_touch.fw_size], buf, count);
		achro4210_touch.fw_size += count;
	}
	
    local_irq_restore(flags);

	return count;
}

//[*]--------------------------------------------------------------------------------------------------[*]
//[*]--------------------------------------------------------------------------------------------------[*]
static 	ssize_t show_ts_upgrade_data	(struct device *dev, struct device_attribute *attr, char *buf)
{
	return	sprintf(buf, "Download Data Size = 0x%04X\n", achro4210_touch.fw_size);
}

//[*]--------------------------------------------------------------------------------------------------[*]
//[*]--------------------------------------------------------------------------------------------------[*]
static	ssize_t set_ts_firmware_info	(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	printk("Do not support this function\n");
	
	return count;
}

//[*]--------------------------------------------------------------------------------------------------[*]
//[*]--------------------------------------------------------------------------------------------------[*]
static 	ssize_t show_ts_firmware_info	(struct device *dev, struct device_attribute *attr, char *buf)
{
	return	sprintf(buf, "Firmware Info [ Ver : 0x%02X, Rev : 0x%02X ]\n", achro4210_touch.fw_ver, achro4210_touch.fw_rev);
}
#endif

//[*]--------------------------------------------------------------------------------------------------[*]
//[*]--------------------------------------------------------------------------------------------------[*]
int		achro4210_touch_sysfs_create		(struct platform_device *pdev)	
{
	// variable init
	achro4210_touch.sampling_rate	= 1;	// 20 msec sampling
	#if 0	// ORG
		achro4210_touch.enable			= 0;	// Default disable
		achro4210_touch.key_enable		= 0;	// Default disable
	#else	// TEST
		achro4210_touch.enable			= 1;	// Default disable
		achro4210_touch.key_enable		= 1;	// Default disable
	#endif

#if 0
	achro4210_touch.fw 			= NULL;;
	achro4210_touch.fw_size 		= 0;
#endif	

	return	sysfs_create_group(&pdev->dev.kobj, &achro4210_touch_attr_group);
}

//[*]--------------------------------------------------------------------------------------------------[*]
void	achro4210_touch_sysfs_remove		(struct platform_device *pdev)	
{
    sysfs_remove_group(&pdev->dev.kobj, &achro4210_touch_attr_group);
}
//[*]--------------------------------------------------------------------------------------------------[*]
//[*]--------------------------------------------------------------------------------------------------[*]

