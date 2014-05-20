//[*]--------------------------------------------------------------------------------------------------[*]
/*
 *
 * ACHRO4210 Dev Board keypad driver (charles.park)
 *
 */
//[*]--------------------------------------------------------------------------------------------------[*]
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/input.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/clk.h>

#include <linux/gpio.h>
#include <linux/delay.h>
#include <asm/io.h>
#include <plat/gpio-cfg.h>
#include <mach/regs-gpio.h>

// Debug message enable flag
#define	DEBUG_MSG			
#define	DEBUG_PM_MSG

//[*]--------------------------------------------------------------------------------------------------[*]
#include "achro4210_keypad.h"

//[*]--------------------------------------------------------------------------------------------------[*]
#define DEVICE_NAME "achro4210-keypad"

//[*]--------------------------------------------------------------------------------------------------[*]
achro4210_keypad_t	achro4210_keypad;

//[*]--------------------------------------------------------------------------------------------------[*]
static void				generate_keycode				(unsigned short prev_key, unsigned short cur_key, int *key_table);
static void 			achro4210_poweroff_timer_run		(void);
static void 			achro4210_poweroff_timer_handler	(unsigned long data);

static int				achro4210_keypad_get_data			(void);
static void 			achro4210_keypad_control			(void);

static void				achro4210_rd_timer_handler			(unsigned long data);

static int              achro4210_keypad_open              (struct input_dev *dev);
static void             achro4210_keypad_close             (struct input_dev *dev);

static void             achro4210_keypad_release_device    (struct device *dev);
static int              achro4210_keypad_resume            (struct platform_device *dev);
static int              achro4210_keypad_suspend           (struct platform_device *dev, pm_message_t state);

static void				achro4210_keypad_config			(unsigned char state);

static int __devinit    achro4210_keypad_probe				(struct platform_device *pdev);
static int __devexit    achro4210_keypad_remove			(struct platform_device *pdev);

static int __init       achro4210_keypad_init				(void);
static void __exit      achro4210_keypad_exit				(void);

//[*]--------------------------------------------------------------------------------------------------[*]
static struct platform_driver achro4210_platform_device_driver = {
		.probe          = achro4210_keypad_probe,
		.remove         = achro4210_keypad_remove,
		.suspend        = achro4210_keypad_suspend,
		.resume         = achro4210_keypad_resume,
		.driver		= {
			.owner	= THIS_MODULE,
			.name	= DEVICE_NAME,
		},
};

//[*]--------------------------------------------------------------------------------------------------[*]
static struct platform_device achro4210_platform_device = {
        .name           = DEVICE_NAME,
        .id             = -1,
        .num_resources  = 0,
        .dev    = {
                .release	= achro4210_keypad_release_device,
        },
};

//[*]--------------------------------------------------------------------------------------------------[*]
module_init(achro4210_keypad_init);
module_exit(achro4210_keypad_exit);

//[*]--------------------------------------------------------------------------------------------------[*]
MODULE_AUTHOR("Hard-Kernel");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Keypad interface for achro4210-Dev board");

//[*]--------------------------------------------------------------------------------------------------[*]
//   Control GPIO Define
//[*]--------------------------------------------------------------------------------------------------[*]
// GPIO Index Define
enum	{
	// KEY CONTROL
	KEYPAD_POWER_ONOFF,
	
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
	// High -> POWER ON(ONO)
	{	KEYPAD_POWER_ONOFF,	S5PV310_GPX0(1), "key power on/off", 0, 0, S3C_GPIO_PULL_NONE},
};

//[*]--------------------------------------------------------------------------------------------------[*]
#define	MAX_KEYCODE_CNT		1

int ACHRO4210_Keycode[MAX_KEYCODE_CNT] = {
		KEY_POWER
};

#if	defined(DEBUG_MSG)
	const char ACHRO4210_KeyMapStr[MAX_KEYCODE_CNT][20] = {
			"KEY_POWER\n"
	};
#endif	// DEBUG_MSG

//[*]--------------------------------------------------------------------------------------------------[*]
static void achro4210_poweroff_timer_run(void)
{
	init_timer(&achro4210_keypad.poweroff_timer);
	achro4210_keypad.poweroff_timer.function = achro4210_poweroff_timer_handler;
	achro4210_keypad.poweroff_timer.expires = jiffies + PERIOD_5SEC;
	add_timer(&achro4210_keypad.poweroff_timer);
}

//[*]--------------------------------------------------------------------------------------------------[*]
extern	void 	SYSTEM_POWER_CONTROL(int power, int val);	// achro4210-sysfs.c

static void achro4210_poweroff_timer_handler(unsigned long data)
{
	// PS_HOLD Enable (LOW-> HIGH)
	SYSTEM_POWER_CONTROL(0, 0);	// 2V8 Off
	SYSTEM_POWER_CONTROL(1, 0);	// 3V3 Off
	SYSTEM_POWER_CONTROL(2, 1);	// 5V0 Off
	printk(KERN_EMERG "%s : setting GPIO_PDA_PS_HOLD low.\n", __func__);
	(*(unsigned long *)(S5PV310_VA_PMU + 0x330C)) = 0x5200;
}

