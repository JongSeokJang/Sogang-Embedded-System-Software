//[*]--------------------------------------------------------------------------------------------------[*]
//
//
// 
//  achro4210 Board : achro4210 sysfs driver (charles.park)
//  2010.04.20
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
// Sleep disable flage
//[*]--------------------------------------------------------------------------------------------------[*]
#define	SLEEP_DISABLE_FLAG

#if defined(SLEEP_DISABLE_FLAG)
	#ifdef CONFIG_HAS_WAKELOCK
		#include <linux/wakelock.h>
		static struct wake_lock 	sleep_wake_lock;
	#endif
#endif

//[*]--------------------------------------------------------------------------------------------------[*]
//   Control GPIO Define
//[*]--------------------------------------------------------------------------------------------------[*]
// GPIO Index Define
enum	{
	// WIFI Control Port
	WIFI_RESET,
	WIFI_REG,
	WIFI_WAKEUP,
	WIFI_WAKEUP_HOST,
	
	// Bluetooth Control Port
	BLUETOOTH_RESET,
	BLUETOOTH_REG,
	BLUETOOTH_WAKEUP,
	BLUETOOTH_WAKEUP_HOST,
	
#if defined(CONFIG_FB_S3C_HDMI_UI_FB)
	// Power Control
	SYSTEM_POWER_2V8,		// BUCK6 Enable Control
	SYSTEM_POWER_3V3,		// 3.3V Enable
	SYSTEM_POWER_5V0,		// 5.-V Enable
	SYSTEM_OUTPUT_485,		// 5.-V Enable
#else
	// 3G Modem Control Port
//	MODEM_POWER, //jhkang: Huins used GPIO port for TouchScreen INT port.
	MODEM_RESET,
	MODEM_DISABLE1,
	MODEM_DISABLE2,
	
	// Status LED Display
	STATUS_LED_RED,
	STATUS_LED_GREEN,
	STATUS_LED_BLUE,
	
	// Power Control
//	SYSTEM_POWER_3V3,		// BUCK6 Enable Control
	SYSTEM_POWER_5V0,		// USB HOST Power
	SYSTEM_POWER_12V0,		// VLED Control (Backlight)
#endif	
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
//#if defined(CONFIG_ANDROID_PARANOID_NETWORK)
	// High -> Reset Active
	{	WIFI_RESET,				S5PV310_GPK1(0), "wifi_reset",			1, 0, S3C_GPIO_PULL_UP	},
	// High -> REG Active
	{	WIFI_REG,				S5PV310_GPK1(1), "wifi_reg", 			1, 0, S3C_GPIO_PULL_UP	},
//#else //- ubuntu
 	// High -> Reset Active
// 	{	WIFI_RESET,				S5PV310_GPK1(0), "wifi_reset",			1, 1, S3C_GPIO_PULL_UP	},
 	// High -> REG Active
// 	{	WIFI_REG,				S5PV310_GPK1(1), "wifi_reg", 			1, 1, S3C_GPIO_PULL_UP	},
//#endif
	// High -> Wakeup Active
	{	WIFI_WAKEUP,			S5PV310_GPK1(4), "wifi_wakeup", 		1, 0, S3C_GPIO_PULL_UP	},
	// Low -> High : Wifi wake up request to host
	{	WIFI_WAKEUP_HOST,		S5PV310_GPX0(7), "wifi_wakeup_host", 	0, 0, S3C_GPIO_PULL_UP	},

