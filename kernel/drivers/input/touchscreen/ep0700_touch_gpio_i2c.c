//[*]----------------------------------------------------------------------------------------------[*]
//
//
// 
//  HardKernel-C1XX gpio i2c driver (charles.park)
//  2009.07.22
// 
//
//[*]----------------------------------------------------------------------------------------------[*]
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/input.h>
#include <linux/fs.h>

#include <mach/irqs.h>
#include <asm/system.h>

#include <linux/delay.h>

#include <asm/gpio.h>
#include <plat/gpio-cfg.h>
#include <mach/regs-gpio.h>
#include <mach/gpio.h>

//[*]----------------------------------------------------------------------------------------------[*]
#include "ep0700_touch_gpio_i2c.h"
#include "achro4210_touch.h"

//[*]----------------------------------------------------------------------------------------------[*]
// Touch I2C Address Define (MCP)
#define	TOUCH_WR_ADDR			0x0A
#define	TOUCH_RD_ADDR				0x0B


//#define	TOUCH_BOOTMODE_WR_ADDR		0x40
//#define	TOUCH_BOOTMODE_RD_ADDR		0x41

//#define use_lcd_sync_port

#ifdef use_lcd_sync_port  //temporary, hsync, vsync

#if 0
#define S5PV310_GPF0CON			(__phys_to_virt(S5PV310_PA_GPIO) + 0x0180)
#define S5PV310_GPF0DAT			(__phys_to_virt(S5PV310_PA_GPIO) + 0x0184)

// Touch I2C Port define
#define	GPIO_I2C_SDA_CON_PORT	(*(unsigned long *)S5PV310_GPF0CON)
#define	GPIO_I2C_SDA_DAT_PORT	(*(unsigned long *)S5PV310_GPF0DAT)
#define	GPIO_SDA_PIN			1

#define	GPIO_I2C_CLK_CON_PORT	(*(unsigned long *)S5PV310_GPF0CON)
#define	GPIO_I2C_CLK_DAT_PORT	(*(unsigned long *)S5PV310_GPF0DAT)
#define	GPIO_CLK_PIN			0
#endif


#else

#define S5PV310_GPL1CON			(S5PV310_VA_GPIO2 + 0xE0)
#define S5PV310_GPL1DAT			(S5PV310_VA_GPIO2 + 0xE4)

// Touch I2C Port define
#define	GPIO_I2C_SDA_CON_PORT	(*(unsigned long *)S5PV310_GPL1CON)
#define	GPIO_I2C_SDA_DAT_PORT	(*(unsigned long *)S5PV310_GPL1DAT)
#define	GPIO_SDA_PIN			1

#define	GPIO_I2C_CLK_CON_PORT	(*(unsigned long *)S5PV310_GPL1CON)
#define	GPIO_I2C_CLK_DAT_PORT	(*(unsigned long *)S5PV310_GPL1DAT)
#define	GPIO_CLK_PIN			0

#endif

#define	DELAY_TIME				5	// us value
#define	PORT_CHANGE_DELAY_TIME	5
//#define	DELAY_TIME				10	// us value
//#define	PORT_CHANGE_DELAY_TIME	10

//[*]----------------------------------------------------------------------------------------------[*]
#define	GPIO_CON_PORT_MASK		0xF
#define	GPIO_CON_PORT_OFFSET		0x4

#define	GPIO_CON_INPUT			0x0
#define	GPIO_CON_OUTPUT			0x1

//[*]----------------------------------------------------------------------------------------------[*]
#define	HIGH						1
#define	LOW						0

//#define	DEBUG_GPIO_I2C		// debug enable flag
#define	DEBUG_MSG(x)			printk(x)

//[*]----------------------------------------------------------------------------------------------[*]
//	static function prototype
//[*]----------------------------------------------------------------------------------------------[*]
static	void			gpio_i2c_sda_port_control	(unsigned char inout);
static	void			gpio_i2c_clk_port_control	(unsigned char inout);