//[*]--------------------------------------------------------------------------------------------------[*]
static void	generate_keycode(unsigned short prev_key, unsigned short cur_key, int *key_table)
{
	unsigned short 	press_key, release_key, i;
	
	press_key	= (cur_key ^ prev_key) & cur_key;
	release_key	= (cur_key ^ prev_key) & prev_key;
	
	i = 0;
	while(press_key)	{
		if(press_key & 0x01)	{
			
			input_report_key(achro4210_keypad.driver, key_table[i], KEY_PRESS);
			
			// POWER OFF PRESS
			if(key_table[i] == KEY_POWER)	achro4210_poweroff_timer_run();
		}
		i++;	press_key >>= 1;
	}
	
	i = 0;
	while(release_key)	{
		if(release_key & 0x01)	{

			input_report_key(achro4210_keypad.driver, key_table[i], KEY_RELEASE);

			// POWER OFF (LONG PRESS)		
			if(key_table[i] == KEY_POWER)		del_timer_sync(&achro4210_keypad.poweroff_timer);
		}
		i++;	release_key >>= 1;
	}
}
//[*]--------------------------------------------------------------------------------------------------[*]
#if defined(DEBUG_MSG)
	static void debug_keycode_printf(unsigned short prev_key, unsigned short cur_key, const char *key_table)
	{
		unsigned short 	press_key, release_key, i;
		
		press_key	= (cur_key ^ prev_key) & cur_key;
		release_key	= (cur_key ^ prev_key) & prev_key;
		
		i = 0;
		while(press_key)	{
			if(press_key & 0x01)	printk("PRESS KEY : %s", (char *)&key_table[i * 20]);
			i++;					press_key >>= 1;
		}
		
		i = 0;
		while(release_key)	{
			if(release_key & 0x01)	printk("RELEASE KEY : %s", (char *)&key_table[i * 20]);
			i++;					release_key >>= 1;
		}
	}
#endif

//[*]--------------------------------------------------------------------------------------------------[*]
static int	achro4210_keypad_get_data(void)
{
	int		key_data = 0;

	key_data |= (gpio_get_value(sControlGpios[KEYPAD_POWER_ONOFF].gpio) ? 0 : 0x01);
		
	return	key_data;
}

//[*]--------------------------------------------------------------------------------------------------[*]
static void achro4210_keypad_control(void)
{
	static	unsigned short	prev_keypad_data = 0, cur_keypad_data = 0;

	// key data process
	cur_keypad_data = achro4210_keypad_get_data();

	if(prev_keypad_data != cur_keypad_data)	{
		
		generate_keycode(prev_keypad_data, cur_keypad_data, &ACHRO4210_Keycode[0]);

		#if defined(DEBUG_MSG)
			debug_keycode_printf(prev_keypad_data, cur_keypad_data, &ACHRO4210_KeyMapStr[0][0]);
		#endif

		prev_keypad_data = cur_keypad_data;

		input_sync(achro4210_keypad.driver);
	}
}

//[*]--------------------------------------------------------------------------------------------------[*]
static void	achro4210_rd_timer_handler(unsigned long data)
{
    unsigned long flags;

    local_irq_save(flags);

	achro4210_keypad_control();
	mod_timer(&achro4210_keypad.rd_timer,jiffies + PERIOD_50MS);

    local_irq_restore(flags);
}

//[*]--------------------------------------------------------------------------------------------------[*]
static int	achro4210_keypad_open(struct input_dev *dev)
{
	#if	defined(DEBUG_MSG)
		printk("%s\n", __FUNCTION__);
	#endif
	
	return	0;
}

//[*]--------------------------------------------------------------------------------------------------[*]
static void	achro4210_keypad_close(struct input_dev *dev)
{
	#if	defined(DEBUG_MSG)
		printk("%s\n", __FUNCTION__);
	#endif
}

//[*]--------------------------------------------------------------------------------------------------[*]
static void	achro4210_keypad_release_device(struct device *dev)
{
	#if	defined(DEBUG_MSG)
		printk("%s\n", __FUNCTION__);
	#endif
}

//[*]--------------------------------------------------------------------------------------------------[*]
static int	achro4210_keypad_resume(struct platform_device *dev)
{
	#if	defined(DEBUG_PM_MSG)
		printk("%s\n", __FUNCTION__);
	#endif
	
	achro4210_keypad_config(KEYPAD_STATE_RESUME);

    return  0;
}

//[*]--------------------------------------------------------------------------------------------------[*]
static int	achro4210_keypad_suspend(struct platform_device *dev, pm_message_t state)
{
	#if	defined(DEBUG_PM_MSG)
		printk("%s\n", __FUNCTION__);
	#endif

	del_timer_sync(&achro4210_keypad.rd_timer);
	
    return  0;
}