	// High -> Reset Active
	{	BLUETOOTH_RESET,		S5PV310_GPK1(2), "bt_reset", 			1, 0, S3C_GPIO_PULL_UP	},
	// High -> REG Active
	{	BLUETOOTH_REG,			S5PV310_GPK1(3), "bt_reg", 				1, 0, S3C_GPIO_PULL_UP	},
	// High -> Wakeup Active
	{	BLUETOOTH_WAKEUP,		S5PV310_GPK1(5), "bt_wakeup", 			1, 0, S3C_GPIO_PULL_UP	},
	// Low -> High : BT wake up request to host
	{	BLUETOOTH_WAKEUP_HOST,	S5PV310_GPX0(6), "bt_wakeup_host",		0, 0, S3C_GPIO_PULL_UP	},

#if defined(CONFIG_FB_S3C_HDMI_UI_FB)
	// SYSTEM POWER CONTROL
	{	SYSTEM_POWER_2V8,		S5PV310_GPX1(0), "power_2v8",			1, 1, S3C_GPIO_PULL_NONE	},
	{	SYSTEM_POWER_3V3,		S5PV310_GPX1(1), "power_3v3",			1, 1, S3C_GPIO_PULL_NONE	},
	{	SYSTEM_POWER_5V0,		S5PV310_GPX1(2), "power_5v0",			1, 1, S3C_GPIO_PULL_NONE	},
	{	SYSTEM_OUTPUT_485,		S5PV310_GPC1(2), "output_485",			1, 1, S3C_GPIO_PULL_NONE	},
#else
	// High -> Power Enable
//	{	MODEM_POWER,			S5PV310_GPX1(0), "modem_power", 		1, 1, S3C_GPIO_PULL_UP	},//jhkang: Huins used GPIO port for TouchScreen INT port.
	// Low -> Reset Active
	{	MODEM_RESET,			S5PV310_GPX1(1), "modem_reset", 		1, 0, S3C_GPIO_PULL_UP	},
	// High -> Disable1 Active
	{	MODEM_DISABLE1,			S5PV310_GPX1(2), "modem_disable1",		1, 0, S3C_GPIO_PULL_UP	},
	// High -> Disable2 Active
	{	MODEM_DISABLE2,			S5PV310_GPX1(3), "modem_disable2",		1, 0, S3C_GPIO_PULL_UP	},
	
	// STATUS LED : High -> LED ON
	{	STATUS_LED_RED,			S5PV310_GPC1(2), "led_red",				1, 0, S3C_GPIO_PULL_DOWN	},
	{	STATUS_LED_GREEN,		S5PV310_GPC1(3), "led_green",			1, 0, S3C_GPIO_PULL_DOWN	},
	{	STATUS_LED_BLUE,		S5PV310_GPC1(4), "led_blue",			1, 1, S3C_GPIO_PULL_DOWN	},
	
	// SYSTEM POWER CONTROL
//	{	SYSTEM_POWER_3V3,		S5PV310_GPX3(5), "power_3v3",			1, 1, S3C_GPIO_PULL_DOWN	},
	{	SYSTEM_POWER_5V0,		S5PV310_GPC0(0), "power_5v0",		1, 1, S3C_GPIO_PULL_DOWN	},
#if defined(CONFIG_FB_S3C_TRULY1280)
	{	SYSTEM_POWER_12V0,		S5PV310_GPC0(3), "power_12v0",	1, 0, S3C_GPIO_PULL_DOWN	},
#elif defined(CONFIG_FB_S3C_A070VW08)
kan	{	SYSTEM_POWER_12V0,		S5PV310_GPL0(1), "power_12v0",	1, 1, S3C_GPIO_PULL_UP	},//0, S3C_GPIO_PULL_DOWN	},
#elif defined(CONFIG_FB_S3C_LMS700KF06)	
	{	SYSTEM_POWER_12V0,		S5PV310_GPL0(7), "power_12v0",	1, 1, S3C_GPIO_PULL_UP	},
#endif
#endif	
};

//[*]--------------------------------------------------------------------------------------------------[*]
//[*]--------------------------------------------------------------------------------------------------[*]
//   function prototype define
//[*]--------------------------------------------------------------------------------------------------[*]
static 	int		achro4210_sysfs_resume		(struct platform_device *dev);
static 	int		achro4210_sysfs_suspend	(struct platform_device *dev, pm_message_t state);
static	int		achro4210_sysfs_probe		(struct platform_device *pdev);
static	int		achro4210_sysfs_remove		(struct platform_device *pdev);	

static 	int 	__init achro4210_sysfs_init(void);
static 	void 	__exit achro4210_sysfs_exit(void);

