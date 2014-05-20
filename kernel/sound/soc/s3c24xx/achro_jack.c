/*
 *	JACK device detection driver.
 *
 *	Copyright (C) 2009 Samsung Electronics, Inc.
 *
 *	Authors:
 *		Uk Kim <w0806.kim@samsung.com>
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; version 2 of the License.
 */

#include <linux/module.h>
#include <linux/sysdev.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/irq.h>
#include <linux/delay.h>
#include <linux/types.h>
#include <linux/input.h>
#include <linux/platform_device.h>
#include <linux/mutex.h>
#include <linux/errno.h>
#include <linux/err.h>
#include <linux/switch.h>
#include <linux/input.h>
#include <linux/timer.h>
#include <linux/wakelock.h>
#include <linux/slab.h>

#include <mach/hardware.h>
#include <mach/gpio.h>
#include <mach/regs-gpio.h>
#include <plat/gpio-cfg.h>
#include <plat/irqs.h>
#include <asm/mach-types.h>

#include <mach/achro_jack.h>

struct achro_jack_info {
	struct achro_jack_port port;
	struct input_dev *input;
};

extern int	max98089_routing(u8 playback_path);

static struct achro_jack_info *hi;
static struct delayed_work jack_detect_work;

struct switch_dev switch_jack_detection = {
		.name = "h2w",
};

static unsigned int current_jack_type_status;

bool	g_btvout=false;
extern bool get_hdmi_onoff_status(void);

void	achro_audio_tvout(bool tvout)
{
#if defined(CONFIG_FB_S3C_HDMI_UI_FB) && !defined(CONFIG_BATTERY_MAX17040)
	g_btvout = false;
#else
	if(tvout) {
		g_btvout = true;
		max98089_routing(3);
	}
	else {
		g_btvout = false;

		switch(current_jack_type_status) {
			case JACK_NO_DEVICE :
				max98089_routing(0);
				break;
	
			case HEADSET_4_POLE_DEVICE :
			case HEADSET_3_POLE_DEVICE :
				max98089_routing(1);
				break;
	
			case TVOUT_DEVICE :
				max98089_routing(3);
				break;
			case UNKNOWN_DEVICE :
				break;
		}
	}
#endif
}
EXPORT_SYMBOL(achro_audio_tvout);

unsigned int get_headset_status(void)
{
	return current_jack_type_status;
}
EXPORT_SYMBOL(get_headset_status);

static int jack_detect_change(struct delayed_work *dwork)
{
	struct achro_gpio_info   *det_jack = &hi->port.det_jack;
	int err=0; 

	err = gpio_request( det_jack->gpio, "GPIO_HEADSET");
	if(err)	goto err;

	if(gpio_get_value(det_jack->gpio) && (current_jack_type_status != JACK_NO_DEVICE)) {
		current_jack_type_status = JACK_NO_DEVICE;
		switch_set_state(&switch_jack_detection, current_jack_type_status);
		msleep(500);
	}
	else if(!gpio_get_value(det_jack->gpio) && (current_jack_type_status != HEADSET_3_POLE_DEVICE)) {
		current_jack_type_status = HEADSET_3_POLE_DEVICE;
		switch_set_state(&switch_jack_detection, current_jack_type_status);
		msleep(500);
	}
	gpio_free(det_jack->gpio);

	switch(current_jack_type_status) {
		case JACK_NO_DEVICE :
			max98089_routing(0);
			break;

		case HEADSET_4_POLE_DEVICE :
		case HEADSET_3_POLE_DEVICE :
			max98089_routing(1);
			break;

		case TVOUT_DEVICE :
			max98089_routing(3);
			break;
		case UNKNOWN_DEVICE :
			break;
	}
	if(g_btvout) max98089_routing(3);

	return 0;
err:
	return err;
}

//IRQ Handler
static irqreturn_t detect_irq_handler(int irq, void *dev_id)
{
	schedule_delayed_work(&jack_detect_work, msecs_to_jiffies(500));
	return IRQ_HANDLED;
}
 