static	unsigned char	gpio_i2c_get_sda			(void);
static	void			gpio_i2c_set_sda			(unsigned char hi_lo);
static	void			gpio_i2c_set_clk			(unsigned char hi_lo);
                                        	
static 	void			gpio_i2c_start				(void);
static 	void			gpio_i2c_stop				(void);
                                        	
static 	void			gpio_i2c_send_ack			(void);
static 	void			gpio_i2c_send_noack			(void);
static 	unsigned char	gpio_i2c_chk_ack			(void);
                		                      	
static 	void 		gpio_i2c_byte_write			(unsigned char wdata);
static 	void			gpio_i2c_byte_read			(unsigned char *rdata);
		        		

//[*]----------------------------------------------------------------------------------------------[*]
static	void			gpio_i2c_sda_port_control	(unsigned char inout)
{
#ifdef use_lcd_sync_port
	gpio_direction_input(S5PV310_GPF0(1));
#else
	GPIO_I2C_SDA_CON_PORT &=  (unsigned long)(~(GPIO_CON_PORT_MASK << (GPIO_SDA_PIN * GPIO_CON_PORT_OFFSET)));
	GPIO_I2C_SDA_CON_PORT |=  (unsigned long)( (inout << (GPIO_SDA_PIN * GPIO_CON_PORT_OFFSET)));
#endif
}

//[*]----------------------------------------------------------------------------------------------[*]
static	void			gpio_i2c_clk_port_control	(unsigned char inout)
{
#ifndef  use_lcd_sync_port
	GPIO_I2C_CLK_CON_PORT &=  (unsigned long)(~(GPIO_CON_PORT_MASK << (GPIO_CLK_PIN * GPIO_CON_PORT_OFFSET)));
	GPIO_I2C_CLK_CON_PORT |=  (unsigned long)( (inout << (GPIO_CLK_PIN * GPIO_CON_PORT_OFFSET)));
#endif	
}

//[*]----------------------------------------------------------------------------------------------[*]
static	unsigned char	gpio_i2c_get_sda		(void)
{
#ifdef use_lcd_sync_port
	return	gpio_get_value(S5PV310_GPF0(1)) ? 1 : 0;
#else
	return	GPIO_I2C_SDA_DAT_PORT & (HIGH << GPIO_SDA_PIN) ? 1 : 0;
#endif
}

//[*]----------------------------------------------------------------------------------------------[*]
static	void			gpio_i2c_set_sda		(unsigned char hi_lo)
{
#ifdef use_lcd_sync_port
	if(hi_lo)	
		gpio_direction_output(S5PV310_GPF0(1), 1);
	else
		gpio_direction_output(S5PV310_GPF0(1), 0);
	udelay(PORT_CHANGE_DELAY_TIME);
#else
	if(hi_lo)	{
		GPIO_I2C_SDA_DAT_PORT |= (HIGH << GPIO_SDA_PIN);
		gpio_i2c_sda_port_control(GPIO_CON_OUTPUT);
		udelay(PORT_CHANGE_DELAY_TIME);
	}
	else		{
		GPIO_I2C_SDA_DAT_PORT &= ~(HIGH << GPIO_SDA_PIN);
		gpio_i2c_sda_port_control(GPIO_CON_OUTPUT);
		udelay(PORT_CHANGE_DELAY_TIME);
	}	
#endif	
}