//[*]--------------------------------------------------------------------------------------------------[*]
static struct platform_driver achro4210_sysfs_driver = {
	.driver = {
		.name = "achro4210-sysfs",
		.owner = THIS_MODULE,
	},
	.probe 		= achro4210_sysfs_probe,
	.remove 	= achro4210_sysfs_remove,
	.suspend	= achro4210_sysfs_suspend,
	.resume		= achro4210_sysfs_resume,
};

//[*]--------------------------------------------------------------------------------------------------[*]
module_init(achro4210_sysfs_init);
module_exit(achro4210_sysfs_exit);

//[*]--------------------------------------------------------------------------------------------------[*]

MODULE_DESCRIPTION("SYSFS driver for achro4210-Dev board");
MODULE_AUTHOR("Hard-Kernel");
MODULE_LICENSE("GPL");

extern	bool s5p_hpd_get_status(void);
//[*]--------------------------------------------------------------------------------------------------[*]
//
//   sysfs function prototype define
//
//[*]--------------------------------------------------------------------------------------------------[*]
static	ssize_t show_gpio	(struct device *dev, struct device_attribute *attr, char *buf);
static 	ssize_t set_gpio	(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);
static	ssize_t show_hdmi	(struct device *dev, struct device_attribute *attr, char *buf);
//[*]--------------------------------------------------------------------------------------------------[*]
//[*]--------------------------------------------------------------------------------------------------[*]
static	DEVICE_ATTR(wifi_reset, 		S_IRWXUGO, show_gpio, set_gpio);
static	DEVICE_ATTR(wifi_reg, 			S_IRWXUGO, show_gpio, set_gpio);
static	DEVICE_ATTR(wifi_wakeup, 		S_IRWXUGO, show_gpio, set_gpio);
static	DEVICE_ATTR(wifi_wakeup_host, 	S_IRWXUGO, show_gpio, set_gpio);
//[*]--------------------------------------------------------------------------------------------------[*]
static	DEVICE_ATTR(bt_reset, 			S_IRWXUGO, show_gpio, set_gpio);
static	DEVICE_ATTR(bt_reg, 			S_IRWXUGO, show_gpio, set_gpio);
static	DEVICE_ATTR(bt_wakeup, 			S_IRWXUGO, show_gpio, set_gpio);
static	DEVICE_ATTR(bt_wakeup_host,		S_IRWXUGO, show_gpio, set_gpio);
//[*]--------------------------------------------------------------------------------------------------[*]
#if defined(CONFIG_FB_S3C_HDMI_UI_FB)
//[*]--------------------------------------------------------------------------------------------------[*]
	static	DEVICE_ATTR(power_2v8,			S_IRWXUGO, show_gpio, set_gpio);
	static	DEVICE_ATTR(power_3v3,			S_IRWXUGO, show_gpio, set_gpio);
	static	DEVICE_ATTR(power_5v0,			S_IRWXUGO, show_gpio, set_gpio);
	static	DEVICE_ATTR(output_485,			S_IRWXUGO, show_gpio, set_gpio);
//[*]--------------------------------------------------------------------------------------------------[*]
#else
//[*]--------------------------------------------------------------------------------------------------[*]
	static	DEVICE_ATTR(modem_power, 		S_IRWXUGO, show_gpio, set_gpio);
	static	DEVICE_ATTR(modem_reset,		S_IRWXUGO, show_gpio, set_gpio);
	static	DEVICE_ATTR(modem_disable1,		S_IRWXUGO, show_gpio, set_gpio);
	static	DEVICE_ATTR(modem_disable2,		S_IRWXUGO, show_gpio, set_gpio);
	//[*]--------------------------------------------------------------------------------------------------[*]
	static	DEVICE_ATTR(led_red, 			S_IRWXUGO, show_gpio, set_gpio);
	static	DEVICE_ATTR(led_green,			S_IRWXUGO, show_gpio, set_gpio);
	static	DEVICE_ATTR(led_blue,			S_IRWXUGO, show_gpio, set_gpio);
	//[*]--------------------------------------------------------------------------------------------------[*]
	static	DEVICE_ATTR(power_3v3,			S_IRWXUGO, show_gpio, set_gpio);
	static	DEVICE_ATTR(power_5v0,			S_IRWXUGO, show_gpio, set_gpio);
	static	DEVICE_ATTR(power_12v0,			S_IRWXUGO, show_gpio, set_gpio);
