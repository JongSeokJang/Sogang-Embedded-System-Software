
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
//#include <plat/regs-gpio.h>

//[*]--------------------------------------------------------------------------------------------------[*]
#ifdef CONFIG_HAS_EARLYSUSPEND
	#include <linux/wakelock.h>
	#include <linux/earlysuspend.h>
	#include <linux/suspend.h>
#endif

//[*]--------------------------------------------------------------------------------------------------[*]

#include "achro4210_touch.h"
#include "ep0700_touch_gpio_i2c.h"
#include "achro4210_touch_sysfs.h"

#include "../keyboard/achro4210_keypad.h"

//[*]--------------------------------------------------------------------------------------------------[*]
//#define	DEBUG_ACHRO4210_TOUCH_MSG
#define	DEBUG_ACHRO4210_TOUCH_PM_MSG





/* multi-touch data process struct */
typedef struct	touch_process_data__t	{
    unsigned 	char	finger_cnt;
    unsigned 	int		x1;
    unsigned 	int		y1;
    unsigned 	int		x2;
    unsigned 	int		y2;
} touch_process_data_t;


#define MODULE_CSR 0x04
#define MODULE_INTCLEAR 0x07

#define MODULE_X1H 0x09
#define MODULE_X1L 0x0A
#define MODULE_Y1H 0x0B
#define MODULE_Y1L 0x0C
#define MODULE_X2H 0x0D
#define MODULE_X2L 0x0E
#define MODULE_Y2H 0x0F
#define MODULE_Y2L 0x10
#define MODULE_FINGER 0x11






//[*]--------------------------------------------------------------------------------------------------[*]
achro4210_touch_t	achro4210_touch;


#if 0
//[*]--------------------------------------------------------------------------------------------------[*]
static void 			achro4210_touch_timer_start	(void);
static void				achro4210_touch_timer_irq		(unsigned long arg);

static void 			achro4210_touch_process_data	(touch_process_data_t *ts_data);
static void 			achro4210_touch_get_data		(void);

irqreturn_t				achro4210_touch_irq			(int irq, void *dev_id);

static int              achro4210_touch_open			(struct input_dev *dev);
static void             achro4210_touch_close			(struct input_dev *dev);

static void             achro4210_touch_release_device	(struct device *dev);

#ifdef CONFIG_HAS_EARLYSUSPEND
	static void				achro4210_touch_early_suspend	(struct early_suspend *h);
	static void				achro4210_touch_late_resume	(struct early_suspend *h);
#else
	static int              achro4210_touch_resume			(struct platform_device *dev);
	static int              achro4210_touch_suspend		(struct platform_device *dev, pm_message_t state);
#endif

static void				achro4210_touch_reset			(void);
static void 			achro4210_touch_config			(unsigned char state);

static int __devinit    achro4210_touch_probe			(struct platform_device *pdev);
static int __devexit    achro4210_touch_remove			(struct platform_device *pdev);




static int __init       achro4210_touch_init			(void);
static void __exit      achro4210_touch_exit			(void);
#endif
#if 0

//[*]--------------------------------------------------------------------------------------------------[*]
static void	achro4210_touch_timer_irq(unsigned long arg)
{
	// Acc data read
	write_seqlock(&achro4210_touch.lock);

	achro4210_touch_get_data();		//achro4210_touch_timer_start();

	write_sequnlock(&achro4210_touch.lock);
}

//[*]--------------------------------------------------------------------------------------------------[*]
static void achro4210_touch_timer_start(void)
{
	init_timer(&achro4210_touch.penup_timer);
	achro4210_touch.penup_timer.data 		= (unsigned long)&achro4210_touch.penup_timer;
	achro4210_touch.penup_timer.function 	= achro4210_touch_timer_irq;

	switch(achro4210_touch.sampling_rate)	{
		default	:
			achro4210_touch.sampling_rate = 0;
			achro4210_touch.penup_timer.expires = jiffies + PERIOD_10MS;
			break;
		case	1:
			achro4210_touch.penup_timer.expires = jiffies + PERIOD_20MS;
			break;
		case	2:
			achro4210_touch.penup_timer.expires = jiffies + PERIOD_50MS;
			break;
	}

	add_timer(&achro4210_touch.penup_timer);
}
#endif