//[*]----------------------------------------------------------------------------------------------[*]
static	void			gpio_i2c_set_clk		(unsigned char hi_lo)
{
#ifdef use_lcd_sync_port
	if(hi_lo)
		gpio_direction_output(S5PV310_GPF0(0), 1);
	else
		gpio_direction_output(S5PV310_GPF0(0), 0);
	udelay(PORT_CHANGE_DELAY_TIME);
#else
	if(hi_lo)	{
		GPIO_I2C_CLK_DAT_PORT |= (HIGH << GPIO_CLK_PIN);
		gpio_i2c_clk_port_control(GPIO_CON_OUTPUT);
		udelay(PORT_CHANGE_DELAY_TIME);
	}
	else		{
		GPIO_I2C_CLK_DAT_PORT &= ~(HIGH << GPIO_CLK_PIN);
		gpio_i2c_clk_port_control(GPIO_CON_OUTPUT);
		udelay(PORT_CHANGE_DELAY_TIME);
	}	
#endif	
}

//[*]----------------------------------------------------------------------------------------------[*]
static	void			gpio_i2c_start			(void)
{
	// Setup SDA, CLK output High
	gpio_i2c_set_sda(HIGH);
	gpio_i2c_set_clk(HIGH);
	
	udelay(DELAY_TIME);

	// SDA low before CLK low
	gpio_i2c_set_sda(LOW);	udelay(DELAY_TIME);
	gpio_i2c_set_clk(LOW);	udelay(DELAY_TIME);
}

//[*]----------------------------------------------------------------------------------------------[*]
static	void			gpio_i2c_stop			(void)
{
	// Setup SDA, CLK output low
	gpio_i2c_set_sda(LOW);
	gpio_i2c_set_clk(LOW);
	
	udelay(DELAY_TIME);
	
	// SDA high after CLK high
	gpio_i2c_set_clk(HIGH);	udelay(DELAY_TIME);
	gpio_i2c_set_sda(HIGH);	udelay(DELAY_TIME);
}

//[*]----------------------------------------------------------------------------------------------[*]
static	void			gpio_i2c_send_ack		(void)
{
	// SDA Low
	gpio_i2c_set_sda(LOW);	udelay(DELAY_TIME);
	gpio_i2c_set_clk(HIGH);	udelay(DELAY_TIME);
	gpio_i2c_set_clk(LOW);	udelay(DELAY_TIME);
}

//[*]----------------------------------------------------------------------------------------------[*]
static	void			gpio_i2c_send_noack		(void)
{
	// SDA High
	gpio_i2c_set_sda(HIGH);	udelay(DELAY_TIME);
	gpio_i2c_set_clk(HIGH);	udelay(DELAY_TIME);
	gpio_i2c_set_clk(LOW);	udelay(DELAY_TIME);
}

//[*]----------------------------------------------------------------------------------------------[*]
static	unsigned char	gpio_i2c_chk_ack		(void)
{
	unsigned char	count = 0, ret = 0;

//	gpio_i2c_set_sda(LOW);		udelay(DELAY_TIME);
	//gpio_i2c_set_clk(HIGH);		udelay(DELAY_TIME);

	gpio_i2c_sda_port_control(GPIO_CON_INPUT);
	udelay(PORT_CHANGE_DELAY_TIME);

	gpio_i2c_set_clk(HIGH);//		udelay(DELAY_TIME);	

	while(gpio_i2c_get_sda())	{
		if(count++ > 100)	{	ret = 1;	break;	}
		else					udelay(DELAY_TIME);	
	}

	gpio_i2c_set_clk(LOW);		udelay(DELAY_TIME);
	
//	#if defined(DEBUG_GPIO_I2C)
//		if(ret)		printk("%s (%d): no ack!!\n",__FUNCTION__, ret);
//		else		printk("%s (%d): ack !! \n" ,__FUNCTION__, ret);
//	#endif

	return	ret;
}

//[*]----------------------------------------------------------------------------------------------[*]
static	void 		gpio_i2c_byte_write		(unsigned char wdata)
{
	unsigned char	cnt, mask;
	
	for(cnt = 0, mask = 0x80; cnt < 8; cnt++, mask >>= 1)	{
		if(wdata & mask)		gpio_i2c_set_sda(HIGH);
		else					gpio_i2c_set_sda(LOW);

		udelay(DELAY_TIME);
		gpio_i2c_set_clk(HIGH);		udelay(DELAY_TIME);
		gpio_i2c_set_clk(LOW);		udelay(DELAY_TIME);
	}
}

