/*
 * ISH1000 linear virbrator driver
 *  Copyright (C) 2010 hardkernel Co.,ltd.
 *  Hakjoo Kim <ruppi.kim@hardkernel.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA  02110-1301, USA.
 */
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/hrtimer.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/earlysuspend.h>
#include <linux/slab.h>

/* TODO: replace with correct header */
#include "../staging/android/timed_output.h"

#define ISH1000_VERSION  "1.2.0"
#define ISH1000_NAME "ish1000"

#define SKIP_I2C_CMD  	11

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define DEFAULT_LEVEL 		140
#define DEFAULT_CLOCK		40
#define DEFAULT_FREQ_HIGH	1
#define DEFAULT_FREQ_LOW	100

#define ACHRO4210_VIBRATOR_NAME "achro4210-vibrator"

static struct 	i2c_client *this_client = NULL;
static struct 	ish1000_data *ish1000_this = NULL;

static unsigned char	vibe_level;
static unsigned char	sys_clock;
static unsigned char	freq_high;
static unsigned char	freq_low;

static unsigned long 	start=0, end=0;
/*
 * driver private data
 */
struct ish1000_data {
	struct timed_output_dev dev;
	struct i2c_client *client;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define AH_BASE_REG_ADDR        0x90
enum
{
  AH_REF_LEVEL_ADDR = AH_BASE_REG_ADDR,
  AH_EXE_CTRL_ADDR,
  AH_VIBE_PW_ADDR,
  AH_PWM_FREQ_HIGH_ADDR,
  AH_PWM_FREQ_LOW_ADDR,
  AH_SYS_CLOCK_ADDR,
  AH_AUTO_SLEEP_WT_ADDR,
  AH_FIX_PAT_ADDR,
  AH_NORMAL_VIBE_LEVEL_ADDR,
  AH_NORMAL_VIBE_TIME_ADDR,
  AH_GPIO_SET0_ADDR,
  AH_GPIO_SET1_ADDR,
  AH_TEST_VIBE_ADDR,
  AH_HW_RESET_ADDR,
  AH_DUMMY_REG_ADDR,
  AH_CHIP_ID_ADDR,
  AH_USRPAT_AMP1_ADDR,
  AH_USRPAT_TIME1_ADDR,
  AH_USRPAT_AMP2_ADDR,
  AH_USRPAT_TIME2_ADDR,
  AH_USRPAT_AMP3_ADDR,
  AH_USRPAT_TIME3_ADDR,
  AH_USRPAT_AMP4_ADDR,
  AH_USRPAT_TIME4_ADDR,
  AH_USRPAT_CTRL_ADDR,