//[*]--------------------------------------------------------------------------------------------------[*]
static void	achro4210_keypad_config(unsigned char state)
{
	if(state == KEYPAD_STATE_BOOT)	{
		int	i;
	
		// Control GPIO Init
		for (i = 0; i < ARRAY_SIZE(sControlGpios); i++) {
			if(gpio_request(sControlGpios[i].gpio, sControlGpios[i].name))	{
				printk("--------------------------------------------\n");
				printk("%s : %s gpio reqest err!\n", __FUNCTION__, sControlGpios[i].name);
				printk("--------------------------------------------\n");
			}
			else	{
				if(sControlGpios[i].output)		gpio_direction_output	(sControlGpios[i].gpio, sControlGpios[i].value);
				else							gpio_direction_input	(sControlGpios[i].gpio);
	
				s3c_gpio_setpull(sControlGpios[i].gpio, sControlGpios[i].pud);
			}
		}
	}

	/* Scan timer init */
	init_timer(&achro4210_keypad.rd_timer);

	achro4210_keypad.rd_timer.function = achro4210_rd_timer_handler;
	achro4210_keypad.rd_timer.expires = jiffies + (HZ/100);

	add_timer(&achro4210_keypad.rd_timer);
}

//[*]--------------------------------------------------------------------------------------------------[*]
static int __devinit    achro4210_keypad_probe(struct platform_device *pdev)
{
    int 	key, code;

	// struct init
	memset(&achro4210_keypad, 0x00, sizeof(achro4210_keypad_t));
	
	achro4210_keypad.driver = input_allocate_device();
	
    if(!(achro4210_keypad.driver))	{
		printk("--------------------------------------------------------\n");
		printk("ERROR! : %s input_allocate_device() error!!! no memory!!\n", __FUNCTION__);
		printk("--------------------------------------------------------\n");
		return -ENOMEM;
    }

	set_bit(EV_KEY, achro4210_keypad.driver->evbit);
//	set_bit(EV_REP, achro4210_keypad.driver->evbit);	// Repeat Key

	for(key = 0; key < MAX_KEYCODE_CNT; key++){
		code = ACHRO4210_Keycode[key];
		if(code<=0)
			continue;
		set_bit(code & KEY_MAX, achro4210_keypad.driver->keybit);
	}

	achro4210_keypad.driver->name 	= DEVICE_NAME;
	achro4210_keypad.driver->phys 	= "achro4210-keypad/input0";
    achro4210_keypad.driver->open 	= achro4210_keypad_open;
    achro4210_keypad.driver->close	= achro4210_keypad_close;
	
	achro4210_keypad.driver->id.bustype	= BUS_HOST;
	achro4210_keypad.driver->id.vendor 	= 0x16B4;
	achro4210_keypad.driver->id.product 	= 0x0701;
	achro4210_keypad.driver->id.version 	= 0x0001;

	achro4210_keypad.driver->keycode = ACHRO4210_Keycode;

	if(input_register_device(achro4210_keypad.driver))	{
		printk("--------------------------------------------------------\n");
		printk("HardKernel-C110 keypad input register device fail!!\n");
		printk("--------------------------------------------------------\n");
		input_free_device(achro4210_keypad.driver);	return	-ENODEV;
	}

	achro4210_keypad_config(KEYPAD_STATE_BOOT);

	printk("Huins : ACHRO-E Keypad driver initialized!! Ver 1.0\n");

    return 0;
}

//[*]--------------------------------------------------------------------------------------------------[*]
static int __devexit    achro4210_keypad_remove(struct platform_device *pdev)
{
	int 	i;
	
	input_unregister_device(achro4210_keypad.driver);
	
	del_timer_sync(&achro4210_keypad.rd_timer);
	
	for (i = 0; i < ARRAY_SIZE(sControlGpios); i++) 	gpio_free(sControlGpios[i].gpio);

	#if	defined(DEBUG_MSG)
		printk("%s\n", __FUNCTION__);
	#endif
	
	return  0;
}

//[*]--------------------------------------------------------------------------------------------------[*]
static int __init	achro4210_keypad_init(void)
{
	int ret = platform_driver_register(&achro4210_platform_device_driver);
	
	#if	defined(DEBUG_MSG)
		printk("%s\n", __FUNCTION__);
	#endif
	
	if(!ret)        {
		ret = platform_device_register(&achro4210_platform_device);
		
		#if	defined(DEBUG_MSG)
			printk("platform_driver_register %d \n", ret);
		#endif
		
		if(ret)	platform_driver_unregister(&achro4210_platform_device_driver);
	}
	return ret;
}

//[*]--------------------------------------------------------------------------------------------------[*]
static void __exit	achro4210_keypad_exit(void)
{
	#if	defined(DEBUG_MSG)
		printk("%s\n",__FUNCTION__);
	#endif
	
	platform_device_unregister(&achro4210_platform_device);
	platform_driver_unregister(&achro4210_platform_device_driver);
}

//[*]--------------------------------------------------------------------------------------------------[*]