//[*]--------------------------------------------------------------------------------------------------[*]
#endif
//[*]--------------------------------------------------------------------------------------------------[*]
static	DEVICE_ATTR(hdmi_state,			S_IRWXUGO, show_hdmi, NULL);
//[*]--------------------------------------------------------------------------------------------------[*]
//[*]--------------------------------------------------------------------------------------------------[*]
static struct attribute *achro4210_sysfs_entries[] = {
	&dev_attr_wifi_reset.attr,
	&dev_attr_wifi_reg.attr,
	&dev_attr_wifi_wakeup.attr,
	&dev_attr_wifi_wakeup_host.attr,

	&dev_attr_bt_reset.attr,
	&dev_attr_bt_reg.attr,
	&dev_attr_bt_wakeup.attr,
	&dev_attr_bt_wakeup_host.attr,

#if defined(CONFIG_FB_S3C_HDMI_UI_FB)
	&dev_attr_power_2v8.attr,
	&dev_attr_power_3v3.attr,
	&dev_attr_power_5v0.attr,
	&dev_attr_output_485.attr,
#else
	&dev_attr_modem_power.attr,
	&dev_attr_modem_reset.attr,
	&dev_attr_modem_disable1.attr,
	&dev_attr_modem_disable2.attr,

	&dev_attr_led_red.attr,
	&dev_attr_led_green.attr,
	&dev_attr_led_blue.attr,

	&dev_attr_power_3v3.attr,
	&dev_attr_power_5v0.attr,
	&dev_attr_power_12v0.attr,
#endif	
	&dev_attr_hdmi_state,
	NULL
};

static struct attribute_group achro4210_sysfs_attr_group = {
	.name   = NULL,
	.attrs  = achro4210_sysfs_entries,
};

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

    if(!(sscanf(buf, "%d\n", &val))) 	return	-EINVAL;

	for (i = 0; i < ARRAY_SIZE(sControlGpios); i++) {
		if(!strcmp(sControlGpios[i].name, attr->attr.name))	{

			printk("=>set_gpio(%s)!!\n", attr->attr.name);
			
			if(sControlGpios[i].output)
			{
				gpio_set_value(sControlGpios[i].gpio, ((val != 0) ? 1 : 0));
				printk("==>val(%d)!!\n", val);
			}
			else
				printk("This GPIO Configuration is INPUT!!\n");
		    return count;
		}
	}

	printk("ERROR! : Not found gpio!\n");
    return 0;
}

//[*]--------------------------------------------------------------------------------------------------[*]
static	ssize_t show_hdmi	(struct device *dev, struct device_attribute *attr, char *buf)
{
#if defined(CONFIG_VIDEO_TVOUT)
	int status = s5p_hpd_get_status();
	
	if(status)	return	sprintf(buf, "%s\n", "on");
	else		return	sprintf(buf, "%s\n", "off");
#else
	return	sprintf(buf, "%s\n", "off");
#endif
}
//[*]--------------------------------------------------------------------------------------------------[*]
//[*]--------------------------------------------------------------------------------------------------[*]
//[*]--------------------------------------------------------------------------------------------------[*]
#if defined(CONFIG_FB_S3C_HDMI_UI_FB)
//[*]--------------------------------------------------------------------------------------------------[*]
void 	SYSTEM_POWER_CONTROL	(int power, int val)
{
	int	index;
	
	switch(power)	{
		case	0:	index = SYSTEM_POWER_2V8;		break;
		case	1:	index = SYSTEM_POWER_3V3;		break;
		case	2:	index = SYSTEM_POWER_5V0;		break;
		default	:									return;
	}
	
	gpio_set_value(sControlGpios[index].gpio, ((val != 0) ? 1 : 0));
}