//[*]----------------------------------------------------------------------------------------------[*]
static	void		gpio_i2c_byte_read		(unsigned char *rdata)
{
	unsigned char	cnt, mask;

	gpio_i2c_sda_port_control(GPIO_CON_INPUT);
	udelay(PORT_CHANGE_DELAY_TIME);

	for(cnt = 0, mask = 0x80, *rdata = 0; cnt < 8; cnt++, mask >>= 1)	{
		gpio_i2c_set_clk(HIGH);		udelay(DELAY_TIME);
		
		if(gpio_i2c_get_sda())		*rdata |= mask;
		
		gpio_i2c_set_clk(LOW);		udelay(DELAY_TIME);
		
	}
}


//[*]----------------------------------------------------------------------------------------------[*]
int 				achro4210_touch_write		(unsigned char addr, unsigned char *wdata, unsigned char wsize)
{
	unsigned char cnt, ack;

	// start
	gpio_i2c_start();
	
	gpio_i2c_byte_write(TOUCH_WR_ADDR);	// i2c address

	if((ack = gpio_i2c_chk_ack()))	{
//		#if defined(DEBUG_GPIO_I2C)
			printk("%s [write address] : no ack\n",__FUNCTION__);
//		#endif

		goto	write_stop;
	}
	
	gpio_i2c_byte_write(addr);	// register
	
	if((ack = gpio_i2c_chk_ack()))	{
//		#if defined(DEBUG_GPIO_I2C)
			printk("%s [write register] : no ack\n",__FUNCTION__);
//		#endif
	}
	
	if(wsize)	{
		for(cnt = 0; cnt < wsize; cnt++)	{
			gpio_i2c_byte_write(wdata[cnt]);

			if((ack = gpio_i2c_chk_ack()))	{
//				#if defined(DEBUG_GPIO_I2C)
					printk("%s [write data] : no ack\n",__FUNCTION__);
//				#endif

				goto	write_stop;
			}
		}
	}

write_stop:
	
	gpio_i2c_stop();

//	if(ack)	printk("%s : error ack(%d)\n", __FUNCTION__, ack);
	
//	#if defined(DEBUG_GPIO_I2C)
//		printk("%s : %d\n", __FUNCTION__, ack);
//	#endif
	return	ack;
}


//[*]----------------------------------------------------------------------------------------------[*]
int 				achro4210_touch_read		(unsigned char addr, unsigned char *rdata, unsigned char rsize)
{
	unsigned char ack, cnt;


	// start
	gpio_i2c_start();
	
	gpio_i2c_byte_write(TOUCH_WR_ADDR);	// i2c address

	if((ack = gpio_i2c_chk_ack()))	{
//		#if defined(DEBUG_GPIO_I2C)
			printk("%s [write address] : no ack\n",__FUNCTION__);
//		#endif

		goto	read_stop;
	}
	
	gpio_i2c_byte_write(addr);	// register
	
	if((ack = gpio_i2c_chk_ack()))	{
//		#if defined(DEBUG_GPIO_I2C)
			printk("%s [write register] : no ack\n",__FUNCTION__);
//		#endif
	}
	
	gpio_i2c_stop();

	






	// start
	gpio_i2c_start();

	gpio_i2c_byte_write(TOUCH_RD_ADDR);	// i2c address

	if((ack = gpio_i2c_chk_ack()))	{
//		#if defined(DEBUG_GPIO_I2C)
			printk("%s [write address] : no ack\n",__FUNCTION__);
//		#endif

		goto	read_stop;
	}
	
	for(cnt=0; cnt < rsize; cnt++)	{
		
		gpio_i2c_byte_read(&rdata[cnt]);
		
		if(cnt == rsize -1)		gpio_i2c_send_noack();
		else					gpio_i2c_send_ack();
	}
	
read_stop:
	gpio_i2c_stop();

//	if(ack)	printk("%s : error ack(%d)\n", __FUNCTION__, ack);

//	#if defined(DEBUG_GPIO_I2C)
//		printk("%s : %d\n", __FUNCTION__, ack);
//	#endif
	return	ack;
}