//[*]--------------------------------------------------------------------------------------------------[*]
static void achro4210_touch_process_data(touch_process_data_t *ts_data)
{
	unsigned char	wdata;
	
	// read address setup
	//achro4210_touch_write(0x00, NULL, 0x00);

	// Acc data read
	write_seqlock(&achro4210_touch.lock);
	//achro4210_touch_read(&achro4210_touch.rd[0], 10);
	achro4210_touch_read(MODULE_FINGER, &achro4210_touch.rd[0], 1);
	achro4210_touch_read(MODULE_X1H, &achro4210_touch.rd[3], 1);
	achro4210_touch_read(MODULE_X1L, &achro4210_touch.rd[2], 1);
	achro4210_touch_read(MODULE_Y1H, &achro4210_touch.rd[5], 1);
	achro4210_touch_read(MODULE_Y1L, &achro4210_touch.rd[4], 1);
	achro4210_touch_read(MODULE_X2H, &achro4210_touch.rd[7], 1);
	achro4210_touch_read(MODULE_X2L, &achro4210_touch.rd[6], 1);
	achro4210_touch_read(MODULE_Y2H, &achro4210_touch.rd[9], 1);
	achro4210_touch_read(MODULE_Y2L, &achro4210_touch.rd[8], 1);
	wdata = 0x01;
	achro4210_touch_write(MODULE_INTCLEAR, &wdata, 1);
	write_sequnlock(&achro4210_touch.lock);

	ts_data->finger_cnt = achro4210_touch.rd[0] & 0x03;

	if((ts_data->x1 = ((achro4210_touch.rd[3] << 8) | achro4210_touch.rd[2])))	{
		//ts_data->x1 = (ts_data->x1 * 133) / 100;
		//ts_data->x1 = TS_ABS_MAX_X - ts_data->x1;	// flip X & resize
	}
		
	if((ts_data->y1 = ((achro4210_touch.rd[5] << 8) | achro4210_touch.rd[4])))	{
		//ts_data->y1 = (ts_data->y1 * 128 ) / 100;	// resize
	}

	
	{
		unsigned int tmp = ts_data->x1;

		ts_data->x1 = abs(ts_data->y1 - 480);
		ts_data->y1 = tmp;
	}	
	
	if(ts_data->finger_cnt > 1)	{
		if((ts_data->x2 = ((achro4210_touch.rd[7] << 8) | achro4210_touch.rd[6])))	{
			//ts_data->x2 = (ts_data->x2 * 133) / 100;
			//ts_data->x2 = TS_ABS_MAX_X - ts_data->x2;	// flip X & resize
		}
		
		if((ts_data->y2 = ((achro4210_touch.rd[9] << 8) | achro4210_touch.rd[8])))	{
			//ts_data->y2 = (ts_data->y2 * 128 ) / 100;	// resize
		}

		{
			unsigned int tmp = ts_data->x2;

			ts_data->x2 = abs(ts_data->y2 - 480);
			ts_data->y2 = tmp;
		}	

	}
}

