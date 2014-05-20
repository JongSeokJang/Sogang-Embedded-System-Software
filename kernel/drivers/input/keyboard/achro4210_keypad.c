//[*]--------------------------------------------------------------------------------------------------[*]
/*
 *
 * achro4210 Dev Board keypad driver (charles.park)
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

#include <linux/delay.h>
#include <linux/io.h>
#include <mach/hardware.h>
#include <mach/regs-gpio.h>
#include <mach/regs-clock.h>
#include <asm/delay.h>
#include <asm/irq.h>

#include <mach/gpio.h>
#include <asm/gpio.h>
#include <plat/gpio-cfg.h>

// Debug message enable flag
#define	DEBUG_MSG			
#define	DEBUG_PM_MSG

//#define LED_FND_TEST
//#define  LED_ADDRESS_ACCESS
#include <linux/regulator/machine.h>

//[*]--------------------------------------------------------------------------------------------------[*]
#include "achro4210_keypad.h"
#include "achro4210_keycode.h"
#include "achro4210_keypad_sysfs.h"

//[*]--------------------------------------------------------------------------------------------------[*]
#define DEVICE_NAME "achro4210-keypad"

//[*]--------------------------------------------------------------------------------------------------[*]
achro4210_keypad_t	achro4210_keypad;

//[*]--------------------------------------------------------------------------------------------------[*]
static void				generate_keycode				(unsigned short prev_key, unsigned short cur_key, int *key_table);
static void 			achro4210_poweroff_timer_run		(void);
static void 			achro4210_poweroff_timer_handler	(unsigned long data);

static int				achro4210_keypad_get_data			(void);
static void				achro4210_keypad_port_init			(void);
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

#ifdef LED_ADDRESS_ACCESS
static void __iomem *s3c_led_base;
#endif

#define FND_ONE {gpio_set_value(S5PV310_GPL2(6), 0);gpio_set_value(S5PV310_GPL2(5), 0);}
#define FND_TWO {gpio_set_value(S5PV310_GPL2(1), 0);gpio_set_value(S5PV310_GPL2(3), 0);\
			     gpio_set_value(S5PV310_GPL2(4), 0);gpio_set_value(S5PV310_GPL2(6), 0);\
			     gpio_set_value(S5PV310_GPL2(7), 0);}
#define FND_THREE {gpio_set_value(S5PV310_GPL2(1), 0);gpio_set_value(S5PV310_GPL2(4), 0);\
			     gpio_set_value(S5PV310_GPL2(5), 0);gpio_set_value(S5PV310_GPL2(6), 0);\
			     gpio_set_value(S5PV310_GPL2(7), 0);}
#define FND_FOUR {gpio_set_value(S5PV310_GPL2(1), 0);gpio_set_value(S5PV310_GPL2(2), 0);\
			     gpio_set_value(S5PV310_GPL2(5), 0);gpio_set_value(S5PV310_GPL2(6), 0);}
#define FND_FIVE {gpio_set_value(S5PV310_GPL2(1), 0);gpio_set_value(S5PV310_GPL2(2), 0);\
			     gpio_set_value(S5PV310_GPL2(4), 0);gpio_set_value(S5PV310_GPL2(5), 0);\
			     gpio_set_value(S5PV310_GPL2(7), 0);}
#define FND_SIX {gpio_set_value(S5PV310_GPL2(1), 0);gpio_set_value(S5PV310_GPL2(2), 0);\
			     gpio_set_value(S5PV310_GPL2(3), 0);gpio_set_value(S5PV310_GPL2(4), 0);\
			     gpio_set_value(S5PV310_GPL2(5), 0); gpio_set_value(S5PV310_GPL2(7), 0);}
#define FND_SEVEN {gpio_set_value(S5PV310_GPL2(2), 0);gpio_set_value(S5PV310_GPL2(5), 0);\
				gpio_set_value(S5PV310_GPL2(6), 0);gpio_set_value(S5PV310_GPL2(7), 0);}
#define FND_EIGHT {gpio_set_value(S5PV310_GPL2(1), 0);gpio_set_value(S5PV310_GPL2(2), 0);\
				gpio_set_value(S5PV310_GPL2(3), 0);gpio_set_value(S5PV310_GPL2(4), 0);\
				gpio_set_value(S5PV310_GPL2(5), 0);gpio_set_value(S5PV310_GPL2(6), 0);\
			     	gpio_set_value(S5PV310_GPL2(7), 0);}
#define FND_NINE {gpio_set_value(S5PV310_GPL2(1), 0);gpio_set_value(S5PV310_GPL2(2), 0);\
			     gpio_set_value(S5PV310_GPL2(4), 0);gpio_set_value(S5PV310_GPL2(5), 0);\
			     gpio_set_value(S5PV310_GPL2(6), 0);gpio_set_value(S5PV310_GPL2(7), 0);}
#define FND_ZERO {gpio_set_value(S5PV310_GPL2(2), 0);gpio_set_value(S5PV310_GPL2(3), 0);\
			     gpio_set_value(S5PV310_GPL2(4), 0);gpio_set_value(S5PV310_GPL2(5), 0);\
			     gpio_set_value(S5PV310_GPL2(6), 0);gpio_set_value(S5PV310_GPL2(7), 0);}

#define FND_CLEAR {gpio_set_value(S5PV310_GPL2(1), 1);gpio_set_value(S5PV310_GPL2(2), 1);\
				gpio_set_value(S5PV310_GPL2(3), 1);gpio_set_value(S5PV310_GPL2(4), 1);\
				gpio_set_value(S5PV310_GPL2(5), 1);gpio_set_value(S5PV310_GPL2(6), 1);\
				gpio_set_value(S5PV310_GPL2(7), 1);}
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
static void achro4210_poweroff_timer_run(void)
{
	init_timer(&achro4210_keypad.poweroff_timer);
	achro4210_keypad.poweroff_timer.function = achro4210_poweroff_timer_handler;

	switch(achro4210_keypad.poweroff_time)	{
		default	:
			achro4210_keypad.poweroff_time = 0;
		case	0:
			achro4210_keypad.poweroff_timer.expires = jiffies + PERIOD_1SEC;
			break;
		case	1:
			achro4210_keypad.poweroff_timer.expires = jiffies + PERIOD_3SEC;
			break;
		case	2:
			achro4210_keypad.poweroff_timer.expires = jiffies + PERIOD_5SEC;
			break;
	}
	add_timer(&achro4210_keypad.poweroff_timer);
}

//[*]--------------------------------------------------------------------------------------------------[*]
extern	void 	STATUS_LED_CONTROL	(int led, int val);		// achro4210-sysfs.c
extern	void 	SYSTEM_POWER_CONTROL(int power, int val);	// achro4210-sysfs.c

static void achro4210_poweroff_timer_handler(unsigned long data)
{
	// POWER OFF 
	achro4210_keypad.poweroff_flag	= TRUE;

	// PS_HOLD Enable (LOW-> HIGH)
	SYSTEM_POWER_CONTROL(0, 0);	// BUCK6 Disable
	SYSTEM_POWER_CONTROL(1, 0);	// P5V Disable
	SYSTEM_POWER_CONTROL(2, 1);	// P12V(VLED) Disable
	STATUS_LED_CONTROL(2, 0);	// BLUE LED OFF
	printk(KERN_EMERG "%s : setting GPIO_PDA_PS_HOLD low.\n", __func__);
	(*(unsigned long *)(S5PV310_VA_PMU + 0x330C)) = 0x5200;
}

#ifdef LED_FND_TEST
//jhkang: LED & FND test
/*
	D3: LED0 -> SPI_1.MOSI -> 85 (QTH5) -> GPB7 b11101111 = 0xEF
	D4: LED1 -> SPI_1.MISO -> 62 (QTH5) -> GPB6 b10111111 = 0xbf
	D5: LED2 -> SPI_1.NSS   -> 97 (QTH5) -> GPB5 b11011111 = 0xDF
	D6: LED3 -> SPI_1.CLK   -> 87 (QTH5) -> GPB4 b01111111 = 0x7F
*/
void led_port_select(unsigned short value)
{
	switch(value)
		{
		case 0:
		#ifdef LED_ADDRESS_ACCESS
			__raw_writel(0xEF, s3c_led_base + 0x44);
			mdelay(500);			
			__raw_writel(0xFF, s3c_led_base + 0x44);
		#else  			
			gpio_set_value(S5PV310_GPB(4), 0);
			mdelay(500);			
			gpio_set_value(S5PV310_GPB(4), 1);

			gpio_set_value(S5PV310_GPE4(0), 1);
			mdelay(500);			
			gpio_set_value(S5PV310_GPE4(0), 0);
		#endif
			break;

		case 1:
			#ifdef LED_ADDRESS_ACCESS
			__raw_writel(0xdF, s3c_led_base + 0x44);
			mdelay(500);			
			__raw_writel(0xFF, s3c_led_base + 0x44);			
			#else		
			gpio_set_value(S5PV310_GPB(5), 0);
			mdelay(500);			
			gpio_set_value(S5PV310_GPB(5), 1);	

			gpio_set_value(S5PV310_GPE3(6), 1);
			mdelay(500);			
			gpio_set_value(S5PV310_GPE3(6), 0);			
			#endif

			break;			
		
		case 2:
		#ifdef LED_ADDRESS_ACCESS
			__raw_writel(0xbF, s3c_led_base + 0x44);
			mdelay(500);			
			__raw_writel(0xFF, s3c_led_base + 0x44);
		#else 		
			gpio_set_value(S5PV310_GPB(6), 0);
			mdelay(500);			
			gpio_set_value(S5PV310_GPB(6), 1);		

			gpio_set_value(S5PV310_GPE3(3), 1);
			mdelay(500);			
			gpio_set_value(S5PV310_GPE3(3), 0);		
		#endif	
			break;
			

		case 3:
			#ifdef LED_ADDRESS_ACCESS
			__raw_writel(0xEF, s3c_led_base + 0x44);
			mdelay(500);			
			__raw_writel(0xFF, s3c_led_base + 0x44);			
			#else		
			gpio_set_value(S5PV310_GPB(7), 0);
			mdelay(500);			
			gpio_set_value(S5PV310_GPB(7), 1);	

			gpio_set_value(S5PV310_GPE3(6), 1);
			mdelay(500);			
			gpio_set_value(S5PV310_GPE3(6), 0);			
			#endif

			break;				
		}
}
void led_fnd_TEST(int key_value)
{
	static int cnt=0;
	int i=0;
	
	switch(key_value)
	{
		case KEY_SETUP:
			FND_ZERO mdelay(1000);	FND_CLEAR
			led_port_select(0);
			FND_ONE mdelay(1000);	FND_CLEAR
			led_port_select(1);
			FND_TWO mdelay(1000);	FND_CLEAR
			led_port_select(2);
			FND_THREE mdelay(1000);	FND_CLEAR
			led_port_select(3);
			FND_FOUR mdelay(1000);	FND_CLEAR
			led_port_select(0);
			FND_FIVE mdelay(1000);	FND_CLEAR
			led_port_select(1);
			FND_SIX mdelay(1000);	FND_CLEAR
			led_port_select(2);
			FND_SEVEN mdelay(1000);	FND_CLEAR
			led_port_select(3);
			FND_EIGHT mdelay(1000);	FND_CLEAR
			led_port_select(0);
			FND_NINE mdelay(1000);	FND_CLEAR
			led_port_select(1);
			break;
		case KEY_MENU: 
			break; 

		case KEY_BACK: 
			break;	

		case KEY_HOME: 
			break;
	}
}
#endif
//[*]--------------------------------------------------------------------------------------------------[*]
static void	generate_keycode(unsigned short prev_key, unsigned short cur_key, int *key_table)
{
	unsigned short 	press_key, release_key, i;
	
	press_key	= (cur_key ^ prev_key) & cur_key;
	release_key	= (cur_key ^ prev_key) & prev_key;
	
	i = 0;
	while(press_key)	{
		if(press_key & 0x01)	{
#ifdef LED_FND_TEST
			led_fnd_TEST(key_table[i]);//jhkang: LED & FND test
#endif			
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
			if(key_table[i] == KEY_POWER){
				del_timer(&achro4210_keypad.poweroff_timer);
				if(achro4210_keypad.poweroff_flag)
				{		
					printk(KERN_INFO "[%s:%d] power off\n", __FUNCTION__, __LINE__);
					POWER_OFF_ENABLE();
					achro4210_keypad.poweroff_flag = FALSE;	
				}
			}
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
			if(press_key & 0x01)
				printk("PRESS KEY : %s", (char *)&key_table[i * 20]);
			i++;
			press_key >>= 1;
		}
		
		i = 0;
		while(release_key)	{
			if(release_key & 0x01)
				printk("RELEASE KEY : %s", (char *)&key_table[i * 20]);
			i++;
			release_key >>= 1;
		}
	}
#endif

//[*]--------------------------------------------------------------------------------------------------[*]
static void	achro4210_keypad_port_init(void)
{
   //jhkang  
	if(gpio_request(S5PV310_GPX2(0),"GPX2.0"))	printk("error!!!\n");
	s3c_gpio_cfgpin(S5PV310_GPX2(0), S3C_GPIO_SFN(0));
	gpio_direction_input(S5PV310_GPX2(0));	s3c_gpio_setpull(S5PV310_GPX2(0), S3C_GPIO_PULL_NONE);

	if(gpio_request(S5PV310_GPX2(1),"GPX2.1"))	printk("error!!!\n");
	s3c_gpio_cfgpin(S5PV310_GPX2(1), S3C_GPIO_SFN(0));
	gpio_direction_input(S5PV310_GPX2(1));	s3c_gpio_setpull(S5PV310_GPX2(1), S3C_GPIO_PULL_DOWN);

	if(gpio_request(S5PV310_GPX2(2),"GPX2.2"))	printk("error!!!\n");
	s3c_gpio_cfgpin(S5PV310_GPX2(2), S3C_GPIO_SFN(0));
	gpio_direction_input(S5PV310_GPX2(2));	s3c_gpio_setpull(S5PV310_GPX2(2), S3C_GPIO_PULL_DOWN);

	if(gpio_request(S5PV310_GPX2(3),"GPX2.3"))	printk("error!!!\n");
	s3c_gpio_cfgpin(S5PV310_GPX2(3), S3C_GPIO_SFN(0));
	gpio_direction_input(S5PV310_GPX2(3));	s3c_gpio_setpull(S5PV310_GPX2(3), S3C_GPIO_PULL_DOWN);

	if(gpio_request(S5PV310_GPX2(4),"GPX2.4"))	printk("error!!!\n");
	s3c_gpio_cfgpin(S5PV310_GPX2(4), S3C_GPIO_SFN(0));
	gpio_direction_input(S5PV310_GPX2(4));	s3c_gpio_setpull(S5PV310_GPX2(4), S3C_GPIO_PULL_DOWN);

	if(gpio_request(S5PV310_GPX2(5),"GPX2.5"))	printk("error!!!\n");
	s3c_gpio_cfgpin(S5PV310_GPX2(5), S3C_GPIO_SFN(0));
	gpio_direction_input(S5PV310_GPX2(5));	s3c_gpio_setpull(S5PV310_GPX2(5), S3C_GPIO_PULL_DOWN);

	if(gpio_request(S5PV310_GPX0(1),"GPX0.1"))	printk("error!!!\n");
	s3c_gpio_cfgpin(S5PV310_GPX0(1), S3C_GPIO_SFN(0));
	gpio_direction_input(S5PV310_GPX0(1));	s3c_gpio_setpull(S5PV310_GPX0(1), S3C_GPIO_PULL_NONE);

}

//[*]--------------------------------------------------------------------------------------------------[*]
static int	achro4210_keypad_get_data(void)
{
	int		key_data = 0;
	
	key_data |= gpio_get_value(S5PV310_GPX2(0)) ? 0x00 : 0x01;
	key_data |= gpio_get_value(S5PV310_GPX2(1)) ? 0x00 : 0x04;
	key_data |= gpio_get_value(S5PV310_GPX2(2)) ? 0x00 : 0x02;

	key_data |= gpio_get_value(S5PV310_GPX2(3)) ? 0x08 : 0x00;
	key_data |= gpio_get_value(S5PV310_GPX2(5)) ? 0x10 : 0x00;
	key_data |= gpio_get_value(S5PV310_GPX0(1)) ? 0x00 : 0x20;
	key_data |= gpio_get_value(S5PV310_GPX2(4)) ? 0x00 : 0x40;

	return	key_data;
}

//[*]--------------------------------------------------------------------------------------------------[*]
#if defined(CONFIG_HDMI_HPD)
extern	int s5p_hpd_get_status(void);
#endif

static void achro4210_keypad_control(void)
{
	static	unsigned short	prev_keypad_data = 0, cur_keypad_data = 0;

#if defined(CONFIG_HDMI_HPD)
	static	unsigned char	chk_count = 0;
#endif
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
#if defined(CONFIG_HDMI_HPD)
	if(chk_count++ > 100)	{
		if(s5p_hpd_get_status())	input_report_switch(achro4210_keypad.driver, SW_LID, ON);
		else						input_report_switch(achro4210_keypad.driver, SW_LID, OFF);
		chk_count = 0;
	}
#endif
}

//[*]--------------------------------------------------------------------------------------------------[*]
static void	achro4210_rd_timer_handler(unsigned long data)
{
    unsigned long flags;

    local_irq_save(flags);

	if(achro4210_keypad.wakeup_delay > KEYPAD_WAKEUP_DELAY)	achro4210_keypad_control();	
	else													achro4210_keypad.wakeup_delay++;
		
	// Kernel Timer restart
	switch(achro4210_keypad.sampling_rate)	{
		default	:
			achro4210_keypad.sampling_rate = 0;
		case	0:
			mod_timer(&achro4210_keypad.rd_timer,jiffies + PERIOD_10MS);
			break;
		case	1:
			mod_timer(&achro4210_keypad.rd_timer,jiffies + PERIOD_20MS);
			break;
		case	2:
			mod_timer(&achro4210_keypad.rd_timer,jiffies + PERIOD_50MS);
			break;
	}

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
	
	// Wakeup dumy key send
	input_report_key(achro4210_keypad.driver, KEY_HOME, KEY_PRESS);					
	input_report_key(achro4210_keypad.driver, KEY_HOME, KEY_RELEASE);					
	input_sync(achro4210_keypad.driver);
	
	achro4210_keypad.wakeup_delay = 0;

    return  0;
}

//[*]--------------------------------------------------------------------------------------------------[*]
static int	achro4210_keypad_suspend(struct platform_device *dev, pm_message_t state)
{
	#if	defined(DEBUG_PM_MSG)
		printk("%s\n", __FUNCTION__);
	#endif

	del_timer(&achro4210_keypad.rd_timer);
	
    return  0;
}

//[*]--------------------------------------------------------------------------------------------------[*]
static void	achro4210_keypad_config(unsigned char state)
{
	if(state == KEYPAD_STATE_BOOT)	{
		achro4210_keypad_port_init();	// GPH2, GPH3 All Input, All Pull up
	}

	/* Scan timer init */
	init_timer(&achro4210_keypad.rd_timer);

	achro4210_keypad.rd_timer.function = achro4210_rd_timer_handler;
	achro4210_keypad.rd_timer.expires = jiffies + (HZ/100);

	add_timer(&achro4210_keypad.rd_timer);
}

//[*]--------------------------------------------------------------------------------------------------[*]

//jhkang: LED setting
/*
	D3: LED0 -> SPI_1.MOSI -> 85 (QTH5) -> GPB7
	D4: LED1 -> SPI_1.MISO -> 62 (QTH5) -> GPB6
	D5: LED2 -> SPI_1.NSS   -> 97 (QTH5) -> GPB5
	D6: LED3 -> SPI_1.CLK   -> 87 (QTH5) -> GPB4
	STATUS_RED_nLED		-> LCD_B_VD20 	     -> 120 (QTH3) ->GPE3_6
	STATUS_GREEN_nLED	-> LCD_B_VD17(TP48) -> 112 (QTH4) ->GPE3_3
	STATUS_BLUE_nLED		-> LCD_B_VD22(TP58) -> 116 (QTH4) ->GPE3_7
*/
void led_port_init(void)
{
	
	if(gpio_request(S5PV310_GPB(4), "GPB4"))
		printk(KERN_INFO "[%s:%d] fail GPB(4)\n", __FUNCTION__, __LINE__);
	s3c_gpio_setpull(S5PV310_GPB(4), S3C_GPIO_PULL_NONE);
	gpio_direction_output(S5PV310_GPB(4), 1); //stnby

	if(gpio_request(S5PV310_GPB(5), "GPB5"))
		printk(KERN_INFO "[%s:%d] fail GPB(5)\n", __FUNCTION__, __LINE__);
	s3c_gpio_setpull(S5PV310_GPB(5), S3C_GPIO_PULL_NONE);
	gpio_direction_output(S5PV310_GPB(5), 1); //stnby

	if(gpio_request(S5PV310_GPB(6), "GPB6"))
		printk(KERN_INFO "[%s:%d] fail GPB(6)\n", __FUNCTION__, __LINE__);
	s3c_gpio_setpull(S5PV310_GPB(6), S3C_GPIO_PULL_NONE);
	gpio_direction_output(S5PV310_GPB(6), 1); //stnby

	if(gpio_request(S5PV310_GPB(7), "GPB7"))
		printk(KERN_INFO "[%s:%d] fail GPB(7)\n", __FUNCTION__, __LINE__);
	s3c_gpio_setpull(S5PV310_GPB(7), S3C_GPIO_PULL_NONE);
	gpio_direction_output(S5PV310_GPB(7), 1); //stnby

	if(gpio_request(S5PV310_GPE3(3), "GPE3.3"))
		printk(KERN_INFO "[%s:%d] fail GPE3(3)\n", __FUNCTION__, __LINE__);
	s3c_gpio_setpull(S5PV310_GPE3(3), S3C_GPIO_PULL_NONE);
	gpio_direction_output(S5PV310_GPE3(3), 0); //stnby

	if(gpio_request(S5PV310_GPE3(6), "GPE3.6"))
		printk(KERN_INFO "[%s:%d] fail GPE3(6)\n", __FUNCTION__, __LINE__);
	s3c_gpio_setpull(S5PV310_GPE3(6), S3C_GPIO_PULL_NONE);
	gpio_direction_output(S5PV310_GPE3(6), 0); //stnby

	if(gpio_request(S5PV310_GPE4(0), "GPE4.0"))
		printk(KERN_INFO "[%s:%d] fail GPE4(0)\n", __FUNCTION__, __LINE__);
	s3c_gpio_setpull(S5PV310_GPE4(0), S3C_GPIO_PULL_NONE);
	gpio_direction_output(S5PV310_GPE4(0), 0); //stnby

#ifdef LED_ADDRESS_ACCESS
	s3c_led_base = ioremap(0x11400000, 0x2);
	printk("s3c_rtc_base: %x\n", s3c_led_base);
	if (s3c_led_base == NULL) {
		printk("Error: no memory mapping for rtc\n");
	}
#endif
	
}

/*
	A: FDN_A    -> FDATA0 -> GPS_GPIO_7 -> GPL2_7
	B: FDN_B    -> FDATA1 -> GPS_GPIO_6 -> GPL2_6 
	C: FDN_C    -> FDATA2 -> GPS_GPIO_5 -> GPL2_5
	D: FDN_D    -> FDATA3 -> GPS_GPIO_4 -> GPL2_4 
	E: FDN_E    -> FDATA4 -> GPS_GPIO_3 -> GPL2_3 
	F: FDN_F    -> FDATA5 -> GPS_GPIO_2 -> GPL2_2 
	G: FDN_G    -> FDATA6 -> GPS_GPIO_1 -> GPL2_1 
	DP: FND_DP  -> FDATA7 -> GPS_GPIO_0 -> GPL2_0
*/
void fnd_port_init(void)
{
	if(gpio_request(S5PV310_GPL2(0), "GPL2_0"))
		printk(KERN_INFO "[%s:%d] fail GPL(2).0\n", __FUNCTION__, __LINE__);
	s3c_gpio_setpull(S5PV310_GPL2(0), S3C_GPIO_PULL_NONE);
	gpio_direction_output(S5PV310_GPL2(0), 1); //stnby

	if(gpio_request(S5PV310_GPL2(1), "GPL2_1"))
		printk(KERN_INFO "[%s:%d] fail GPL(2).1\n", __FUNCTION__, __LINE__);
	s3c_gpio_setpull(S5PV310_GPL2(1), S3C_GPIO_PULL_NONE);
	gpio_direction_output(S5PV310_GPL2(1), 1); //stnby

	if(gpio_request(S5PV310_GPL2(2), "GPL2_2"))
		printk(KERN_INFO "[%s:%d] fail GPL(2).2\n", __FUNCTION__, __LINE__);
	s3c_gpio_setpull(S5PV310_GPL2(2), S3C_GPIO_PULL_NONE);
	gpio_direction_output(S5PV310_GPL2(2), 1); //stnby

	if(gpio_request(S5PV310_GPL2(3), "GPL2_3"))
		printk(KERN_INFO "[%s:%d] fail GPL(2).3\n", __FUNCTION__, __LINE__);
	s3c_gpio_setpull(S5PV310_GPL2(3), S3C_GPIO_PULL_NONE);
	gpio_direction_output(S5PV310_GPL2(3), 1); //stnby

	if(gpio_request(S5PV310_GPL2(4), "GPL2_4"))
		printk(KERN_INFO "[%s:%d] fail GPL(2).4\n", __FUNCTION__, __LINE__);
	s3c_gpio_setpull(S5PV310_GPL2(4), S3C_GPIO_PULL_NONE);
	gpio_direction_output(S5PV310_GPL2(4), 1); //stnby

	if(gpio_request(S5PV310_GPL2(5), "GPL2_5"))
		printk(KERN_INFO "[%s:%d] fail GPL(2).5\n", __FUNCTION__, __LINE__);
	s3c_gpio_setpull(S5PV310_GPL2(5), S3C_GPIO_PULL_NONE);
	gpio_direction_output(S5PV310_GPL2(5), 1); //stnby

	if(gpio_request(S5PV310_GPL2(6), "GPL2_6"))
		printk(KERN_INFO "[%s:%d] fail GPL(2).6\n", __FUNCTION__, __LINE__);
	s3c_gpio_setpull(S5PV310_GPL2(6), S3C_GPIO_PULL_NONE);
	gpio_direction_output(S5PV310_GPL2(6), 1); //stnby

	if(gpio_request(S5PV310_GPL2(7), "GPL2_7"))
		printk(KERN_INFO "[%s:%d] fail GPL(2).7\n", __FUNCTION__, __LINE__);
	s3c_gpio_setpull(S5PV310_GPL2(7), S3C_GPIO_PULL_NONE);
	gpio_direction_output(S5PV310_GPL2(7), 1); //stnby

}
static int __devinit    achro4210_keypad_probe(struct platform_device *pdev)
{
    int 	key, code, rc;

	led_port_init();//jhkang: LED setting
	fnd_port_init();
	
	// struct init	
	memset(&achro4210_keypad, 0x00, sizeof(achro4210_keypad_t));
	
	// create sys_fs
	if((rc = achro4210_keypad_sysfs_create(pdev)))	{
		printk("%s : sysfs_create_group fail!!\n", __FUNCTION__);
		return	rc;
	}

	achro4210_keypad.driver = input_allocate_device();
	
    if(!(achro4210_keypad.driver))	{
		printk("ERROR! : %s input_allocate_device() error!!! no memory!!\n", __FUNCTION__);
		achro4210_keypad_sysfs_remove(pdev);
		return -ENOMEM;
    }

	set_bit(EV_KEY, achro4210_keypad.driver->evbit);
//	set_bit(EV_REP, achro4210_keypad.driver->evbit);	// Repeat Key

	set_bit(EV_SW, achro4210_keypad.driver->evbit);
	set_bit(SW_LID & SW_MAX, achro4210_keypad.driver->swbit);

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
		printk("HardKernel-C100 keypad input register device fail!!\n");

		achro4210_keypad_sysfs_remove(pdev);
		input_free_device(achro4210_keypad.driver);	return	-ENODEV;
	}

	achro4210_keypad_config(KEYPAD_STATE_BOOT);

	printk("Huins : ACHRO4210 Keypad driver initialized!! Ver 1.0\n");

    return 0;
}

//[*]--------------------------------------------------------------------------------------------------[*]
static int __devexit    achro4210_keypad_remove(struct platform_device *pdev)
{
	achro4210_keypad_sysfs_remove(pdev);

	input_unregister_device(achro4210_keypad.driver);
	
	del_timer(&achro4210_keypad.rd_timer);
	
	// Free GPIOs
	gpio_free(S5PV310_GPX2(0));
	gpio_free(S5PV310_GPX2(1));
	gpio_free(S5PV310_GPX2(2));

	gpio_free(S5PV310_GPX2(3));
	gpio_free(S5PV310_GPX2(5));
	gpio_free(S5PV310_GPX0(1));

#ifdef LED_ADDRESS_ACCESS
	iounmap(s3c_led_base);//free LED
#endif	
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