EXPORT_SYMBOL(SYSTEM_POWER_CONTROL);
//[*]--------------------------------------------------------------------------------------------------[*]
#else
//[*]--------------------------------------------------------------------------------------------------[*]
void	STATUS_LED_CONTROL	(int led, int val)
{
	int	index;
	
	switch(led)	{
		case	0:	index = STATUS_LED_RED;		break;
		case	1:	index = STATUS_LED_GREEN;	break;
		case	2:	index = STATUS_LED_BLUE;	break;
		default	:								return;
	}
	
	gpio_set_value(sControlGpios[index].gpio, ((val != 0) ? 1 : 0));
}

EXPORT_SYMBOL(STATUS_LED_CONTROL);
//[*]--------------------------------------------------------------------------------------------------[*]
void 	SYSTEM_POWER_CONTROL	(int power, int val)
{
	int	index;
	
	switch(power)	{
//		case	0:	index = SYSTEM_POWER_3V3;		break;
		case	1:	index = SYSTEM_POWER_5V0;		break;
		case	2:	index = SYSTEM_POWER_12V0;		break;
		default	:									return;
	}
	
	gpio_set_value(sControlGpios[index].gpio, ((val != 0) ? 1 : 0));
}

EXPORT_SYMBOL(SYSTEM_POWER_CONTROL);
//[*]--------------------------------------------------------------------------------------------------[*]
#endif
//[*]--------------------------------------------------------------------------------------------------[*]
//[*]--------------------------------------------------------------------------------------------------[*]
static int	achro4210_sysfs_resume(struct platform_device *dev)
{
	#if	defined(DEBUG_PM_MSG)
		printk("%s\n", __FUNCTION__);
	#endif

    return  0;
}

//[*]--------------------------------------------------------------------------------------------------[*]
static int	achro4210_sysfs_suspend(struct platform_device *dev, pm_message_t state)
{
	#if	defined(DEBUG_PM_MSG)
		printk("%s\n", __FUNCTION__);
	#endif
	
    return  0;
}

//[*]--------------------------------------------------------------------------------------------------[*]
static	int		achro4210_sysfs_probe		(struct platform_device *pdev)	
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

#if defined(SLEEP_DISABLE_FLAG)
	#ifdef CONFIG_HAS_WAKELOCK
		wake_lock(&sleep_wake_lock);
	#endif
#endif

	return	sysfs_create_group(&pdev->dev.kobj, &achro4210_sysfs_attr_group);
}

//[*]--------------------------------------------------------------------------------------------------[*]
static	int		achro4210_sysfs_remove		(struct platform_device *pdev)	
{
	int	i;
	
	for (i = 0; i < ARRAY_SIZE(sControlGpios); i++) 	gpio_free(sControlGpios[i].gpio);

#if defined(SLEEP_DISABLE_FLAG)
	#ifdef CONFIG_HAS_WAKELOCK
		wake_unlock(&sleep_wake_lock);
	#endif
#endif

    sysfs_remove_group(&pdev->dev.kobj, &achro4210_sysfs_attr_group);
    
    return	0;
}
//[*]--------------------------------------------------------------------------------------------------[*]
static int __init achro4210_sysfs_init(void)
{	
#if defined(SLEEP_DISABLE_FLAG)
	#ifdef CONFIG_HAS_WAKELOCK
		printk("--------------------------------------------------------\n");
		printk("\n%s(%d) : Sleep Disable Flag SET!!(Wake_lock_init)\n\n", __FUNCTION__, __LINE__);
		printk("--------------------------------------------------------\n");

	    wake_lock_init(&sleep_wake_lock, WAKE_LOCK_SUSPEND, "sleep_wake_lock");
	#endif
#endif
    return platform_driver_register(&achro4210_sysfs_driver);
}

//[*]--------------------------------------------------------------------------------------------------[*]
static void __exit achro4210_sysfs_exit(void)
{
#if defined(SLEEP_DISABLE_FLAG)
	#ifdef CONFIG_HAS_WAKELOCK
	    wake_lock_destroy(&sleep_wake_lock);
	#endif
#endif
    platform_driver_unregister(&achro4210_sysfs_driver);
}

//[*]--------------------------------------------------------------------------------------------------[*]