//[*]--------------------------------------------------------------------------------------------------[*]
unsigned char		prev_key = 0;
unsigned char		cur_key = 0;
extern achro4210_keypad_t	achro4210_keypad;
static void achro4210_touch_get_data(void)
{
//	unsigned int	temp_x, temp_y, cnt;
	touch_process_data_t	ts_data;
	int timer_set_flag = 0;
	
	achro4210_touch_process_data(&ts_data);
		
	if(ts_data.finger_cnt > 0 && ts_data.finger_cnt < 3)	{
		achro4210_touch.x 	= ts_data.x1;	achro4210_touch.y	= ts_data.y1;
		input_report_abs(achro4210_touch.driver, ABS_MT_TOUCH_MAJOR, 200);   // press               
		input_report_abs(achro4210_touch.driver, ABS_MT_WIDTH_MAJOR, 10);
		input_report_abs(achro4210_touch.driver, ABS_MT_POSITION_X, achro4210_touch.x);
		input_report_abs(achro4210_touch.driver, ABS_MT_POSITION_Y, achro4210_touch.y);
		input_mt_sync(achro4210_touch.driver);
//		printk("%s x1=%d, y1=%d\n", __FUNCTION__, achro4210_touch.x,achro4210_touch.y);
		
		if(ts_data.finger_cnt > 1)	{
			achro4210_touch.x 	= ts_data.x2;	achro4210_touch.y	= ts_data.y2;
			input_report_abs(achro4210_touch.driver, ABS_MT_TOUCH_MAJOR, 200);   // press               
			input_report_abs(achro4210_touch.driver, ABS_MT_WIDTH_MAJOR, 10);
			input_report_abs(achro4210_touch.driver, ABS_MT_POSITION_X, achro4210_touch.x);
			input_report_abs(achro4210_touch.driver, ABS_MT_POSITION_Y, achro4210_touch.y);
			input_mt_sync(achro4210_touch.driver);
//			printk("%s x2=%d, y2=%d\n", __FUNCTION__, achro4210_touch.x, achro4210_touch.y);
		}
		
		input_sync(achro4210_touch.driver);
		timer_set_flag = 1;
	}
	else	{
		input_report_abs(achro4210_touch.driver, ABS_MT_TOUCH_MAJOR, 0);   // press               
		input_report_abs(achro4210_touch.driver, ABS_MT_WIDTH_MAJOR, 10);
		input_report_abs(achro4210_touch.driver, ABS_MT_POSITION_X, achro4210_touch.x);
		input_report_abs(achro4210_touch.driver, ABS_MT_POSITION_Y, achro4210_touch.y);
		input_mt_sync(achro4210_touch.driver);
		input_sync(achro4210_touch.driver);
//		printk("%s x1=%d, y1=%d\n", __FUNCTION__, achro4210_touch.x, achro4210_touch.y);
	}
#if 0 //jhkang: convert touch event to key event
	{

		unsigned char 	i;
		unsigned char 	press_key, release_key;
		unsigned int achro210T_key_pad[] = {KEY_BACK, KEY_SEARCH, KEY_HOME, KEY_MENU};

		cur_key = achro4210_touch.rd[10] & 0x0F;

		cur_key &= 0x0F;

		printk(KERN_INFO "[%s:%s:%d]rd=0x%x, cur_key=0x%x\n",__FILE__, __FUNCTION__, __LINE__,achro4210_touch.rd[10],cur_key);		
		if(prev_key != cur_key)	
		{
			press_key	= (cur_key ^ prev_key) & cur_key;
			release_key	= (cur_key ^ prev_key) & prev_key;
				
			i = 0;
			while(press_key)	{
				if(press_key & 0x01)	{
					input_report_key(achro4210_keypad.driver, achro210T_key_pad[i], KEY_PRESS);
	//				printk(KERN_ERR "[touch key] %d press\n", i);
				}
				i++;	press_key >>= 1;
			}

			i = 0;
			while(release_key)	{
				if(release_key & 0x01)	{
					input_report_key(achro4210_keypad.driver, achro210T_key_pad[i], KEY_RELEASE);
	//				printk(KERN_ERR "[touch key] %d release\n", i);
				}
				i++;	release_key >>= 1;
			}

			prev_key = cur_key;

		}

		if(prev_key)
			timer_set_flag = 1;
	}
#endif	
}

//------------------------------------------------------------------------------------------------------------------------
irqreturn_t	achro4210_touch_irq(int irq, void *dev_id)
{
	unsigned long	flags;
	
	printk(KERN_INFO "[%s:%s:%d]rd=0x%x, cur_key=0x%x\n",__FILE__, __FUNCTION__, __LINE__,achro4210_touch.rd[10],cur_key);		
	local_irq_save(flags); local_irq_disable();

//printk("%s\n", __FUNCTION__);	

	achro4210_touch_get_data();

	local_irq_restore(flags);
	return	IRQ_HANDLED;
}