#if 0
//[*]----------------------------------------------------------------------------------------------[*]
int 				achro4210_touch_bootmode_write		(unsigned char addr, unsigned char *wdata, unsigned char wsize)
{
	unsigned char cnt, ack;

	// start
	gpio_i2c_start();
	
	gpio_i2c_byte_write(TOUCH_BOOTMODE_WR_ADDR);	// i2c address

	if((ack = gpio_i2c_chk_ack()))	{
		#if defined(DEBUG_GPIO_I2C)
			DEBUG_MSG(("%s [write address] : no ack\n",__FUNCTION__));
		#endif

		goto	write_stop;
	}
	
	gpio_i2c_byte_write(addr);	// register
	
	if((ack = gpio_i2c_chk_ack()))	{
		#if defined(DEBUG_GPIO_I2C)
			DEBUG_MSG(("%s [write register] : no ack\n",__FUNCTION__));
		#endif
	}
	
	if(wsize)	{
		for(cnt = 0; cnt < wsize; cnt++)	{
			gpio_i2c_byte_write(wdata[cnt]);

			if((ack = gpio_i2c_chk_ack()))	{
				#if defined(DEBUG_GPIO_I2C)
					DEBUG_MSG(("%s [write register] : no ack\n",__FUNCTION__));
				#endif

				goto	write_stop;
			}
		}
	}

write_stop:
	
	gpio_i2c_stop();

	#if defined(DEBUG_GPIO_I2C)
		DEBUG_MSG(("%s : %d\n", __FUNCTION__, ack));
	#endif
	return	ack;
}

//[*]----------------------------------------------------------------------------------------------[*]
int 				achro4210_touch_bootmode_read		(unsigned char *rdata, unsigned char rsize)
{
	unsigned char ack, cnt;

	// start
	gpio_i2c_start();

	gpio_i2c_byte_write(TOUCH_BOOTMODE_RD_ADDR);	// i2c address

	if((ack = gpio_i2c_chk_ack()))	{
		#if defined(DEBUG_GPIO_I2C)
			DEBUG_MSG(("%s [write address] : no ack\n",__FUNCTION__));
		#endif

		goto	read_stop;
	}
	
	for(cnt=0; cnt < rsize; cnt++)	{
		
		gpio_i2c_byte_read(&rdata[cnt]);
		
		if(cnt == rsize -1)		gpio_i2c_send_noack();
		else					gpio_i2c_send_ack();
	}
	
read_stop:
	gpio_i2c_stop();

	#if defined(DEBUG_GPIO_I2C)
		DEBUG_MSG(("%s : %d\n", __FUNCTION__, ack));
	#endif
	return	ack;
}
#endif

//[*]----------------------------------------------------------------------------------------------[*]
//[*]----------------------------------------------------------------------------------------------[*]
void achro4210_touch_port_init	(void)
{
	int err;

#ifdef	use_lcd_sync_port
	err = gpio_request(S5PV310_GPF0(0), "TOUCH_SCL");
	if (err) {
		printk(KERN_ERR "Touch SCL(GPF0.0)\n");
//		return err;
	}

	err = gpio_request(S5PV310_GPF0(1), "TOUCH_SDA");
	if (err) {
		printk(KERN_ERR "Touch SDA(GPF0.1)\n");
//		return err;
	}
#endif


	gpio_i2c_set_sda(HIGH);		
	gpio_i2c_set_clk(HIGH);
}
//[*]----------------------------------------------------------------------------------------------[*]
//[*]----------------------------------------------------------------------------------------------[*]