  AH_MAX_REG_ADDR = 0xAF
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static int ish1000_i2c_write (unsigned char reg, unsigned char wdata)
{
	u8 data[2];
	data[0] = reg & 0xFF;
	data[1] = wdata & 0xFF;
	
	return 	i2c_master_send (this_client, data, 2);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static int ish1000_i2c_read (unsigned char reg_addr)
{
	return 	i2c_smbus_read_byte_data (this_client, reg_addr);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static int ish1000_hw_init(void)
{
#if 0
	printk("%s : sys_clock = %d\n",__FUNCTION__,sys_clock);
	printk("%s : freq_high = %d\n",__FUNCTION__,freq_high);
	printk("%s : freq_low = %d\n",__FUNCTION__,freq_low);
	printk("%s : vibe_level = %d\n",__FUNCTION__,vibe_level);
#endif

	i2c_smbus_write_byte_data (this_client, AH_HW_RESET_ADDR, 1);
	mdelay (1);
	i2c_smbus_write_byte_data (this_client, AH_HW_RESET_ADDR, 0);
	mdelay (1);
	ish1000_i2c_write(AH_SYS_CLOCK_ADDR, sys_clock);
	mdelay (100);
	ish1000_i2c_write(AH_PWM_FREQ_HIGH_ADDR, freq_high);
	mdelay (1);
	ish1000_i2c_write(AH_PWM_FREQ_LOW_ADDR, freq_low);
	mdelay (100);
	ish1000_i2c_write(AH_NORMAL_VIBE_LEVEL_ADDR, vibe_level);

	mdelay (1);
	ish1000_i2c_write (AH_EXE_CTRL_ADDR, 0xc0);

	return 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static	ssize_t show_vibe_level	(struct device *dev, struct device_attribute *attr, char *buf);
static	ssize_t set_vibe_level	(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);
static	DEVICE_ATTR(vibe_level, S_IRWXUGO, show_vibe_level, set_vibe_level);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static	ssize_t show_sys_clock	(struct device *dev, struct device_attribute *attr, char *buf);
static	ssize_t set_sys_clock	(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);
static	DEVICE_ATTR(sys_clock, S_IRWXUGO, show_sys_clock, set_sys_clock);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static	ssize_t show_freq_high	(struct device *dev, struct device_attribute *attr, char *buf);
static	ssize_t set_freq_high	(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);
static	DEVICE_ATTR(freq_high, S_IRWXUGO, show_freq_high, set_freq_high);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static	ssize_t show_freq_low	(struct device *dev, struct device_attribute *attr, char *buf);
static	ssize_t set_freq_low	(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);
static	DEVICE_ATTR(freq_low, S_IRWXUGO, show_freq_low, set_freq_low);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static 	ssize_t show_vibe_level		(struct device *dev, struct device_attribute *attr, char *buf)
{
	return	sprintf(buf, "%d\n",vibe_level);
}

static 	ssize_t set_vibe_level		(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    unsigned int	val;

    if(!(sscanf(buf, "%u\n", &val)))	return	-EINVAL;

    if(val > 255) 	vibe_level = 254;
    else			vibe_level = val;

	ish1000_hw_init();

    return count;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static 	ssize_t show_sys_clock	(struct device *dev, struct device_attribute *attr, char *buf)
{
	return	sprintf(buf, "%d\n",sys_clock);
}

static 	ssize_t set_sys_clock	(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    unsigned int	val;

    if(!(sscanf(buf, "%u\n", &val)))	return	-EINVAL;
    if(val > 255) 	sys_clock = 254;
    else			sys_clock = val;
	ish1000_hw_init();

    return count;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static 	ssize_t show_freq_high	(struct device *dev, struct device_attribute *attr, char *buf)
{
	return	sprintf(buf, "%d\n",freq_high);
}

static 	ssize_t set_freq_high	(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    unsigned int	val;

    if(!(sscanf(buf, "%u\n", &val)))	return	-EINVAL;
    if(val > 255) 	freq_high = 254;
    else			freq_high = val;
	ish1000_hw_init();

    return count;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static 	ssize_t show_freq_low	(struct device *dev, struct device_attribute *attr, char *buf)
{
	return	sprintf(buf, "%d\n",freq_low);
}

static 	ssize_t set_freq_low	(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    unsigned int	val;

    if(!(sscanf(buf, "%u\n", &val)))	return	-EINVAL;
    if(val > 255) 	freq_low = 254;
    else			freq_low = val;
	ish1000_hw_init();

    return count;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static struct attribute *achro4210_vibrator_sysfs_entries[] = {
	&dev_attr_vibe_level.attr,
	&dev_attr_sys_clock.attr,
	&dev_attr_freq_high.attr,
	&dev_attr_freq_low.attr,
	NULL
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static struct attribute_group achro4210_vibrator_attr_group = {
	.name   = NULL,
	.attrs  = achro4210_vibrator_sysfs_entries,
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static void ish1000_vibrator_onoff (struct ish1000_data *ish1000, int on, int time_5msec)
{
	if(on){
		if(ish1000_i2c_read(AH_EXE_CTRL_ADDR)!=0xc0) {
			printk("%s : AH_EXE_CTRL_ADDR disable to enable...\n",__FUNCTION__);
			ish1000_i2c_write(AH_EXE_CTRL_ADDR, 0xc0);
			udelay(50);
		}
		ish1000_i2c_write (AH_NORMAL_VIBE_LEVEL_ADDR, vibe_level);
		udelay(50);
		ish1000_i2c_write (AH_NORMAL_VIBE_TIME_ADDR, (unsigned char)time_5msec);
		udelay(50);
	}
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void direct_vibration(int time_5msec)
{
	unsigned long	flags;
	
	ish1000_i2c_write(AH_EXE_CTRL_ADDR, 0xc0);
	udelay(50);
	ish1000_i2c_write (AH_NORMAL_VIBE_LEVEL_ADDR, vibe_level);
	udelay(50);
	ish1000_i2c_write (AH_NORMAL_VIBE_TIME_ADDR, (unsigned char)time_5msec);
	udelay(50);

}
EXPORT_SYMBOL(direct_vibration);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static void ish1000_vibrator_enable (struct timed_output_dev *dev, int value)
{
	struct ish1000_data *ish1000 = container_of (dev, struct ish1000_data, dev);
	int time_5msec=0;

	if (4 < value){
		end = jiffies;
		if(SKIP_I2C_CMD < abs(start-end)) {
			time_5msec = value/5;
			ish1000_vibrator_onoff (ish1000, 1, time_5msec);
		}
		start = jiffies;
	}
	return;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static int ish1000_vibrator_get_time (struct timed_output_dev *dev)
{
	return 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static struct timed_output_dev ish1000_vibrator = {
  .name = "vibrator",
  .get_time = ish1000_vibrator_get_time,
  .enable = ish1000_vibrator_enable,
};

#define ISH1000_CHIP_VER 0x0d

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static int ish1000_detect (struct i2c_client *client, struct i2c_board_info *info)
{
	int id=0;
	
	id = ish1000_i2c_read(AH_CHIP_ID_ADDR);
	printk("ish1000 verion is 0x%x\r\n", id);
	
	if((id==ISH1000_CHIP_VER)||(id==0x08) || (id == 0xf) )
		return 0;
	
	return -ENODEV;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static int ish1000_i2c_probe (struct i2c_client *client, const struct i2c_device_id *id)
{
	int err;
	struct ish1000_data *ish1000;
	
	ish1000 = kzalloc (sizeof (struct ish1000_data), GFP_KERNEL);

	/* setup i2c client */
	if (!i2c_check_functionality (client->adapter, I2C_FUNC_I2C)) {
		err = -ENODEV;
		goto error_1;
	}
	i2c_set_clientdata (client, ish1000);
	ish1000->client = client;
	this_client = client;
	ish1000_this = ish1000;

	/* detect and init hardware */
	if ((err = ish1000_detect (client, NULL)))
		goto error_2;

	ish1000->dev = ish1000_vibrator;
	err = timed_output_dev_register (&ish1000->dev);
	if (err < 0)
		goto error_3;

	// variable init
	vibe_level	= DEFAULT_LEVEL;
	sys_clock	= DEFAULT_CLOCK;
	freq_high	= DEFAULT_FREQ_HIGH;
	freq_low	= DEFAULT_FREQ_LOW;

	ish1000_hw_init();
	
	return 0;
	
error_3:
	timed_output_dev_unregister (&ish1000->dev);
error_2:
	i2c_set_clientdata (client, NULL);
error_1:
	kfree (ish1000);
	
	return err;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static int ish1000_i2c_remove (struct i2c_client *client)
{
	struct ish1000_data *ish1000 = i2c_get_clientdata (client);
	
	timed_output_dev_unregister (&ish1000->dev);
	i2c_set_clientdata (client, NULL);
	kfree (ish1000);
	
	return 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifdef CONFIG_PM
#ifdef CONFIG_HAS_EARLYSUSPEND
static void ish1000_early_suspend(struct platform_device *dev, pm_message_t state)
{
	printk("\t%s [%d]\n",__FUNCTION__,__LINE__);
	return 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static void ish1000_late_resume(struct platform_device *dev)
{
	mdelay(200);	// Power on delay vdd_lcd
	printk("%s\n",__FUNCTION__);
	ish1000_hw_init();
	return 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static struct early_suspend ish1000_early_suspend_desc = {
     .level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN,
     .suspend = ish1000_early_suspend,
     .resume = ish1000_late_resume
};

#else
	static int ish1000_suspend (struct i2c_client *client, pm_message_t mesg)
	{
	  printk ("%s\r\n", __FUNCTION__);
	  return 0;
	}
	
	static int ish1000_resume (struct i2c_client *client)
	{
	  printk ("%s\r\n", __FUNCTION__);
	  return 0;
	}
#endif
#else
#define ish1000_suspend NULL
#define ish1000_resume  NULL
#endif


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static const struct i2c_device_id ish1000_id[] = {
  {ISH1000_NAME, 0},
  {},
};
MODULE_DEVICE_TABLE (i2c, ish1000_id);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
struct i2c_driver ish1000_driver = {
	.driver = {
	     .name = "ish1000",
	     .owner = THIS_MODULE,
     },
	.probe =    ish1000_i2c_probe,
	.remove =   ish1000_i2c_remove,
	.id_table = ish1000_id,
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static int __devinit    ish1000_probe	(struct platform_device *pdev)
{
	int ret;

	ret =sysfs_create_group(&pdev->dev.kobj, &achro4210_vibrator_attr_group);
	if(ret)	{
		printk("%s : sysfs_create_group fail!!\n", __FUNCTION__);
		return	ret;
	}
	return 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static int __devexit    ish1000_remove	(struct platform_device *pdev)
{
	sysfs_remove_group(&pdev->dev.kobj, &achro4210_vibrator_attr_group);
	return 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static void	vibrator_release_device	(struct device *dev)
{
	#if	defined(DEBUG_ACHRO4210_TOUCH_MSG)
		printk("%s\n", __FUNCTION__);
	#endif
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static struct platform_driver vibrator_platform_driver = {
	.probe          = ish1000_probe,
	.remove         = ish1000_remove,

#ifndef CONFIG_HAS_EARLYSUSPEND
	.suspend        = ish1000_suspend,
	.resume         = ish1000_resume,
#endif
	.driver		= {
		.owner	= THIS_MODULE,
		.name	= ACHRO4210_VIBRATOR_NAME,
	},
};
//[*]--------------------------------------------------------------------------------------------------[*]
static struct platform_device vibrator_platform_device = {
        .name           = ACHRO4210_VIBRATOR_NAME,
        .id             = -1,
        .num_resources  = 0,
        .dev    = {
                .release	= vibrator_release_device,
        },
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static int __init ish1000_init (void)
{
	int ret;

	ret = i2c_add_driver (&ish1000_driver);
	if (ret) {
		printk(KERN_ERR "i2c add driver Failed %d\n", ret);
		return -1;
	}

	ret = platform_driver_register(&vibrator_platform_driver);
	if (ret) {
		printk(KERN_ERR "Vibrator Platform Driver Register Failed %d\n", ret);
		return -1;
	}
	else {
		ret = platform_device_register(&vibrator_platform_device);
		if (ret) {
			printk(KERN_ERR "Vibrator Platform Device Register Failed %d\n", ret);
			return -1;
		}
	}
	
#ifdef CONFIG_HAS_EARLYSUSPEND
	register_early_suspend(&ish1000_early_suspend_desc);
#endif
	return 0;
}
module_init (ish1000_init);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static void __exit ish1000_exit (void)
{
	platform_driver_unregister(&vibrator_platform_driver);
#ifdef CONFIG_HAS_EARLYSUSPEND
	unregister_early_suspend(&ish1000_early_suspend_desc);
#endif
	i2c_del_driver (&ish1000_driver);
}
module_exit (ish1000_exit);


MODULE_LICENSE ("GPL");
MODULE_DESCRIPTION ("ISH1000 linear virbrator driver");
MODULE_VERSION (ISH1000_VERSION);