//[*]--------------------------------------------------------------------------------------------------[*]
static void		achro4210_touch_reset(void)
{
	if(gpio_request(TS_RESET_OUT,"TS_RESET_OUT"))	{
		printk("%s : request port error!\n", __FUNCTION__);
	}

	gpio_direction_output(TS_RESET_OUT, 1);	
	s3c_gpio_setpull(TS_RESET_OUT, S3C_GPIO_PULL_DOWN);

	gpio_set_value(TS_RESET_OUT, 0);	mdelay(10);
	gpio_set_value(TS_RESET_OUT, 1);	mdelay(10);
	gpio_set_value(TS_RESET_OUT, 0);	mdelay(10);

	gpio_free(TS_RESET_OUT);
}


//[*]--------------------------------------------------------------------------------------------------[*]
static void 	achro4210_touch_config(unsigned char state)
{
	unsigned char	wdata;

	achro4210_touch_reset();	
	achro4210_touch_port_init();
	mdelay(500);

	// Touchscreen Active mode
	wdata = 0x09;
	achro4210_touch_write(MODULE_CSR, &wdata, 1);
	wdata = 0x01;
	achro4210_touch_write(MODULE_INTCLEAR, &wdata, 1);
	mdelay(10);

	if(state == TOUCH_STATE_BOOT)	{

		if(!request_irq(ACHRO4210_TOUCH_IRQ, achro4210_touch_irq, IRQF_DISABLED, "ACHRO4210-Touch IRQ", (void *)&achro4210_touch))
			printk("ACHRO4210 TOUCH request_irq = %d\r\n" , ACHRO4210_TOUCH_IRQ);
		else
			printk("ACHRO4210 TOUCH request_irq = %d error!! \r\n", ACHRO4210_TOUCH_IRQ);

		set_irq_type(ACHRO4210_TOUCH_IRQ, IRQ_TYPE_EDGE_FALLING);
					//IRQ_TYPE_EDGE_BOTH);//IRQ_TYPE_EDGE_FALLING);  //IRQ_TYPE_EDGE_RISING
		
		// seqlock init
		seqlock_init(&achro4210_touch.lock);		achro4210_touch.seq = 0;
	}
	else {
#if 0
		// INT_mode : disable interrupt, low-active, finger moving
		wdata = 0x01;
		achro4210_touch_write(MODULE_INTMODE, &wdata, 1);
		mdelay(10);
		// INT_mode : enable interrupt, low-active, finger moving
		wdata = 0x09;
		achro4210_touch_write(MODULE_INTMODE, &wdata, 1);
		mdelay(10);
#endif
	}
}

#ifdef	CONFIG_HAS_EARLYSUSPEND
static void		achro4210_touch_late_resume	(struct early_suspend *h)
#else
static 	int		achro4210_touch_resume			(struct platform_device *dev)
#endif
{
//	#if	defined(DEBUG_ACHRO4210_TOUCH_PM_MSG)
		printk("%s\n", __FUNCTION__);
//	#endif

#if 0
	achro4210_touch_config(TOUCH_STATE_RESUME);
#endif

	// interrupt enable
	enable_irq(ACHRO4210_TOUCH_IRQ);

#ifndef	CONFIG_HAS_EARLYSUSPEND
	return	0;
#endif	
}
//[*]--------------------------------------------------------------------------------------------------[*]
#ifdef	CONFIG_HAS_EARLYSUSPEND
static void		achro4210_touch_early_suspend	(struct early_suspend *h)
#else
static 	int		achro4210_touch_suspend		(struct platform_device *dev, pm_message_t state)
#endif
{
//	unsigned char	wdata;
//	#if	defined(DEBUG_ACHRO4210_TOUCH_PM_MSG)
		printk("%s\n", __FUNCTION__);
//	#endif

#if 0
	wdata = 0x00;
	achro4210_touch_write(MODULE_POWERMODE, &wdata, 1);	
	mdelay(10);

	// INT_mode : disable interrupt
	wdata = 0x00;
	achro4210_touch_write(MODULE_INTMODE, &wdata, 1);
	mdelay(10);

	// Touchscreen enter freeze mode : 
	wdata = 0x03;
	achro4210_touch_write(MODULE_POWERMODE, &wdata, 1);
	mdelay(10);
#endif

	// interrupt disable
	disable_irq(ACHRO4210_TOUCH_IRQ);

#ifndef	CONFIG_HAS_EARLYSUSPEND
	return	0;
#endif	
}