static int achro_jack_probe(struct platform_device *pdev)
{
	int ret;
	struct achro_jack_platform_data *pdata = pdev->dev.platform_data;
	struct achro_gpio_info   *det_jack;
	current_jack_type_status = UNKNOWN_DEVICE;
	
	printk(KERN_INFO "ACHRO JACK: Registering jack driver\n");
	
	hi = kzalloc(sizeof(struct achro_jack_info), GFP_KERNEL);
	if (!hi)
		return -ENOMEM;

	memcpy (&hi->port, pdata->port, sizeof(struct achro_jack_port));

	ret = switch_dev_register(&switch_jack_detection);
	if (ret < 0)
	{
		printk(KERN_ERR "SEC JACK: Failed to register switch device\n");
		goto err_switch_dev_register;
	}

	//GPIO configuration
	det_jack = &hi->port.det_jack;
	if (gpio_is_valid(det_jack->gpio))
	{
		ret = gpio_request( det_jack->gpio, "GPIO_HEADSET");
		if(ret)
		{
			printk(KERN_ERR "failed to request GPIO_HEADSET ..\n");
			goto err_switch_dev_register;
		}
		gpio_direction_input(det_jack->gpio);
		s3c_gpio_setpull(det_jack->gpio, S3C_GPIO_PULL_NONE);
		gpio_free(det_jack->gpio);
	}
	else goto err_request_detect_irq;

	ret = request_irq(det_jack->eint, detect_irq_handler, IRQF_DISABLED, "hpjack-irq", NULL);
	if (ret < 0) {
		printk(KERN_ERR "achro HEADSET: Failed to register detect interrupt.\n");
		goto err_request_detect_irq;
	}
	set_irq_type(det_jack->eint, IRQ_TYPE_EDGE_FALLING);
	INIT_DELAYED_WORK(&jack_detect_work, jack_detect_change);

#if defined(CONFIG_FB_S3C_HDMI_UI_FB) && !defined(CONFIG_BATTERY_MAX17040)
	g_btvout = false;
#else
	if(get_hdmi_onoff_status())		g_btvout = true;
	else 	g_btvout = false;
	printk("\n%s g_btvout = %d\n\n",__FUNCTION__, g_btvout);
#endif
	schedule_delayed_work(&jack_detect_work, msecs_to_jiffies(5000));

	return 0;

err_request_detect_irq:
	switch_dev_unregister(&switch_jack_detection);

err_switch_dev_register:
	return ret;
}

static int achro_jack_remove(struct platform_device *pdev)
{
	input_unregister_device(hi->input);
	free_irq(hi->port.det_jack.eint, 0);
	switch_dev_unregister(&switch_jack_detection);
	return 0;
}

#ifdef CONFIG_PM
static int achro_jack_suspend(struct platform_device *pdev, pm_message_t state)
{
	return 0;
}
static int achro_jack_resume(struct platform_device *pdev)
{
	return 0;
}
#else
#define achro_jack_resume	NULL
#define achro_jack_suspend	NULL
#endif

static struct platform_driver achro_jack_driver = {
	.probe		= achro_jack_probe,
	.remove		= achro_jack_remove,
	.suspend	= achro_jack_suspend,
	.resume		= achro_jack_resume,
	.driver		= {
		.name		= "achro_jack",
		.owner		= THIS_MODULE,
	},
};

static int __init achro_jack_init(void)
{
	return platform_driver_register(&achro_jack_driver);
}

static void __exit achro_jack_exit(void)
{
	platform_driver_unregister(&achro_jack_driver);
}
module_init(achro_jack_init);
module_exit(achro_jack_exit);

MODULE_AUTHOR("Uk Kim <w0806.kim@samsung.com>");
MODULE_DESCRIPTION("SEC JACK detection driver");
MODULE_LICENSE("GPL");