//[*]--------------------------------------------------------------------------------------------------[*]
static int	achro4210_touch_open	(struct input_dev *dev)
{
	#if	defined(DEBUG_ACHRO4210_TOUCH_MSG)
		printk("%s\n", __FUNCTION__);
	#endif
	
	return	0;
}

//[*]--------------------------------------------------------------------------------------------------[*]
static void	achro4210_touch_close	(struct input_dev *dev)
{
	#if	defined(DEBUG_ACHRO4210_TOUCH_MSG)
		printk("%s\n", __FUNCTION__);
	#endif
}

//[*]--------------------------------------------------------------------------------------------------[*]
static int __devinit   achro4210_touch_probe	(struct platform_device *pdev)
{
	int		rc;

	// struct init
	memset(&achro4210_touch, 0x00, sizeof(achro4210_touch_t));

	// create sys_fs
	if((rc = achro4210_touch_sysfs_create(pdev)))	{
		printk("%s : sysfs_create_group fail!!\n", __FUNCTION__);
		return	rc;
	}

	achro4210_touch.driver = input_allocate_device();

	if(!(achro4210_touch.driver))	{
		printk("ERROR! : %s cdev_alloc() error!!! no memory!!\n", __FUNCTION__);
		achro4210_touch_sysfs_remove(pdev);
		return -ENOMEM;
	}

	achro4210_touch.driver->name 	= ACHRO4210_TOUCH_DEVICE_NAME;
	achro4210_touch.driver->phys 	= "achro4210_touch/input1";
	achro4210_touch.driver->open 	= achro4210_touch_open;
	achro4210_touch.driver->close		= achro4210_touch_close;

	achro4210_touch.driver->id.bustype	= BUS_HOST;
	achro4210_touch.driver->id.vendor 	= 0x16B4;
	achro4210_touch.driver->id.product	= 0x0702;
	achro4210_touch.driver->id.version	= 0x0001;

	set_bit(EV_ABS, achro4210_touch.driver->evbit);

	/* multi touch */
	input_set_abs_params(achro4210_touch.driver, ABS_MT_POSITION_X, TS_ABS_MIN_X, TS_ABS_MAX_X,	0, 0);
	input_set_abs_params(achro4210_touch.driver, ABS_MT_POSITION_Y, TS_ABS_MIN_Y, TS_ABS_MAX_Y,	0, 0);
	input_set_abs_params(achro4210_touch.driver, ABS_MT_TOUCH_MAJOR, 0, 255, 2, 0);
	input_set_abs_params(achro4210_touch.driver, ABS_MT_WIDTH_MAJOR, 0,  15, 2, 0);

	if(input_register_device(achro4210_touch.driver))	{
		printk("ACHRO4210 TOUCH input register device fail!!\n");

		achro4210_touch_sysfs_remove(pdev);
		input_free_device(achro4210_touch.driver);		return	-ENODEV;
	}

#ifdef CONFIG_HAS_EARLYSUSPEND
	achro4210_touch.power.suspend 	= achro4210_touch_early_suspend;
	achro4210_touch.power.resume 	= achro4210_touch_late_resume;
	achro4210_touch.power.level 	= EARLY_SUSPEND_LEVEL_DISABLE_FB-1;
	//if, is in USER_SLEEP status and no active auto expiring wake lock
	//if (has_wake_lock(WAKE_LOCK_SUSPEND) == 0 && get_suspend_state() == PM_SUSPEND_ON)
	register_early_suspend(&achro4210_touch.power);
#endif

#if 0
	if (gpio_is_valid(TS_ATTB))
	{
		if(gpio_request( TS_ATTB, "TS_ATTB")) {
			printk(KERN_ERR "failed to request GPH1 for TS_ATTB..\n");
			return -1;
		}
		gpio_direction_input(TS_ATTB);
		s3c_gpio_setpull(TS_ATTB, S3C_GPIO_PULL_NONE);

		gpio_free(TS_ATTB);
	}
#endif//jhkang: 일단 막자. 
	achro4210_touch_config(TOUCH_STATE_BOOT);

	printk("--------------------------------------------------------\n");
	printk("Huins : Achro4210 (MT) Multi-Touch driver initialized!! Ver 1.0\n");
	printk("--------------------------------------------------------\n");

	return	0;
}

//[*]--------------------------------------------------------------------------------------------------[*]
static int __devexit    achro4210_touch_remove	(struct platform_device *pdev)
{
	#if	defined(DEBUG_ACHRO4210_TOUCH_MSG)
		printk("%s\n", __FUNCTION__);
	#endif
	
	free_irq(ACHRO4210_TOUCH_IRQ, (void *)&achro4210_touch); 

	achro4210_touch_sysfs_remove(pdev);

	input_unregister_device(achro4210_touch.driver);

	return  0;
}

//[*]--------------------------------------------------------------------------------------------------[*]
static void	achro4210_touch_release_device	(struct device *dev)
{
	#if	defined(DEBUG_ACHRO4210_TOUCH_MSG)
		printk("%s\n", __FUNCTION__);
	#endif
}


//[*]--------------------------------------------------------------------------------------------------[*]
static struct platform_driver achro4210_touch_platform_device_driver = {
		.probe          = achro4210_touch_probe,
		.remove         = achro4210_touch_remove,

#ifndef CONFIG_HAS_EARLYSUSPEND
		.suspend        = achro4210_touch_suspend,
		.resume         = achro4210_touch_resume,
#endif
		.driver		= {
			.owner	= THIS_MODULE,
			.name	= ACHRO4210_TOUCH_DEVICE_NAME,
		},
};

//[*]--------------------------------------------------------------------------------------------------[*]
static struct platform_device achro4210_touch_platform_device = {
        .name           = ACHRO4210_TOUCH_DEVICE_NAME,
        .id             = -1,
        .num_resources  = 0,
        .dev    = {
                .release	= achro4210_touch_release_device,
        },
};

//[*]--------------------------------------------------------------------------------------------------[*]
static int __init 	achro4210_touch_init	(void)
{
	int ret = platform_driver_register(&achro4210_touch_platform_device_driver);
	
	#if	defined(DEBUG_ACHRO4210_TOUCH_MSG)
		printk("%s\n", __FUNCTION__);
	#endif
	
	if(!ret)        {
		ret = platform_device_register(&achro4210_touch_platform_device);
		
		#if	defined(DEBUG_ACHRO4210_TOUCH_MSG)
			printk("platform_driver_register %d \n", ret);
		#endif
		
		if(ret)	platform_driver_unregister(&achro4210_touch_platform_device_driver);
	}
	return ret;
}

//[*]--------------------------------------------------------------------------------------------------[*]
static void __exit	achro4210_touch_exit	(void)
{
	#if	defined(DEBUG_ACHRO4210_TOUCH_MSG)
		printk("%s\n",__FUNCTION__);
	#endif
	
	platform_device_unregister(&achro4210_touch_platform_device);
	platform_driver_unregister(&achro4210_touch_platform_device_driver);
}


//[*]--------------------------------------------------------------------------------------------------[*]
module_init(achro4210_touch_init);
module_exit(achro4210_touch_exit);

//[*]--------------------------------------------------------------------------------------------------[*]
MODULE_AUTHOR("Huins");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("7\" WXGA Touch panel for Achro 4210 Dev Board");//[*]--------------------------------------------------------------------------------------------------[*]
//[*]--------------------------------------------------------------------------------------------------[*]
//[*]--------------------------------------------------------------------------------------------------[*]


