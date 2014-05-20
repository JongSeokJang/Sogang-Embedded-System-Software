/* linux/arch/arm/mach-s5pv310/mach-smdkc210.c
 *
 * Copyright (c) 2010 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include <linux/platform_device.h>
#include <linux/serial_core.h>
#include <linux/delay.h>
#include <linux/usb/ch9.h>
#include <linux/i2c.h>
#include <linux/gpio.h>
#include <linux/spi/spi.h>
#include <linux/spi/spi_bitbang.h>
#include <linux/spi/spi_gpio.h>
#include <linux/mmc/host.h>
#include <linux/smsc911x.h>
#include <linux/io.h>
#include <linux/clk.h>
#if defined(CONFIG_S5P_MEM_CMA)
#include <linux/cma.h>
#endif
#include <linux/regulator/machine.h>
#ifdef CONFIG_ANDROID_PMEM
#include <linux/android_pmem.h>
#endif

#include <mach/regs-srom.h>
#include <asm/pmu.h>
#include <asm/mach/arch.h>
#include <asm/mach-types.h>
#include <plat/regs-serial.h>
#include <plat/s5pv310.h>
#include <plat/cpu.h>
#include <plat/fb.h>
#include <plat/iic.h>
#include <plat/devs.h>
#include <plat/adc.h>
#include <plat/ts.h>
#include <plat/fimg2d.h>
#include <plat/sdhci.h>
#include <plat/mshci.h>
#include <plat/regs-otg.h>
#include <plat/pm.h>
#include <plat/tvout.h>

#include <plat/csis.h>
#include <plat/fimc.h>
#include <plat/media.h>

#include <plat/s3c64xx-spi.h>
#include <plat/gpio-cfg.h>
#include <plat/pd.h>

#include <media/s5k3ba_platform.h>
#include <media/s5k4ba_platform.h>
#include <media/s5k4ea_platform.h>
#include <media/s5k6aa_platform.h>
#include <media/s5k5cagx_platform.h>
#include <media/s5k5aafa_platform.h>
#include <media/s5k4eagx_platform.h>
#include <media/s5k4ca_platform.h>

#include <mach/regs-gpio.h>

#include <mach/map.h>
#include <mach/regs-mem.h>
#include <mach/regs-clock.h>
#include <mach/media.h>
#include <mach/gpio.h>

#include <mach/spi-clocks.h>


#if defined(CONFIG_REGULATOR_MAX8997)
extern struct max8997_platform_data max8997_pdata;
#endif

#ifdef CONFIG_MPU_SENSORS_MPU3050
#include <linux/mpu.h>
#endif

#if defined(CONFIG_BACKLIGHT_PWM)	// CHARLES
	#include <linux/pwm_backlight.h>
#endif

#if defined(CONFIG_DEV_THERMAL)
#include <plat/s5p-tmu.h>
#include <mach/regs-tmu.h>
#endif

#if defined(CONFIG_ACHRO_HEADSET)
#include <mach/achro_jack.h>
#endif


#if defined(CONFIG_SPI_S5PV310)
	#include <plat/spi.h>
#endif

/* Following are default values for UCON, ULCON and UFCON UART registers */
#define SMDKC210_UCON_DEFAULT	(S3C2410_UCON_TXILEVEL |	\
				 S3C2410_UCON_RXILEVEL |	\
				 S3C2410_UCON_TXIRQMODE |	\
				 S3C2410_UCON_RXIRQMODE |	\
				 S3C2410_UCON_RXFIFO_TOI |	\
				 S3C2443_UCON_RXERR_IRQEN)

#define SMDKC210_ULCON_DEFAULT	S3C2410_LCON_CS8

#define SMDKC210_UFCON_DEFAULT	(S3C2410_UFCON_FIFOMODE |	\
				 S5PV210_UFCON_TXTRIG4 |	\
				 S5PV210_UFCON_RXTRIG4)
/*[------------------------------------------------------------------------------------------------------]*/
static struct s3c2410_uartcfg smdkc210_uartcfgs[] __initdata = {
	[0] = {
		.hwport		= 0,
		.flags		= 0,
		.ucon		= SMDKC210_UCON_DEFAULT,
		.ulcon		= SMDKC210_ULCON_DEFAULT,
		.ufcon		= SMDKC210_UFCON_DEFAULT,
	},
	[1] = {
		.hwport		= 1,
		.flags		= 0,
		.ucon		= SMDKC210_UCON_DEFAULT,
		.ulcon		= SMDKC210_ULCON_DEFAULT,
		.ufcon		= SMDKC210_UFCON_DEFAULT,
	},
	[2] = {
		.hwport		= 2,
		.flags		= 0,
		.ucon		= SMDKC210_UCON_DEFAULT,
		.ulcon		= SMDKC210_ULCON_DEFAULT,
		.ufcon		= SMDKC210_UFCON_DEFAULT,
	},
	[3] = {
		.hwport		= 3,
		.flags		= 0,
		.ucon		= SMDKC210_UCON_DEFAULT,
		.ulcon		= SMDKC210_ULCON_DEFAULT,
		.ufcon		= SMDKC210_UFCON_DEFAULT,
	},
	[4] = {
		.hwport		= 4,
		.flags		= 0,
		.ucon		= SMDKC210_UCON_DEFAULT,
		.ulcon		= SMDKC210_ULCON_DEFAULT,
		.ufcon		= SMDKC210_UFCON_DEFAULT,
	},
};
/*[------------------------------------------------------------------------------------------------------]*/
#undef WRITEBACK_ENABLED
/*[------------------------------------------------------------------------------------------------------]*/
#ifdef CONFIG_VIDEO_FIMC
/*
 * External camera reset
 * Because the most of cameras take i2c bus signal, so that
 * you have to reset at the boot time for other i2c slave devices.
 * This function also called at fimc_init_camera()
 * Do optimization for cameras on your platform.
*/

#ifdef CONFIG_ITU_A
static int smdkv310_cam0_reset(int dummy)
{
	int err;
	/* Camera A */
	err = gpio_request(S5PV310_GPX1(2), "GPX1");
	if (err)
		printk(KERN_ERR "#### failed to request GPX1_2 ####\n");

	s3c_gpio_setpull(S5PV310_GPX1(2), S3C_GPIO_PULL_NONE);
	gpio_direction_output(S5PV310_GPX1(2), 0);
	gpio_direction_output(S5PV310_GPX1(2), 1);
	gpio_free(S5PV310_GPX1(2));

	return 0;
}
#endif
#ifdef CONFIG_ITU_B
static int smdkv310_cam1_reset(int dummy)
{
#if 0   //must modify GPIO
	int err;

	/* Camera B */
	err = gpio_request(S5PV310_GPX1(0), "GPX1");
	if (err)
		printk(KERN_ERR "#### failed to request GPX1_0 ####\n");

	s3c_gpio_setpull(S5PV310_GPX1(0), S3C_GPIO_PULL_NONE);
	gpio_direction_output(S5PV310_GPX1(0), 0);
	gpio_direction_output(S5PV310_GPX1(0), 1);
	gpio_free(S5PV310_GPX1(0));
#endif

	return 0;
}
#endif
/* for 12M camera */
#ifdef CE143_MONACO
static int smdkv310_cam0_standby(void)
{
	int err;
	/* Camera A */
	err = gpio_request(S5PV310_GPX3(3), "GPX3");
	if (err)
		printk(KERN_ERR "#### failed to request GPX3_3 ####\n");
	s3c_gpio_setpull(S5PV310_GPX3(3), S3C_GPIO_PULL_NONE);
	gpio_direction_output(S5PV310_GPX3(3), 0);
	gpio_direction_output(S5PV310_GPX3(3), 1);
	gpio_free(S5PV310_GPX3(3));

	return 0;
}

static int smdkv310_cam1_standby(void)
{
	int err;

	/* Camera B */
	err = gpio_request(S5PV310_GPX1(1), "GPX1");
	if (err)
		printk(KERN_ERR "#### failed to request GPX1_1 ####\n");
	s3c_gpio_setpull(S5PV310_GPX1(1), S3C_GPIO_PULL_NONE);
	gpio_direction_output(S5PV310_GPX1(1), 0);
	gpio_direction_output(S5PV310_GPX1(1), 1);
	gpio_free(S5PV310_GPX1(1));

	return 0;
}
#endif

/* Set for MIPI-CSI Camera module Reset */
#ifdef CONFIG_CSI_C
static int smdkv310_mipi_cam0_reset(int dummy)
{
	int err;

	err = gpio_request(S5PV310_GPX1(2), "GPX1");
	if (err)
		printk(KERN_ERR "#### failed to reset(GPX1_2) MIPI CAM\n");

	s3c_gpio_setpull(S5PV310_GPX1(2), S3C_GPIO_PULL_NONE);
	gpio_direction_output(S5PV310_GPX1(2), 0);
	gpio_direction_output(S5PV310_GPX1(2), 1);
	gpio_free(S5PV310_GPX1(2));

	return 0;
}
#endif
#ifdef CONFIG_CSI_D
static int smdkv310_mipi_cam1_reset(int dummy)
{
	int err;

	err = gpio_request(S5PV310_GPX1(0), "GPX1");
	if (err)
		printk(KERN_ERR "#### failed to reset(GPX1_0) MIPI CAM\n");

	s3c_gpio_setpull(S5PV310_GPX1(0), S3C_GPIO_PULL_NONE);
	gpio_direction_output(S5PV310_GPX1(0), 0);
	gpio_direction_output(S5PV310_GPX1(0), 1);
	gpio_free(S5PV310_GPX1(0));

	return 0;
}
#endif

#ifdef CONFIG_VIDEO_S5K5CAGX
void cam0_s5k5cagx_reset(int power_up)
{
	int err;
	printk(KERN_INFO "s5k5cagx reset\n");

	err = gpio_request(S5PV310_GPC1(1), "GPC1");
	if (err)
		printk(KERN_ERR "#### failed to reset(GPC1_0) CAM0 \n");

	err = gpio_request(S5PV310_GPJ1(4), "GPJ1");
	if (err)
		printk(KERN_ERR "#### failed to reset(GPJ1_4) CAM0 \n");

	s3c_gpio_setpull(S5PV310_GPC1(0), S3C_GPIO_PULL_NONE);
	s3c_gpio_setpull(S5PV310_GPJ1(4), S3C_GPIO_PULL_NONE);

	gpio_direction_output(S5PV310_GPC1(1), 0);
	gpio_direction_output(S5PV310_GPC1(1), 1);

	gpio_direction_output(S5PV310_GPJ1(4), 1); //stnby

	if(power_up)
	{
		gpio_set_value(S5PV310_GPJ1(4), 1);
		//reset  --> L 
		gpio_set_value(S5PV310_GPC1(1), 0);
		mdelay(50);
		//reset  --> H			
		gpio_set_value(S5PV310_GPC1(1), 1);
	}
	else //power down
	{
		//reset  --> L 
		gpio_set_value(S5PV310_GPC1(1), 0);
		gpio_set_value(S5PV310_GPJ1(4), 0);
	}

	gpio_free(S5PV310_GPC1(1));
	gpio_free(S5PV310_GPJ1(4));

}	
#endif
#ifdef CONFIG_VIDEO_S5K4EAGX
void cam0_s5k4eagx_reset(int power_up)
{
	int err;
	printk(KERN_INFO "s5k4eagx reset\n");

	err = gpio_request(S5PV310_GPX1(3), "GPX1");
	if (err)
		printk(KERN_ERR "#### failed to reset(GPX1_3) CAM0 \n");
	err = gpio_request(S5PV310_GPJ1(4), "GPJ1");
	if (err)
		printk(KERN_ERR "#### failed to reset(GPJ1_4) CAM0 \n");

	s3c_gpio_setpull(S5PV310_GPX1(3), S3C_GPIO_PULL_NONE);
	s3c_gpio_setpull(S5PV310_GPJ1(4), S3C_GPIO_PULL_NONE);

	gpio_direction_output(S5PV310_GPX1(3), 0);
	gpio_direction_output(S5PV310_GPJ1(4), 1); //stnby

	if(power_up)
	{
		gpio_set_value(S5PV310_GPJ1(4), 1);

		//reset  --> L 
		gpio_set_value(S5PV310_GPX1(3), 0);
		mdelay(50);
		//reset  --> H			
		gpio_set_value(S5PV310_GPX1(3), 1);
		mdelay(50);
	}
	else //power down
	{
		//reset  --> L 
		gpio_set_value(S5PV310_GPX1(3), 0);
		gpio_set_value(S5PV310_GPJ1(4), 0);
	}
	gpio_free(S5PV310_GPX1(3));
	gpio_free(S5PV310_GPJ1(4));
}	
#endif
	
#ifdef CONFIG_VIDEO_S5K5AAFA
void cam1_s5k5aafa_reset(int power_up)
{
	int err;	
	printk(KERN_INFO "s5k5aafa reset\n");

	err = gpio_request(S5PV310_GPC1(0), "GPC1");	
	if (err)
		printk(KERN_ERR "#### failed to reset(GPC1_1) CAM1 \n");	

	err = gpio_request(S5PV310_GPE0(3), "GPE0");	
	if (err)
		printk(KERN_ERR "#### failed to reset(GPE0_3) CAM1 \n");

	gpio_direction_output(S5PV310_GPC1(0), 0);
	gpio_direction_output(S5PV310_GPC1(0), 1);
	gpio_direction_output(S5PV310_GPE0(3), 1); //stnby

	if(power_up)
	{
		gpio_set_value(S5PV310_GPJ0(3), 1);
		//reset  --> L 
		gpio_set_value(S5PV310_GPC1(0), 0);
		mdelay(50);
		//reset  --> H			
		gpio_set_value(S5PV310_GPC1(0), 1);
	}
	else //power down
	{
		//reset  --> L 
		gpio_set_value(S5PV310_GPJ1(3), 0);
		gpio_set_value(S5PV310_GPC1(0), 0);
	}
	gpio_free(S5PV310_GPC1(0));
	gpio_free(S5PV310_GPE0(3));

}	
#endif

#if 0
static int achro4210_cam0_power(int onoff)
{    
	static struct regulator *cam0_18_regulator;
	static struct regulator *cam0_28_regulator;
	int ret=0;

	cam0_18_regulator = regulator_get(NULL, "vdd_cam0_18");
	if (IS_ERR(cam0_18_regulator)) {
		printk(KERN_ERR "failed to get resource %s\n", "vdd_cam0_18");
		return -1;
	}
	
	cam0_28_regulator = regulator_get(NULL, "vdd_cam0_28");
	if (IS_ERR(cam0_28_regulator)) {
		printk(KERN_ERR "failed to get resource %s\n", "vdd_cam0_28");
		return -1;
	}

	cam1_s5k5aafa_reset(0);
	//
	if(onoff){
		ret=regulator_enable(cam0_18_regulator);
		ret=regulator_enable(cam0_28_regulator);

		mdelay(50);
		printk( KERN_INFO "cam0 power: ON\n");
		cam0_s5k5cagx_reset(1);

	}
	else{
		cam0_s5k5cagx_reset(0);
		printk( KERN_INFO "cam0 power: OFF\n");
		ret=regulator_disable(cam0_18_regulator);
		ret=regulator_disable(cam0_28_regulator);
	}
	return ret;
}
#endif

#ifdef CONFIG_KEYPAD_ACHRO_PC
static int achro4210_cam0_power_for_achro_pc(int onoff)
{	 
	static struct regulator *cam0_18_regulator;
	static struct regulator *cam0_28_regulator;
	int ret=0;
	
int err;

	
	//
	if(onoff){
	//	ret=regulator_enable(cam0_18_regulator);
	//	ret=regulator_enable(cam0_28_regulator);

		err = gpio_request(S5PV310_GPX1(0), "GPX1");	

		gpio_direction_output(S5PV310_GPX1(0), 1);
		gpio_set_value(S5PV310_GPX1(0), 1);
		

		mdelay(50);
		printk( KERN_INFO "cam0 power: ON\n");
		cam0_s5k4eagx_reset(1);

	}
	else{
		cam0_s5k4eagx_reset(0);
		printk( KERN_INFO "cam0 power: OFF\n");
	//	ret=regulator_disable(cam0_18_regulator);
		//ret=regulator_disable(cam0_28_regulator);
	}
	return ret;
}
#endif

static int achro4210_cam1_power(int onoff)
{      
	static struct regulator *cam1_15_regulator;
	static struct regulator *cam1_30_regulator;
	int ret=0;
	
	cam1_15_regulator = regulator_get(NULL, "vdd_cam1_15");
	if (IS_ERR(cam1_15_regulator)) {
		printk(KERN_ERR "failed to get resource %s\n", "vdd_cam1_15");
		return -1;
	}

	cam1_30_regulator = regulator_get(NULL, "vdd_cam1_30");
	if (IS_ERR(cam1_30_regulator)) {
		printk(KERN_ERR "failed to get resource %s\n", "vdd_cam1_30");
		return -1;
	}

	if(onoff){
		ret=regulator_enable(cam1_15_regulator);
		ret=regulator_enable(cam1_30_regulator);
		
		mdelay(50);
		printk( KERN_INFO "cam1 power: ON\n");
		mdelay(100);
	
	}
	else{
		mdelay(5);
		printk( KERN_INFO "cam1 power: OFF\n");
		ret=regulator_disable(cam1_15_regulator);
		ret=regulator_disable(cam1_30_regulator);
	}
	return ret;
}

#ifdef CONFIG_VIDEO_S5K3BA
static struct s5k3ba_platform_data s5k3ba_plat = {
	.default_width = 640,
	.default_height = 480,
	.pixelformat = V4L2_PIX_FMT_VYUY,
	.freq = 24000000,
	.is_mipi = 0,
};

static struct i2c_board_info  s5k3ba_i2c_info = {
	I2C_BOARD_INFO("S5K3BA", 0x2d),
	.platform_data = &s5k3ba_plat,
};

static struct s3c_platform_camera s5k3ba = {
#ifdef CONFIG_ITU_A
	.id		= CAMERA_PAR_A,
	.clk_name	= "sclk_cam0",
	.i2c_busnum	= 0,
	.cam_power	= smdkv310_cam0_reset,
#endif
#ifdef CONFIG_ITU_B
	.id		= CAMERA_PAR_B,
	.clk_name	= "sclk_cam1",
	.i2c_busnum	= 1,
	.cam_power	= smdkv310_cam1_reset,
#endif
	.type		= CAM_TYPE_ITU,
	.fmt		= ITU_601_YCBCR422_8BIT,
	.order422	= CAM_ORDER422_8BIT_CRYCBY,
	.info		= &s5k3ba_i2c_info,
	.pixelformat	= V4L2_PIX_FMT_VYUY,
	.srclk_name	= "xusbxti",
	.clk_rate	= 24000000,
	.line_length	= 1920,
	.width		= 640,
	.height		= 480,
	.window		= {
		.left	= 0,
		.top	= 0,
		.width	= 640,
		.height	= 480,
	},

	/* Polarity */
	.inv_pclk	= 0,
	.inv_vsync	= 1,
	.inv_href	= 0,
	.inv_hsync	= 0,

	.initialized	= 0,
};
#endif

#ifdef CONFIG_VIDEO_S5K4BA
static struct s5k4ba_platform_data s5k4ba_plat = {
	.default_width = 800,
	.default_height = 600,
	.pixelformat = V4L2_PIX_FMT_YUYV,
	.freq = 24000000,
	.is_mipi = 0,
};

static struct i2c_board_info  s5k4ba_i2c_info = {
	I2C_BOARD_INFO("S5K4BA", 0x2d),
	.platform_data = &s5k4ba_plat,
};

static struct s3c_platform_camera s5k4ba = {
#ifdef CONFIG_ITU_A
	.id		= CAMERA_PAR_A,
	.clk_name	= "sclk_cam0",
	.i2c_busnum	= 0,
	.cam_power	= smdkv310_cam0_reset,
#endif
#ifdef CONFIG_ITU_B
	.id		= CAMERA_PAR_B,
	.clk_name	= "sclk_cam1",
	.i2c_busnum	= 1,
	.cam_power	= smdkv310_cam1_reset,
#endif
	.type		= CAM_TYPE_ITU,
	.fmt		= ITU_601_YCBCR422_8BIT,
	.order422	= CAM_ORDER422_8BIT_YCBYCR,
	.info		= &s5k4ba_i2c_info,
	.pixelformat	= V4L2_PIX_FMT_YUYV,
	.srclk_name	= "mout_mpll",
	.clk_rate	= 60000000,
	.line_length	= 1920,
	.width		= 800,
	.height		= 600,
	.window		= {
		.left	= 0,
		.top	= 0,
		.width	= 800,
		.height	= 600,
	},

	/* Polarity */
	.inv_pclk	= 0,
	.inv_vsync	= 1,
	.inv_href	= 0,
	.inv_hsync	= 0,
	.reset_camera	= 1,
	.initialized	= 0,
};
#endif

/* 2 MIPI Cameras */
#ifdef CONFIG_VIDEO_S5K4EA
static struct s5k4ea_platform_data s5k4ea_plat = {
	.default_width = 1920,
	.default_height = 1080,
	.pixelformat = V4L2_PIX_FMT_UYVY,
	.freq = 24000000,
	.is_mipi = 1,
};

static struct i2c_board_info  s5k4ea_i2c_info = {
	I2C_BOARD_INFO("S5K4EA", 0x2d),
	.platform_data = &s5k4ea_plat,
};

static struct s3c_platform_camera s5k4ea = {
#ifdef CONFIG_CSI_C
	.id		= CAMERA_CSI_C,
	.clk_name	= "sclk_cam0",
	.i2c_busnum	= 0,
	.cam_power	= smdkv310_mipi_cam0_reset,
#endif
#ifdef CONFIG_CSI_D
	.id		= CAMERA_CSI_D,
	.clk_name	= "sclk_cam1",
	.i2c_busnum	= 1,
	.cam_power	= smdkv310_mipi_cam1_reset,
#endif
	.type		= CAM_TYPE_MIPI,
	.fmt		= MIPI_CSI_YCBCR422_8BIT,
	.order422	= CAM_ORDER422_8BIT_CBYCRY,
	.info		= &s5k4ea_i2c_info,
	.pixelformat	= V4L2_PIX_FMT_UYVY,
	.srclk_name	= "mout_mpll",
	.clk_rate	= 48000000,
	.line_length	= 1920,
	.width		= 1920,
	.height		= 1080,
	.window		= {
		.left	= 0,
		.top	= 0,
		.width	= 1920,
		.height	= 1080,
	},

	.mipi_lanes	= 2,
	.mipi_settle	= 12,
	.mipi_align	= 32,

	/* Polarity */
	.inv_pclk	= 0,
	.inv_vsync	= 1,
	.inv_href	= 0,
	.inv_hsync	= 0,

	.initialized	= 0,
};
#endif

#ifdef CONFIG_VIDEO_S5K6AA
static struct s5k6aa_platform_data s5k6aa_plat = {
	.default_width = 640,
	.default_height = 480,
	.pixelformat = V4L2_PIX_FMT_UYVY,
	.freq = 24000000,
	.is_mipi = 1,
};

static struct i2c_board_info  s5k6aa_i2c_info = {
	I2C_BOARD_INFO("S5K6AA", 0x3c),
	.platform_data = &s5k6aa_plat,
};

static struct s3c_platform_camera s5k6aa = {
#ifdef CONFIG_CSI_C
	.id		= CAMERA_CSI_C,
	.clk_name	= "sclk_cam0",
	.i2c_busnum	= 0,
	.cam_power	= smdkv310_mipi_cam0_reset,
#endif
#ifdef CONFIG_CSI_D
	.id		= CAMERA_CSI_D,
	.clk_name	= "sclk_cam1",
	.i2c_busnum	= 1,
	.cam_power	= smdkv310_mipi_cam1_reset,
#endif
	.type		= CAM_TYPE_MIPI,
	.fmt		= MIPI_CSI_YCBCR422_8BIT,
	.order422	= CAM_ORDER422_8BIT_CBYCRY,
	.info		= &s5k6aa_i2c_info,
	.pixelformat	= V4L2_PIX_FMT_UYVY,
	.srclk_name	= "xusbxti",
	.clk_rate	= 24000000,
	.line_length	= 1920,
	/* default resol for preview kind of thing */
	.width		= 640,
	.height		= 480,
	.window		= {
		.left	= 0,
		.top	= 0,
		.width	= 640,
		.height	= 480,
	},

	.mipi_lanes	= 1,
	.mipi_settle	= 6,
	.mipi_align	= 32,

	/* Polarity */
	.inv_pclk	= 0,
	.inv_vsync	= 1,
	.inv_href	= 0,
	.inv_hsync	= 0,

	.initialized	= 0,
};
#endif

#ifdef CONFIG_VIDEO_S5K5CAGX
static struct s5k5cagx_platform_data s5k5cagx_plat = {
	.default_width = 1920,
	.default_height = 1080,
	.pixelformat = V4L2_PIX_FMT_YUYV,
	.freq = 24000000,
	.is_mipi = 0,
};

static struct i2c_board_info  s5k5cagx_i2c_info = {
	I2C_BOARD_INFO("S5K5CAGX", 0x3c),
	.platform_data = &s5k5cagx_plat,
};

static struct s3c_platform_camera s5k5cagx = {
	.id		= CAMERA_PAR_A,
	.type		= CAM_TYPE_ITU,
	.fmt		= ITU_601_YCBCR422_8BIT,
	.order422	= CAM_ORDER422_8BIT_YCBYCR,
#ifdef CONFIG_KEYPAD_ACHRO_PC
	.i2c_busnum	= 2,
#else
	.i2c_busnum	= 1,
#endif
	.info		= &s5k5cagx_i2c_info,
	.pixelformat	= V4L2_PIX_FMT_YUYV,
	.srclk_name	= "xusbxti",
	.clk_name	= "sclk_cam0",
	.clk_rate	= 24000000,
	.line_length	= 480,//1920,
	.width		= 640,//1920,
	.height		= 480,//1080,
	.window		= {
		.left	= 0,
		.top	= 0,
		.width	= 640,//1920,
		.height	= 480,//640,//1080,
	},

	/* Polarity */
	.inv_pclk	= 0,
	.inv_vsync	= 1,
	.inv_href	= 0,
	.inv_hsync	= 0,

	.initialized	= 0,
#ifdef CONFIG_KEYPAD_ACHRO_PC
	.cam_power	= achro4210_cam0_power_for_achro_pc,
#else
	.cam_power	= achro4210_cam0_power,
#endif

};
#endif

#ifdef CONFIG_VIDEO_S5K4EAGX
static struct s5k5cagx_platform_data s5k4eagx_plat = {
	.default_width = 1920,
	.default_height = 1080,
	.pixelformat = V4L2_PIX_FMT_YUYV,
	.freq = 24000000,
	.is_mipi = 0,
};

static struct i2c_board_info  s5k4eagx_i2c_info = {
	I2C_BOARD_INFO("S5K4EAGX", 0x2d),
	.platform_data = &s5k4eagx_plat,
};

static struct s3c_platform_camera s5k4eagx = {
	.id		= CAMERA_PAR_A,
	.type		= CAM_TYPE_ITU,
	.fmt		= ITU_601_YCBCR422_8BIT,
	.order422	= CAM_ORDER422_8BIT_YCBYCR,
	.i2c_busnum	= 2,
	.info		= &s5k4eagx_i2c_info,
	.pixelformat	= V4L2_PIX_FMT_YUYV,
	.srclk_name	= "xusbxti",
	.clk_name	= "sclk_cam0",
	.clk_rate	= 24000000,
	.line_length	= 1600,//1920,
	.width		= 800,//1920,
	.height		= 600,//1080,
	.window		= {
		.left	= 0,
		.top	= 0,
		.width	= 800,//1920,
		.height	= 600,//640,//1080,
	},

	/* Polarity */
	.inv_pclk	= 0,
	.inv_vsync	= 1,
	.inv_href	= 0,
	.inv_hsync	= 0,

	.initialized	= 0,
	.cam_power	= achro4210_cam0_power_for_achro_pc,
};
#endif

#ifdef CONFIG_VIDEO_S5K5AAFA
static struct s5k5aafa_platform_data s5k5aafa_plat = {
	.default_width = 1920,
	.default_height = 1080,
	.pixelformat = V4L2_PIX_FMT_YUYV,
	.freq =24000000,// 26700000,
	.is_mipi = 0,
};

static struct i2c_board_info  s5k5aafa_i2c_info = {
	I2C_BOARD_INFO("S5K5AAFA", 0x2d),
	.platform_data = &s5k5aafa_plat,
};

static struct s3c_platform_camera s5k5aafa = {
	.id		= CAMERA_PAR_B,
	.type		= CAM_TYPE_ITU,
	.fmt		= ITU_601_YCBCR422_8BIT,
	.order422	= CAM_ORDER422_8BIT_CRYCBY,
	.i2c_busnum	= 1,
	.info		= &s5k5aafa_i2c_info,
	.pixelformat	= V4L2_PIX_FMT_YUYV,
	.srclk_name	= "xusbxti",
	.clk_name	= "sclk_cam1",
	.clk_rate	= 26700000,
	.line_length	= 480,//1920,
	.width		= 680,//1920,
	.height		= 480,//1080,
	.window		= {
		.left	= 0,
		.top	= 0,
		.width	= 640,//1920,
		.height	= 480,//640,//1080,
	},

	/* Polarity */
	.inv_pclk	= 0,
	.inv_vsync	= 1,
	.inv_href	= 0,
	.inv_hsync	= 0,

	.initialized	= 0,
	.cam_power	= achro4210_cam1_power,
};
#endif

#if defined(CONFIG_VIDEO_S5K4CA)
/*
 * External camera reset
 * Because the most of cameras take i2c bus signal, so that
 * you have to reset at the boot time for other i2c slave devices.
 * This function also called at fimc_init_camera()
 * Do optimization for cameras on your platform.
*/

void s5k4ca_power_control(int select)
{
	static struct regulator *cam0_18_regulator;
	static struct regulator *cam0_28_regulator;
	int ret=0;

	cam0_18_regulator = regulator_get(NULL, "vdd_cam0_18");
	if (IS_ERR(cam0_18_regulator)) {
		printk(KERN_ERR "failed to get resource %s\n", "vdd_cam0_18");
		return;
	}

	cam0_28_regulator = regulator_get(NULL, "vdd_cam0_28");
	if (IS_ERR(cam0_28_regulator)) {
		printk(KERN_ERR "failed to get resource %s\n", "vdd_cam0_28");
		return;
	}

	if(select == 1)
	{
		ret = regulator_enable(cam0_18_regulator);
		ret = regulator_enable(cam0_28_regulator);
		printk( KERN_INFO "s5k4ca: powered ON\n");
	} else {
		ret = regulator_disable(cam0_18_regulator);
		ret = regulator_disable(cam0_28_regulator);
		printk( KERN_INFO "s5k4ca: powered OFF\n");
	}
	mdelay(50);
}
EXPORT_SYMBOL(s5k4ca_power_control);

void cam0_s5k4ca_reset(int power_up)
{
	int err;
	printk(KERN_INFO "s5k4ca reset\n");

	err = gpio_request(S5PV310_GPC1(1), "GPC1");
	if (err)
		printk(KERN_ERR "#### failed to reset(GPC1_1) CAM0 \n");

	err = gpio_request(S5PV310_GPJ1(4), "GPJ1");
	if (err)
		printk(KERN_ERR "#### failed to reset(GPJ1_4) CAM0 \n");

	s3c_gpio_setpull(S5PV310_GPC1(1), S3C_GPIO_PULL_UP);
	s3c_gpio_setpull(S5PV310_GPJ1(4), S3C_GPIO_PULL_UP);

	gpio_direction_output(S5PV310_GPC1(1), 0);
	gpio_direction_output(S5PV310_GPC1(1), 1);

	gpio_direction_output(S5PV310_GPJ1(4), 1); //stnby

	if(power_up)
	{
		gpio_set_value(S5PV310_GPJ1(4), 1);
		//reset  --> L 
		gpio_set_value(S5PV310_GPC1(1), 0);
		mdelay(50);
		//reset  --> H			
		gpio_set_value(S5PV310_GPC1(1), 1);
	}
	else //power down
	{
		//reset  --> L 
		gpio_set_value(S5PV310_GPC1(1), 0);
		gpio_set_value(S5PV310_GPJ1(4), 0);
	}

	gpio_free(S5PV310_GPC1(1));
	gpio_free(S5PV310_GPJ1(4));
}
static int achro4210_cam0_power(int onoff)
{    
#if 1 ////camera test code : jhkang
	int ret=0;

	if(onoff){
		printk( KERN_INFO "cam0 power: ON\n");
		cam0_s5k4ca_reset(1);
	}
	else{
		cam0_s5k4ca_reset(0);
	}
	return ret;
#else
	static struct regulator *cam0_18_regulator;
	static struct regulator *cam0_28_regulator;
	int ret=0;

	cam0_18_regulator = regulator_get(NULL, "vdd_cam0_18");
	if (IS_ERR(cam0_18_regulator)) {
		printk(KERN_ERR "failed to get resource %s\n", "vdd_cam0_18");
		return -1;
	}
	
	cam0_28_regulator = regulator_get(NULL, "vdd_cam0_28");
	if (IS_ERR(cam0_28_regulator)) {
		printk(KERN_ERR "failed to get resource %s\n", "vdd_cam0_28");
		return -1;
	}

//	cam1_reset(0);
	//
	if(onoff){
		ret=regulator_enable(cam0_18_regulator);
		ret=regulator_enable(cam0_28_regulator);

		mdelay(50);
		printk( KERN_INFO "cam0 power: ON\n");
		cam0_s5k4ca_reset(1);

	}
	else{
		cam0_s5k4ca_reset(0);
		printk( KERN_INFO "cam0 power: OFF\n");
		ret=regulator_disable(cam0_18_regulator);
		ret=regulator_disable(cam0_28_regulator);
	}
	return ret;
#endif	
}

static struct s5k4ca_platform_data s5k4ca_plat = {
	.default_width = 800,
	.default_height = 600,
	.pixelformat = V4L2_PIX_FMT_YUV422P,
	.freq = 44000000,
	.is_mipi = 0,
};

static struct i2c_board_info  s5k4ca_i2c_info = {
	I2C_BOARD_INFO("S5K4CA", 0x2d),
	.platform_data = &s5k4ca_plat,
};

static struct s3c_platform_camera s5k4ca = {
	.id		= CAMERA_PAR_A,
	.type		= CAM_TYPE_ITU,
	.fmt		= ITU_601_YCBCR422_8BIT,
	.order422	= CAM_ORDER422_8BIT_CRYCBY,
	.i2c_busnum	= 1,
	.info		= &s5k4ca_i2c_info,
	.pixelformat	= V4L2_PIX_FMT_YUV422P,
	.srclk_name	= "xusbxti",
	.clk_name	= "sclk_cam0",
	.clk_rate	= 44000000,
	.line_length	= 1920,
	.width		= 800,
	.height		= 600,
	.window		= {
		.left	= 0,
		.top	= 0,
		.width	= 800,
		.height	= 600,
	},

	/* Polarity */
	.inv_pclk	= 0,
	.inv_vsync	= 1,
	.inv_href	= 0,
	.inv_hsync	= 0,

	.initialized	= 0,
	.cam_power	= achro4210_cam0_power,
};

#endif

#ifdef WRITEBACK_ENABLED
static struct i2c_board_info  writeback_i2c_info = {
	I2C_BOARD_INFO("WriteBack", 0x0),
};

static struct s3c_platform_camera writeback = {
	.id		= CAMERA_WB,
	.fmt		= ITU_601_YCBCR422_8BIT,
	.order422	= CAM_ORDER422_8BIT_CBYCRY,
	.i2c_busnum	= 0,
	.info		= &writeback_i2c_info,
	.pixelformat	= V4L2_PIX_FMT_YUV444,
	.line_length	= 800,
	.width		= 480,
	.height		= 800,
	.window		= {
		.left	= 0,
		.top	= 0,
		.width	= 480,
		.height	= 800,
	},

	.initialized	= 0,
};
#endif

/* Interface setting */
static struct s3c_platform_fimc fimc_plat = {
#ifdef CONFIG_ITU_A
	.default_cam	= CAMERA_PAR_A,
#endif
#ifdef CONFIG_ITU_B
	.default_cam	= CAMERA_PAR_B,
#endif
#ifdef CONFIG_CSI_C
	.default_cam	= CAMERA_CSI_C,
#endif
#ifdef CONFIG_CSI_D
	.default_cam	= CAMERA_CSI_D,
#endif
#ifdef WRITEBACK_ENABLED
	.default_cam	= CAMERA_WB,
#endif
	.camera		= {
#ifdef CONFIG_VIDEO_S5K3BA
		&s5k3ba,
#endif
#ifdef CONFIG_VIDEO_S5K4BA
		&s5k4ba,
#endif
#ifdef CONFIG_VIDEO_S5K4EA
		&s5k4ea,
#endif
#ifdef CONFIG_VIDEO_S5K6AA
		&s5k6aa,
#endif

#ifdef CONFIG_VIDEO_S5K5CAGX
		&s5k5cagx,
#endif
#ifdef CONFIG_VIDEO_S5K4EAGX
		&s5k4eagx,
#endif

#ifdef CONFIG_VIDEO_S5K5AAFA
		&s5k5aafa,
#endif

#if defined(CONFIG_VIDEO_S5K4CA)
		&s5k4ca,
#endif

#ifdef WRITEBACK_ENABLED
		&writeback,
#endif
	},
	.hw_ver		= 0x51,
};
#endif /* CONFIG_VIDEO_FIMC */
/*[------------------------------------------------------------------------------------------------------]*/
#if	defined(CONFIG_S3C64XX_DEV_SPI) || defined(CONFIG_SPI_S5PV310)
static struct s3c64xx_spi_csinfo spi1_csi[] = {
	[0] = {
		.line = S5PV310_GPB(5),
		.set_level = gpio_set_value,
	},
};

#if defined(CONFIG_SPI_S5PV310)
static void s3c_cs_suspend(int pin, pm_message_t pm)
{
		/* Whatever need to be done */
}
	
static void s3c_cs_resume(int pin)
{
		/* Whatever need to be done */
}

static void s3c_cs_set(int pin, int lvl)
{
		if(lvl == CS_HIGH)
		   s3c_gpio_setpin(pin, 1);
		else
		   s3c_gpio_setpin(pin, 0);
}
static void s3c_cs_config(int pin, int mode, int pull)
{
		s3c_gpio_cfgpin(pin, mode);

		if(pull == CS_HIGH){
		   s3c_gpio_setpull(pin, S3C_GPIO_PULL_UP);
		   s3c_gpio_setpin(pin, 0);
		}
		else{
		   s3c_gpio_setpull(pin, S3C_GPIO_PULL_DOWN);
		   s3c_gpio_setpin(pin, 1);
		}
}

#define S5PV210_GPB_OUTPUT(__gpio)	(0x1 << ((__gpio) * 4))

static struct s3c_spi_pdata s3c_slv_pdata_0[] __initdata = {
		[0] = { /* Slave-0 */
				.cs_level	  = CS_FLOAT,
				.cs_pin 	  = S5PV310_GPB(1),
				.cs_mode	  = S5PV210_GPB_OUTPUT(1),
				.cs_set 	  = s3c_cs_set,
				.cs_config	  = s3c_cs_config,
				.cs_suspend   = s3c_cs_suspend,
				.cs_resume	  = s3c_cs_resume,
		},
};

static struct s3c_spi_pdata s3c_slv_pdata_1[] __initdata = {
		[0] = { /* Slave-0 */
				.cs_level	  = CS_FLOAT,
				.cs_pin 	  = S5PV310_GPB(5),
				.cs_mode	  = S5PV210_GPB_OUTPUT(5),
				.cs_set 	  = s3c_cs_set,
				.cs_config	  = s3c_cs_config,
				.cs_suspend   = s3c_cs_suspend,
				.cs_resume	  = s3c_cs_resume,
		},
};


static struct spi_board_info s3c_spi_devs[] __initdata = {
[0] = {
#if defined(CONFIG_TDMB_MTV818)
		.modalias = "mtv350",
#else
	#if defined(CONFIG_TEST_SPI)
		.modalias = "spi_test",
	#else
		.modalias = "spidev",
	#endif
#endif
		.mode			 = SPI_MODE_0,	/* CPOL=0, CPHA=0 */
		.max_speed_hz	 = 6000000,
		/* Connected to SPI-1 as 1st Slave */
		.bus_num		 = 1,
		.irq			 = IRQ_SPI1,
		.chip_select	 = 0,
//					.controller_data = &spi1_csi[0],
		},

#if 0
    [1] = {
        .modalias        = "spidev", /* Test Interface */
        .mode            = SPI_MODE_0,  /* CPOL=0, CPHA=0 */
        .max_speed_hz    = 100000,
        /* Connected to SPI-0 as 2nd Slave */
        .bus_num         = 0,
        .irq             = IRQ_SPI0,
        .chip_select     = 1,
    },		
#endif	        

};

#else
static struct spi_board_info spi1_board_info[] __initdata = {
	{
#if defined(CONFIG_TDMB_MTV818)
		.modalias = "mtv350",
#else
	#if defined(CONFIG_TEST_SPI)
		.modalias = "spi_test",
	#else
		.modalias = "spidev",
	#endif
#endif
		.platform_data = NULL,
		.max_speed_hz = 1*1000*1000,
		.bus_num = 1,
		.chip_select = 0,
		.mode = SPI_MODE_0,
		.controller_data = &spi1_csi[0],
	}
};
#endif

#else
#define SPI1_CS S5PV310_GPB(5)
#define SPI1_CLK S5PV310_GPB(4)
#define SPI1_SI S5PV310_GPB(6)
#define SPI1_SO S5PV310_GPB(3)

static struct s3c64xx_spi_csinfo spi1_csi[] = {
	[0] = {
		.line = S5PV310_GPB(5),
		.set_level = gpio_set_value,
	},
};

static struct spi_board_info spi1_board_info[] __initdata = {
	{
#if defined(CONFIG_TDMB_MTV818)
		.modalias = "mtv350",
#endif
		.platform_data = NULL,
		.max_speed_hz = 6*1000*1000,
		.bus_num = 1,
		.chip_select = 0,
		.mode = SPI_MODE_0,
		//.controller_data = &spi1_csi[0],
		.controller_data = SPI1_CS,
	}	
};

static struct spi_gpio_platform_data mtv350_spi_gpio_data = {
    .sck    = SPI1_CLK,
    .mosi   = SPI1_SI,
    .miso   = SPI1_SO,
    .num_chipselect = 1,
};

static struct platform_device s3c_device_spi_gpio = {
    .name   = "spi_gpio",
    .id = 1,
    .dev    = {
//	.parent = &s3c_device_fb.dev
        .platform_data  = &mtv350_spi_gpio_data,
    },
};
#endif	//#if	defined(CONFIG_S3C64XX_DEV_SPI)
/*[------------------------------------------------------------------------------------------------------]*/
#if defined(CONFIG_BOARD_ACHRO4210)	// CHARLES

extern	void 	SYSTEM_POWER_CONTROL	(int power, int val);	// achro4210-sysfs.c
#if !defined(CONFIG_FB_S3C_HDMI_UI_FB)
extern	void 	STATUS_LED_CONTROL		(int led, int val);	// achro4210-sysfs.c
#endif
/*[------------------------------------------------------------------------------------------------------]*/
static struct platform_device achro4210_sysfs = {
    .name = "achro4210-sysfs",
    .id = -1,
};
/*[------------------------------------------------------------------------------------------------------]*/
#if defined(CONFIG_FB_S3C_HDMI_UI_FB)
	static struct platform_device fnd_sysfs = {
	    .name = "fnd-sysfs",
	    .id = -1,
	};
#endif
/*[------------------------------------------------------------------------------------------------------]*/
static void achro4210_power_off(void)
{
	/* PS_HOLD --> Output Low */
	printk(KERN_EMERG "%s : setting GPIO_PDA_PS_HOLD low.\n", __func__);

	SYSTEM_POWER_CONTROL(0, 0);	// BUCK6 Disable
	SYSTEM_POWER_CONTROL(1, 0);	// P5V Disable
	SYSTEM_POWER_CONTROL(2, 1);	// P12V(VLED) Disable
	
#if !defined(CONFIG_FB_S3C_HDMI_UI_FB)
	STATUS_LED_CONTROL(2, 0);	// BLUE LED OFF
#endif	
	
	/* PS_HOLD output High --> Low  PS_HOLD_CONTROL, R/W, 0x1002_330C */
	(*(unsigned long *)(S5PV310_VA_PMU + 0x330C)) = 0x5200;	// Power OFF

	while(1);

	printk(KERN_EMERG "%s : should not reach here!\n", __func__);
}
/*[------------------------------------------------------------------------------------------------------]*/
#if defined(CONFIG_BACKLIGHT_PWM)
static struct platform_pwm_backlight_data achro4210_backlight_data = {
	.pwm_id  = 2,
	.max_brightness = 255,

	.dft_brightness = 100,
	.pwm_period_ns  = 78770,
};
/*[------------------------------------------------------------------------------------------------------]*/
static struct platform_device achro4210_backlight_device = {
	.name      = "pwm-backlight",
	.id        = -1,
	.dev        = {
		.parent = &s3c_device_timer[2].dev,
		.platform_data = &achro4210_backlight_data,
	},
};
/*[------------------------------------------------------------------------------------------------------]*/
static void __init achro4210_backlight_register(void)
{
	int ret = platform_device_register(&achro4210_backlight_device);
	if (ret)
		printk(KERN_ERR "achro4210: failed to register backlight device: %d\n", ret);
}
#endif	// defined(CONFIG_BACKLIGHT_PWM)
/*[------------------------------------------------------------------------------------------------------]*/
#if defined(CONFIG_FB_S3C_LMS700KF06)
static struct s3c_platform_fb lms700kf06_data __initdata = {
	.hw_ver = 0x70,
	.clk_name = "sclk_lcd",
	.nr_wins = 5,
	.default_win = CONFIG_FB_S3C_DEFAULT_WINDOW,
	.swap = FB_SWAP_HWORD | FB_SWAP_WORD,

//	.reset_lcd = lms700kf06_reset_lcd,
};
#endif
/*[------------------------------------------------------------------------------------------------------]*/
#if defined(CONFIG_FB_S3C_A070VW08)
static struct s3c_platform_fb a070vw08_data __initdata = {
	.hw_ver = 0x70,
	.clk_name = "sclk_lcd",
	.nr_wins = 5,
	.default_win = CONFIG_FB_S3C_DEFAULT_WINDOW,
	.swap = FB_SWAP_HWORD | FB_SWAP_WORD,
};
#endif
/*[------------------------------------------------------------------------------------------------------]*/
#if defined(CONFIG_FB_S3C_TRULY1280)
static struct s3c_platform_fb truly1280_data __initdata = {
	.hw_ver = 0x70,
	.clk_name = "sclk_lcd",
	.nr_wins = 5,
	.default_win = CONFIG_FB_S3C_DEFAULT_WINDOW,
	.swap = FB_SWAP_HWORD | FB_SWAP_WORD,
};
#endif
/*[------------------------------------------------------------------------------------------------------]*/
#if defined(CONFIG_FB_S3C_HDMI_UI_FB)
static struct s3c_platform_fb hdmi_ui_fb_data __initdata = {
	.hw_ver = 0x70,
	.clk_name = "sclk_lcd",
	.nr_wins = 5,
	.default_win = CONFIG_FB_S3C_DEFAULT_WINDOW,
	.swap = FB_SWAP_HWORD | FB_SWAP_WORD,
};
#endif
/*[------------------------------------------------------------------------------------------------------]*/
#if defined (CONFIG_BATTERY_MAX17040)
#include <linux/max17040_battery.h>
static int max8903g_charger_enable(void)
{
	gpio_set_value(GPIO_CHARGER_LED, 0);
	gpio_set_value(GPIO_CHARGER_ENABLE, 0);
	msleep_interruptible(2);
	return	gpio_get_value(GPIO_CHARGER_STATUS) ? 0 : 1;
}
static void max8903g_charger_disable(void)
{
	gpio_set_value(GPIO_CHARGER_LED, 1);
	gpio_set_value(GPIO_CHARGER_ENABLE, 1);
	msleep_interruptible(2);
}
static int max8903g_charger_done(void)
{
	gpio_set_value(GPIO_CHARGER_LED, gpio_get_value(GPIO_CHARGER_STATUS));
	return	gpio_get_value(GPIO_CHARGER_STATUS) ? 1 : 0;
}
static int max8903g_charger_ac_online(void)
{
	return	gpio_get_value(GPIO_CHARGER_AC_ONLINE) ? 0 : 1;
}
static int max8903g_charger_usb_online(void)
{
	return	gpio_get_value(GPIO_CHARGER_USB_ONLINE) ? 0 : 1;
}

static int max8903g_battery_online(void)
{
	/*think that  battery always is inserted */
	return 1;
}
static struct max17040_platform_data max17040_platform_data = {
	.charger_enable = max8903g_charger_enable,
	.charger_ac_online = max8903g_charger_ac_online,
	.charger_usb_online = max8903g_charger_usb_online,
	.battery_online = max8903g_battery_online,
	.charger_done = max8903g_charger_done,
	.charger_disable = max8903g_charger_disable,
};
#endif	//	#if defined (CONFIG_BATTERY_MAX17040)
/*[------------------------------------------------------------------------------------------------------]*/
#if defined(CONFIG_ACHRO_HEADSET)
static struct achro_jack_port achro_jack_port[] = {
	{
		{
			.eint		= EINT_HEADSET,
			.gpio		= GPIO_HEADSET,
		},
	}
};
/*[------------------------------------------------------------------------------------------------------]*/
static struct achro_jack_platform_data achro_jack_data = {
		.port			= achro_jack_port,
		.nheadsets		= ARRAY_SIZE(achro_jack_port),
};
/*[------------------------------------------------------------------------------------------------------]*/
static struct platform_device achro_device_jack = {
		.name			= "achro_jack",
		.id 			= -1,
		.dev			= {
				.platform_data	= &achro_jack_data,
		},
};
/*[------------------------------------------------------------------------------------------------------]*/
#endif


#endif	// defined(CONFIG_BOARD_ACHRO4210)
/*[------------------------------------------------------------------------------------------------------]*/
#ifdef CONFIG_I2C_S3C2410
/* I2C0 */
static struct i2c_board_info i2c_devs0[] __initdata = {
#if defined(CONFIG_REGULATOR_MAX8997)
	{
		/* The address is 0xCC used since SRAD = 0 */
		I2C_BOARD_INFO("max8997", (0xCC >> 1)),
		.platform_data = &max8997_pdata,
	},
#endif
	{
		I2C_BOARD_INFO("max98089", (0x20>>1)),
	},
};
/*[------------------------------------------------------------------------------------------------------]*/
#ifdef CONFIG_S3C_DEV_I2C1

#if defined (GPIO_I2C1_CONTROL)
#include	<linux/i2c-gpio.h>

static struct 	i2c_gpio_platform_data 	i2c1_gpio_platdata = {
	.sda_pin = S5PV310_GPD1(2),
	.scl_pin = S5PV310_GPD1(3),
	.udelay  = 5,
	.sda_is_open_drain = 0,
	.scl_is_open_drain = 0,
	.scl_is_output_only = 0
};

static struct 	platform_device 	i2c1_gpio_device = {
	.name 	= "i2c-gpio",
	.id  	= 1,
	.dev.platform_data = &i2c1_gpio_platdata,
};
#endif
/* I2C1 */
static struct i2c_board_info i2c_devs1[] __initdata = {
	#ifdef CONFIG_VIDEO_TVOUT
		{
			I2C_BOARD_INFO("s5p_ddc", 0x3A),
		},
	#endif
	#if defined (CONFIG_BATTERY_MAX17040)
		{
			I2C_BOARD_INFO("max17040", 0x36), 
			.platform_data = &max17040_platform_data,
		},
	#endif
	#if defined(CONFIG_BH1780GLI)
		{
			I2C_BOARD_INFO("bh1780", (0x52 >> 1)),	// Light Sensor
		},
	#endif
};
#endif
/*[------------------------------------------------------------------------------------------------------]*/
#ifdef CONFIG_S3C_DEV_I2C2
#if defined(GPIO_I2C2_CONTROL)

#include	<linux/i2c-gpio.h>

static struct 	i2c_gpio_platform_data 	i2c2_gpio_platdata = {
	.sda_pin = S5PV310_GPA0(6), // gpio number
	.scl_pin = S5PV310_GPA0(7),
	.udelay  = 2,
	.sda_is_open_drain = 0,
	.scl_is_open_drain = 0,
	.scl_is_output_only = 0
};

static struct 	platform_device 	i2c2_gpio_device = {
	.name 	= "i2c-gpio",
	.id  	= 2, // adepter number
	.dev.platform_data = &i2c2_gpio_platdata,
};
#endif	//#if defined(GPIO_I2C2_CONTROL)
#ifdef CONFIG_MPU_SENSORS_MPU3050
static struct mpu3050_platform_data mpu3050_data={
	.int_config		= 	0x10,
	// HISEOB : CORRECTED! DO NOT CHANGE!
	.orientation	= 
		{ 0, 1, 0,
		  1, 0, 0,
		  0, 0, 1 },
	.level_shifter	= 0,

	.accel = {
		.get_slave_descr=bma150_get_slave_descr,
		.adapt_num = 2,
		.irq = IRQ_EINT(23),
		.bus = EXT_SLAVE_BUS_SECONDARY,
		.address = 0x38,
		// HISEOB : CORRECTED! DO NOT CHANGE!
		.orientation = 
			{ -1, 0, 0,
			   0,-1, 0,
			   0, 0, 1 },
	},

	.compass = {
		.get_slave_descr = ams0303_get_slave_descr,
		.adapt_num 	= 2,
		.irq = IRQ_EINT(13),
		.bus		= EXT_SLAVE_BUS_PRIMARY,
		.address 	= 0x28,
		.orientation = 
			{ 1, 0, 0,
			  0, -1, 0,
			  0, 0, 1 },
	},
};

#endif //#ifdef CONFIG_MPU_SENSORS_MPU3050
/*[------------------------------------------------------------------------------------------------------]*/
static struct i2c_board_info i2c_devs2[] __initdata = {
	#if defined(CONFIG_INPUT_YAMAHA_SENSORS)
		{ 	
			I2C_BOARD_INFO("bma150", (0x70 >> 1)), 
		},
		{
			I2C_BOARD_INFO("yas529", (0x5C >> 1)),
		},
	#else
		#ifdef CONFIG_MPU_SENSORS_MPU3050
		{
			I2C_BOARD_INFO("mpu3050", 0x68),
			.irq = S5PV310_GPX3(2),
			.platform_data = &mpu3050_data,
		},
		#endif //#ifdef CONFIG_MPU_SENSORS_MPU3050                    		
	#endif
};
/*[------------------------------------------------------------------------------------------------------]*/
static void achro4210_i2c2_set_eint() {
	if (gpio_request(S5PV310_GPX3(2), "MPU3050 INT"))
		printk("MPU3050 INT(GPX3.2) Port request error!!!\n");
	else {
		s3c_gpio_setpull(S5PV310_GPX3(2), S3C_GPIO_PULL_DOWN);
		gpio_direction_input(S5PV310_GPX3(2));
		gpio_free(S5PV310_GPX3(2));
	}

	if (gpio_request(S5PV310_GPX2(7), "BMA150 INT"))
		printk("BMA150 INT(GPX2.7) Port request error!!!\n");
	else {
		s3c_gpio_setpull(S5PV310_GPX2(7), S3C_GPIO_PULL_DOWN);
		gpio_direction_input(S5PV310_GPX2(7));
		gpio_free(S5PV310_GPX2(7));
	}

	if (gpio_request(S5PV310_GPX0(3), "MAG Reset"))
		printk("MAG Reset(GPX0.3) Port request error!!!\n");
	else {
		s3c_gpio_setpull(S5PV310_GPX0(3), S3C_GPIO_PULL_DOWN);
		gpio_direction_output(S5PV310_GPX0(3), 1);
		gpio_free(S5PV310_GPX0(3));
	}

	if (gpio_request(S5PV310_GPX1(5), "MAG INT"))
		printk("MAG INT(GPX1.5) Port request error!!!\n");
	else {
		s3c_gpio_setpull(S5PV310_GPX1(5), S3C_GPIO_PULL_DOWN);
		gpio_direction_input(S5PV310_GPX1(5));
		gpio_free(S5PV310_GPX1(5));
	}
}
/*[------------------------------------------------------------------------------------------------------]*/
#endif
/*[------------------------------------------------------------------------------------------------------]*/
#ifdef CONFIG_S3C_DEV_I2C3
static struct i2c_board_info i2c_devs3[] __initdata = {
};
#endif
/*[------------------------------------------------------------------------------------------------------]*/
#ifdef CONFIG_S3C_DEV_I2C4
/* I2C4 */
static struct i2c_board_info i2c_devs4[] __initdata = {
};
#endif
/*[------------------------------------------------------------------------------------------------------]*/
#ifdef CONFIG_S3C_DEV_I2C5
static struct i2c_board_info i2c_devs5[] __initdata = {
};
#endif
/*[------------------------------------------------------------------------------------------------------]*/
#ifdef CONFIG_S3C_DEV_I2C6
static struct i2c_board_info i2c_devs6[] __initdata = {
};
#endif
/*[------------------------------------------------------------------------------------------------------]*/
#ifdef CONFIG_S3C_DEV_I2C7
static struct i2c_board_info i2c_devs7[] __initdata = {
};
#endif
/*[------------------------------------------------------------------------------------------------------]*/
#ifdef CONFIG_TOUCHSCREEN_EETI_EXC7200
#define	GPIO_I2C8_CONTROL
#if defined(GPIO_I2C8_CONTROL)
#include	<linux/i2c-gpio.h>

#define GPIO_TOUCH_SDA S5PV310_GPL1(1)
#define GPIO_TOUCH_SCL S5PV310_GPL1(0)


#define EINT_TOUCH     IRQ_EINT(8) // S5PV310_GPX1(0)

static struct 	i2c_gpio_platform_data 	i2c8_gpio_platdata = {
	.sda_pin = GPIO_TOUCH_SDA, // gpio number
	.scl_pin = GPIO_TOUCH_SCL,
	.udelay  = 2,
	.sda_is_open_drain = 0,
	.scl_is_open_drain = 0,
	.scl_is_output_only = 0
};

static struct 	platform_device 	s3c_device_i2c_touch = {
	.name 	= "i2c-gpio",
	.id  	= 8, // adepter number
	.dev.platform_data = &i2c8_gpio_platdata,
};
#endif
/* I2C8 */
static struct i2c_board_info i2c_devs8[] __initdata = {
	{ 	
		I2C_BOARD_INFO("egalax_i2c", (0x04)), 
		.irq = EINT_TOUCH,
	},
};
#endif
/*[------------------------------------------------------------------------------------------------------]*/
#ifdef CONFIG_TOUCHSCREEN_BT412B 
#define	GPIO_I2C8_CONTROL
#if defined(GPIO_I2C8_CONTROL)

#include	<linux/i2c-gpio.h>

#define GPIO_TOUCH_SDA S5PV310_GPL1(1)
#define GPIO_TOUCH_SCL S5PV310_GPL1(0)
#define EINT_TOUCH     IRQ_EINT(8)

static struct 	i2c_gpio_platform_data 	i2c8_gpio_platdata = {
	.sda_pin = GPIO_TOUCH_SDA,
	.scl_pin = GPIO_TOUCH_SCL,
	.udelay  = 3,
	.sda_is_open_drain = 0,
	.scl_is_open_drain = 0,
	.scl_is_output_only = 0
};

static struct 	platform_device 	s3c_device_i2c_touch = {
	.name 	= "i2c-gpio",
	.id  	= 8,
	.dev.platform_data = &i2c8_gpio_platdata,
};
#endif

static struct i2c_board_info i2c_devs8[] __initdata = {
	{
			I2C_BOARD_INFO("sain_touch", 0x20),
			.irq = EINT_TOUCH,
	},
};

static void achro4210_i2c8_gpio_init() {
	if (gpio_request(S5PV310_GPL1(1), "GPL1"))
		printk("TOUCH SDA(GPL1.1) Port request error!!!\n");
	else {
		s3c_gpio_setpull(S5PV310_GPL1(1), S3C_GPIO_PULL_UP);
		gpio_direction_input(S5PV310_GPL1(1));
		gpio_free(S5PV310_GPL1(1));
	}
	if (gpio_request(S5PV310_GPL1(0), "GPL1"))
		printk("TOUCH SDA(GPL1.0)  Port request error!!!\n");
	else {
		s3c_gpio_setpull(S5PV310_GPL1(0), S3C_GPIO_PULL_UP);
		gpio_direction_input(S5PV310_GPL1(0));
		gpio_free(S5PV310_GPL1(0));
	}
	if (gpio_request(S5PV310_GPX1(0), "GPX1"))
		printk(KERN_ERR "#### failed to request GPX1_0 ####\n");
	else {
		s3c_gpio_setpull(S5PV310_GPX1(0), S3C_GPIO_PULL_NONE);
		gpio_direction_input(S5PV310_GPX1(0));
		gpio_free(S5PV310_GPX1(0));
	}
	if (gpio_request(S5PV310_GPC0(3), "GPC0.3"))
		printk(KERN_ERR "#### failed to request GPC0_3 ####\n");
	else {
		s3c_gpio_setpull(S5PV310_GPC0(3), S3C_GPIO_PULL_NONE);
		gpio_direction_output(S5PV310_GPC0(3), 1);
		gpio_set_value(S5PV310_GPC0(3), 0);
		mdelay(100);
		gpio_set_value(S5PV310_GPC0(3), 1);
		gpio_free(S5PV310_GPC0(3));
	}
}
#endif
/*[------------------------------------------------------------------------------------------------------]*/
#endif	// #ifdef CONFIG_I2C_S3C2410
/*[------------------------------------------------------------------------------------------------------]*/
#ifdef CONFIG_BATTERY_ACHRO_DUMMY
static struct platform_device achro_battery = {
        .name = "dummy-battery",
};
#endif
/*[------------------------------------------------------------------------------------------------------]*/
#ifdef CONFIG_S3C_DEV_HSMMC
static struct s3c_sdhci_platdata smdkc210_hsmmc0_pdata __initdata = {
#if defined(CONFIG_BCM4329_WIFI_ENABLE)
	.cd_type		= S3C_SDHCI_CD_GPIO,
#else
	.cd_type		= S3C_SDHCI_CD_INTERNAL,
#endif
#if defined(CONFIG_S5PV310_SD_CH0_8BIT)
	.max_width		= 8,
	.host_caps		= MMC_CAP_8_BIT_DATA,
#endif
};
#endif
/*[------------------------------------------------------------------------------------------------------]*/
#ifdef CONFIG_S3C_DEV_HSMMC1
static struct s3c_sdhci_platdata smdkc210_hsmmc1_pdata __initdata = {
	.cd_type		= S3C_SDHCI_CD_INTERNAL,
};
#endif
/*[------------------------------------------------------------------------------------------------------]*/
#ifdef CONFIG_S3C_DEV_HSMMC2
static struct s3c_sdhci_platdata smdkc210_hsmmc2_pdata __initdata = {
	.cd_type		= S3C_SDHCI_CD_INTERNAL,
#if defined(CONFIG_S5PV310_SD_CH2_8BIT)
	.max_width		= 8,
	.host_caps		= MMC_CAP_8_BIT_DATA,
#endif
};
#endif
/*[------------------------------------------------------------------------------------------------------]*/
#ifdef CONFIG_S3C_DEV_HSMMC3
static struct s3c_sdhci_platdata smdkc210_hsmmc3_pdata __initdata = {
	.cd_type		= S3C_SDHCI_CD_INTERNAL,
};
#endif
/*[------------------------------------------------------------------------------------------------------]*/
#ifdef CONFIG_S5P_DEV_MSHC
static struct s3c_mshci_platdata smdkc210_mshc_pdata __initdata = {
	.cd_type		= S3C_SDHCI_CD_INTERNAL,
#if defined(CONFIG_S5PV310_MSHC_CH0_8BIT) && \
defined(CONFIG_S5PV310_MSHC_CH0_DDR)
	.max_width		= 8,
	.host_caps		= MMC_CAP_8_BIT_DATA | MMC_CAP_DDR,
#elif defined(CONFIG_S5PV310_MSHC_CH0_8BIT)
	.max_width		= 8,
	.host_caps		= MMC_CAP_8_BIT_DATA,
#elif defined(CONFIG_S5PV310_MSHC_CH0_DDR)
	.host_caps				= MMC_CAP_DDR,
#endif
};
#endif
/*[------------------------------------------------------------------------------------------------------]*/
#ifdef CONFIG_VIDEO_FIMG2D
static struct fimg2d_platdata fimg2d_data __initdata = {
	.hw_ver = 30,
	.parent_clkname = "mout_mpll",
	.clkname = "sclk_fimg2d",
	.gate_clkname = "fimg2d",
	.clkrate = 250 * 1000000,
};
#endif
/*[------------------------------------------------------------------------------------------------------]*/
#ifdef CONFIG_SMSC911X
static struct resource s5p_smsc911x_resources[] = {
	[0] = {
		.start = S5PV310_PA_SROM1,
		.end   = S5PV310_PA_SROM1 + 0xFF,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = IRQ_EINT(5),
		.end   = IRQ_EINT(5),
		.flags = IORESOURCE_IRQ|IORESOURCE_IRQ_LOWLEVEL,
	}
};

static struct smsc911x_platform_config smsc911x_config = {
	.irq_polarity   = SMSC911X_IRQ_POLARITY_ACTIVE_LOW,
	.irq_type       = SMSC911X_IRQ_TYPE_PUSH_PULL,
	.flags          = SMSC911X_USE_16BIT|SMSC911X_FORCE_INTERNAL_PHY,
	.phy_interface  = PHY_INTERFACE_MODE_MII,
};

struct platform_device s5p_device_smsc911x = {
	.name           = "smsc911x",
	.id             =  -1,
	.num_resources  = ARRAY_SIZE(s5p_smsc911x_resources),
	.resource       = s5p_smsc911x_resources,
	.dev = {
		.platform_data = &smsc911x_config,
	},
};

#if 0
static int __init ethaddr_setup(char *line)
{
        char *ep;
        int i;

        /* there should really be routines to do this stuff */
        for (i = 0; i < 6; i++) {
                smsc911x_config.dev_addr[i] = line ? simple_strtoul(line, &ep, 16) : 0;
                if (line)
                        line = (*ep) ? ep+1 : ep;
        }
        printk("Huins ddddd User MAC address: %pM\n", smsc911x_config.dev_addr);
        return 0;
}
__setup("ethaddr=", ethaddr_setup);
#endif
// HISEOB : smsc911x Init Codes
static void __init achro4210_smsc911x_init(void)
{
	u32 tmp;
	int i;
	unsigned int *srom_bank1;
	// SROM_CSn[1]
	s3c_gpio_cfgpin(S5PV310_GPY0(1), S3C_GPIO_SFN(2));
	s3c_gpio_setpull(S5PV310_GPY0(1), S3C_GPIO_PULL_NONE);

	for(i = 0; i < 8; i++) {
		s3c_gpio_cfgpin(S5PV310_GPY5(i), S3C_GPIO_SFN(2));
		s3c_gpio_setpull(S5PV310_GPY5(i), S3C_GPIO_PULL_NONE);
		s3c_gpio_cfgpin(S5PV310_GPY6(i), S3C_GPIO_SFN(2));
		s3c_gpio_setpull(S5PV310_GPY6(i), S3C_GPIO_PULL_NONE);
	}

	/* configure nCS1 width to 16 bits */
	tmp = __raw_readl(S5PV310_SROM_BW) &
		    ~(S5PV310_SROM_BW__CS_MASK <<
				    S5PV310_SROM_BW__NCS1__SHIFT);
	tmp |= (
		(1 << S5PV310_SROM_BW__DATAWIDTH__SHIFT)	|
		(1 << S5PV310_SROM_BW__ADDRMODE__SHIFT)		|
		(0 << S5PV310_SROM_BW__WAITENABLE__SHIFT)	|
		(1 << S5PV310_SROM_BW__BYTEENABLE__SHIFT)	)
		<< S5PV310_SROM_BW__NCS1__SHIFT;
	__raw_writel(tmp, S5PV310_SROM_BW);
	/* set timing for nCS1 suitable for ethernet chip */
	__raw_writel(
			(0x0 << S5PV310_SROM_BCX__PMC__SHIFT)	|
		    (0x6 << S5PV310_SROM_BCX__TACP__SHIFT)	|
		    (0x4 << S5PV310_SROM_BCX__TCAH__SHIFT)	|
		    (0x1 << S5PV310_SROM_BCX__TCOH__SHIFT)	|
			(0xe << S5PV310_SROM_BCX__TACC__SHIFT)	|
			(0x4 << S5PV310_SROM_BCX__TCOS__SHIFT)	|
			(0x0 << S5PV310_SROM_BCX__TACS__SHIFT),
			S5PV310_SROM_BC1);
	/* configure nCS0 width to 8 bits */
	tmp = __raw_readl(S5PV310_SROM_BW) &
		~(S5PV310_SROM_BW__CS_MASK <<
				S5PV310_SROM_BW__NCS0__SHIFT);
	tmp |= (
			(0 << S5PV310_SROM_BW__DATAWIDTH__SHIFT)    |
			(1 << S5PV310_SROM_BW__ADDRMODE__SHIFT)     |
			(0 << S5PV310_SROM_BW__WAITENABLE__SHIFT)   |
			(1 << S5PV310_SROM_BW__BYTEENABLE__SHIFT)   )
		<< S5PV310_SROM_BW__NCS0__SHIFT;
	__raw_writel(tmp, S5PV310_SROM_BW);


	/* set timing for nCS0 suitable for FPGA*/
	__raw_writel(
			(0x0 << S5PV310_SROM_BCX__PMC__SHIFT)   | 
			(0x6 << S5PV310_SROM_BCX__TACP__SHIFT)  |  
			(0x4 << S5PV310_SROM_BCX__TCAH__SHIFT)  |  
			(0x1 << S5PV310_SROM_BCX__TCOH__SHIFT)  |  
			(0xd << S5PV310_SROM_BCX__TACC__SHIFT)  |  
			(0x4 << S5PV310_SROM_BCX__TCOS__SHIFT)  |  
			(0x0 << S5PV310_SROM_BCX__TACS__SHIFT),       
			S5PV310_SROM_BC0);
}
#endif
/*[------------------------------------------------------------------------------------------------------]*/
#ifdef CONFIG_ANDROID_PMEM
static struct android_pmem_platform_data pmem_pdata = {
	.name = "pmem",
	.no_allocator = 1,
	.cached = 0,
	.start = 0, // will be set during proving pmem driver.
	.size = 0 // will be set during proving pmem driver.
};

static struct android_pmem_platform_data pmem_gpu1_pdata = {
	.name = "pmem_gpu1",
	.no_allocator = 1,
	.cached = 0,
	/* .buffered = 1, */
	.start = 0,
	.size = 0,
};

static struct platform_device pmem_device = {
	.name = "android_pmem",
	.id = 0,
	.dev = { .platform_data = &pmem_pdata },
};

static struct platform_device pmem_gpu1_device = {
	.name = "android_pmem",
	.id = 1,
	.dev = { .platform_data = &pmem_gpu1_pdata },
};

static void __init android_pmem_set_platdata(void)
{
#if defined(CONFIG_S5P_MEM_CMA)
	pmem_pdata.size = CONFIG_ANDROID_PMEM_MEMSIZE_PMEM * SZ_1K;
	pmem_gpu1_pdata.size = CONFIG_ANDROID_PMEM_MEMSIZE_PMEM_GPU1 * SZ_1K;
#elif defined(CONFIG_S5P_MEM_BOOTMEM)
	pmem_pdata.start = (u32)s5p_get_media_memory_bank(S5P_MDEV_PMEM, 0);
	pmem_pdata.size = (u32)s5p_get_media_memsize_bank(S5P_MDEV_PMEM, 0);

	pmem_gpu1_pdata.start = (u32)s5p_get_media_memory_bank(S5P_MDEV_PMEM_GPU1, 0);
	pmem_gpu1_pdata.size = (u32)s5p_get_media_memsize_bank(S5P_MDEV_PMEM_GPU1, 0);
#endif
}
#endif
/*[------------------------------------------------------------------------------------------------------]*/
#ifdef CONFIG_BATTERY_S3C
struct platform_device sec_device_battery = {
	.name	= "sec-fake-battery",
	.id	= -1,
};
#endif
/*[------------------------------------------------------------------------------------------------------]*/
static struct resource pmu_resource[] = {
	[0] = {
		.start = IRQ_PMU_0,
		.end   = IRQ_PMU_0,
		.flags = IORESOURCE_IRQ,
	},
	[1] = {
		.start = IRQ_PMU_1,
		.end   = IRQ_PMU_1,
		.flags = IORESOURCE_IRQ,
	}
};
/*[------------------------------------------------------------------------------------------------------]*/
static struct platform_device pmu_device = {
	.name 		= "arm-pmu",
	.id 		= ARM_PMU_DEVICE_CPU,
	.resource 	= pmu_resource,
	.num_resources 	= 2,
};
/*[------------------------------------------------------------------------------------------------------]*/
static struct platform_device *smdkc210_devices[] __initdata = {

#ifdef CONFIG_BOARD_ACHRO4210	// CHARLES
	&achro4210_sysfs,
#if defined(CONFIG_FB_S3C_HDMI_UI_FB)
	&fnd_sysfs,
#endif
#endif

#ifdef CONFIG_BACKLIGHT_PWM			// CHARLES
	&s3c_device_timer[2],
#endif

#ifdef CONFIG_S5PV310_DEV_PD
	&s5pv310_device_pd[PD_MFC],
	&s5pv310_device_pd[PD_G3D],
	&s5pv310_device_pd[PD_LCD0],
	&s5pv310_device_pd[PD_LCD1],
	&s5pv310_device_pd[PD_CAM],
	&s5pv310_device_pd[PD_GPS],
	&s5pv310_device_pd[PD_TV],
	/* &s5pv310_device_pd[PD_MAUDIO], */
#endif

#ifdef CONFIG_BATTERY_S3C
	&sec_device_battery,
#endif

#ifdef CONFIG_FB_S3C
	&s3c_device_fb,
#endif

#ifdef CONFIG_I2C_S3C2410
	&s3c_device_i2c0,
#if defined(CONFIG_S3C_DEV_I2C1)
#if defined (GPIO_I2C1_CONTROL)
	&i2c1_gpio_device,
#else
	&s3c_device_i2c1,
#endif
#endif
#if defined(CONFIG_S3C_DEV_I2C2)
#if defined(GPIO_I2C2_CONTROL)
	&i2c2_gpio_device,
#else
	&s3c_device_i2c2,
#endif
#endif
#if defined(CONFIG_S3C_DEV_I2C3)
#if defined(GPIO_I2C3_CONTROL)
	&i2c3_gpio_device,
#else
	&s3c_device_i2c3,
#endif
#endif
#if defined(CONFIG_S3C_DEV_I2C4)
#if defined(GPIO_I2C4_CONTROL)
	&i2c4_gpio_device,
#else
	&s3c_device_i2c4,
#endif
#endif
#if defined(CONFIG_S3C_DEV_I2C5)
#if defined(GPIO_I2C5_CONTROL)
	&i2c5_gpio_device,
#else
	&s3c_device_i2c5,
#endif
#endif
#if defined(CONFIG_S3C_DEV_I2C6)
#if defined(GPIO_I2C6_CONTROL)
	&i2c6_gpio_device,
#else
	&s3c_device_i2c6,
#endif
#endif
#if defined(CONFIG_S3C_DEV_I2C7)
#if defined(GPIO_I2C7_CONTROL)
	&i2c7_gpio_device,
#else
	&s3c_device_i2c7,
#endif
#endif
#if !defined(CONFIG_TOUCHSCREEN_ACHRO4210_MT_P)
#if defined(CONFIG_S3C_DEV_I2C4)
	&s3c_device_i2c4,
#endif
#endif

#if defined(CONFIG_TOUCHSCREEN_EETI_EXC7200)
	&s3c_device_i2c_touch,
#endif

#if defined(CONFIG_TOUCHSCREEN_BT412B)
	&s3c_device_i2c_touch,
#else //jhkang insert
	&s3c_device_i2c6,
#endif

#endif	// #ifdef CONFIG_I2C_S3C2410

#ifdef CONFIG_SND_S3C64XX_SOC_I2S_V4
	&s5pv310_device_iis0,
#endif
#ifdef CONFIG_SND_S3C_SOC_PCM
	&s5pv310_device_pcm1,
#endif

#ifdef CONFIG_SND_SAMSUNG_SOC_SPDIF
	&s5pv310_device_spdif,
#endif
#ifdef CONFIG_MTD_NAND
	&s3c_device_nand,
#endif

#ifdef CONFIG_SND_S5P_RP
	&s5pv310_device_rp,
#endif

#ifdef CONFIG_S3C_DEV_HSMMC
	&s3c_device_hsmmc0,
#endif
#ifdef CONFIG_S3C_DEV_HSMMC1
	&s3c_device_hsmmc1,
#endif
#ifdef CONFIG_S3C_DEV_HSMMC2
	&s3c_device_hsmmc2,
#endif
#ifdef CONFIG_S3C_DEV_HSMMC3
	&s3c_device_hsmmc3,
#endif
#ifdef CONFIG_S5P_DEV_MSHC
	&s3c_device_mshci,
#endif

#ifdef CONFIG_VIDEO_TVOUT
	&s5p_device_tvout,
	&s5p_device_cec,
	&s5p_device_hpd,
#endif

#ifdef CONFIG_S3C2410_WATCHDOG
	&s3c_device_wdt,
#endif

#ifdef CONFIG_USB
	&s3c_device_usb_ehci,
	&s3c_device_usb_ohci,
#endif

#ifdef CONFIG_ANDROID_PMEM
	&pmem_device,
	&pmem_gpu1_device,
#endif

#ifdef CONFIG_VIDEO_FIMC
	&s3c_device_fimc0,
	&s3c_device_fimc1,
	&s3c_device_fimc2,
	&s3c_device_fimc3,
#ifdef CONFIG_VIDEO_FIMC_MIPI
	&s3c_device_csis0,
	&s3c_device_csis1,
#endif
#endif
#ifdef CONFIG_VIDEO_JPEG
	&s5p_device_jpeg,
#endif
#if defined(CONFIG_VIDEO_MFC50) || defined(CONFIG_VIDEO_MFC5X)
	&s5p_device_mfc,
#endif

#ifdef CONFIG_VIDEO_FIMG2D
	&s5p_device_fimg2d,
#endif

#ifdef CONFIG_USB_GADGET
	&s3c_device_usbgadget,
#endif
#ifdef CONFIG_USB_ANDROID_RNDIS
	&s3c_device_rndis,
#endif
#ifdef CONFIG_USB_ANDROID
	&s3c_device_android_usb,
	&s3c_device_usb_mass_storage,
#endif

#if defined(CONFIG_S3C64XX_DEV_SPI)
	&s5pv310_device_spi1,

#elif defined(CONFIG_SPI_S5PV310)
	&s3c_device_spi1,

#else //mtv350 
	&s3c_device_spi_gpio,
#endif

#ifdef CONFIG_SMSC911X
	&s5p_device_smsc911x,
#endif

#ifdef CONFIG_S5P_SYSMMU
	&s5p_device_sysmmu,
#endif

#ifdef CONFIG_S3C_DEV_GIB
	&s3c_device_gib,
#endif

#ifdef CONFIG_S3C_DEV_RTC
	&s3c_device_rtc,
#endif

	&s5p_device_ace,

#ifdef CONFIG_SATA_AHCI_PLATFORM
	&s5pv310_device_sata,
#endif

#ifdef CONFIG_DEV_THERMAL
	&s5p_device_tmu,
#endif

#ifdef CONFIG_BATTERY_ACHRO_DUMMY
	&achro_battery,
#endif

	&pmu_device,
};
/*[------------------------------------------------------------------------------------------------------]*/
#if defined(CONFIG_VIDEO_TVOUT)
static struct s5p_platform_hpd hdmi_hpd_data __initdata = {

};
static struct s5p_platform_cec hdmi_cec_data __initdata = {

};
#endif
/*[------------------------------------------------------------------------------------------------------]*/
#if defined(CONFIG_S5P_MEM_CMA)
static void __init s5pv310_reserve(void);
#endif
/*[------------------------------------------------------------------------------------------------------]*/
static void __init smdkc210_map_io(void)
{
	s5p_init_io(NULL, 0, S5P_VA_CHIPID);
	s3c24xx_init_clocks(24000000);
	s3c24xx_init_uarts(smdkc210_uartcfgs, ARRAY_SIZE(smdkc210_uartcfgs));

#ifdef CONFIG_MTD_NAND
	s3c_device_nand.name = "s5pv310-nand";
#endif
#if defined(CONFIG_S5P_MEM_CMA)
	s5pv310_reserve();
#elif defined(CONFIG_S5P_MEM_BOOTMEM)
	s5p_reserve_bootmem();
#endif
}
/*[------------------------------------------------------------------------------------------------------]*/
#ifdef CONFIG_MTD_NAND
// HISEOB : NAND Init Codes
void achro4210_nand_init(void)
{
	unsigned int *reg;
	unsigned int tmp;
	int i;
	//Xm0FCLE-> GPY2[0]/NF_CLE/OND_ADDRVALID
	s3c_gpio_cfgpin(S5PV310_GPY2(0), S3C_GPIO_SFN(2));
	s3c_gpio_setpull(S5PV310_GPY2(0), S3C_GPIO_PULL_NONE);

	//Xm0FALE-> GPY2[1]/NF_ALE/OND_SMCLK
	s3c_gpio_cfgpin(S5PV310_GPY2(1), S3C_GPIO_SFN(2));
	s3c_gpio_setpull(S5PV310_GPY2(1), S3C_GPIO_PULL_NONE);

	//Xm0FRnB0->GPY2[2]/NF_RnB[0]/OND_INT[0]
	s3c_gpio_cfgpin(S5PV310_GPY2(2), S3C_GPIO_SFN(2));
	s3c_gpio_setpull(S5PV310_GPY2(2), S3C_GPIO_PULL_NONE);	

	//Xm0CSn2->GPY0[3]/SROM_CSn[3]/NF_CSn[1]/OND_CSn[1]
	s3c_gpio_cfgpin(S5PV310_GPY0(3), S3C_GPIO_SFN(2));
	s3c_gpio_setpull(S5PV310_GPY0(3), S3C_GPIO_PULL_NONE);

	//Xm0OEn->GPY0[4]/EBI_Oen
	s3c_gpio_cfgpin(S5PV310_GPY0(4), S3C_GPIO_SFN(2));
	s3c_gpio_setpull(S5PV310_GPY0(4), S3C_GPIO_PULL_NONE);

	//Xm0WEn->GPY0[5]/EBI_Wen
	s3c_gpio_cfgpin(S5PV310_GPY0(5), S3C_GPIO_SFN(2));
	s3c_gpio_setpull(S5PV310_GPY0(5), S3C_GPIO_PULL_NONE);

	//Address/Data Bus ->  Xm0DATA0 ~ Xm0DATA7
	for(i=0; i < 8; i++)
	{
		s3c_gpio_cfgpin(S5PV310_GPY5(i), S3C_GPIO_SFN(2));
		s3c_gpio_setpull(S5PV310_GPY5(i), S3C_GPIO_PULL_NONE);
	}
	// HISEOB : NAND Controller Register Settings
	reg = ioremap_nocache(0x0CE00000, 0x8);
	tmp = __raw_readl(reg);
	tmp &= ~(0xF <<12 | 0xF << 8 | 0xF << 4 | 0xE << 0);
	tmp |= (0x7 << 12 | 0x7 << 8 | 0x7 << 4 | 0x2 << 0);
	__raw_writel(tmp, reg);
	tmp = __raw_readl(reg + 0x4);
	// HISEOB : Chip Selects MUST Disabled during INIT
	// to Prevent Data Corruption of SROM Controller
	tmp |= (1<<1 | 1<<2 | 1<<22 | 1<<23);
	__raw_writel(tmp, reg + 0x4);
	iounmap(reg);
}
#endif
/*[------------------------------------------------------------------------------------------------------]*/
static void __init smdkc210_machine_init(void)
{
	static void __iomem *s3c_rtc_base;

#if defined(CONFIG_SPI_S5PV310)
	struct clk *sclk = NULL;
	struct clk *prnt = NULL;
	struct device *spi1_dev = &s3c_device_spi1.dev;

#elif defined(CONFIG_S3C64XX_DEV_SPI)
	struct clk *sclk = NULL;
	struct clk *prnt = NULL;
	struct device *spi1_dev = &s5pv310_device_spi1.dev;
#endif	//#if defined(CONFIG_S3C64XX_DEV_SPI)



#ifdef CONFIG_ANDROID_PMEM
	android_pmem_set_platdata();
#endif
	s3c_pm_init();

#if defined(CONFIG_S5PV310_DEV_PD) && !defined(CONFIG_PM_RUNTIME)
	/*
	 * These power domains should be always on
	 * without runtime pm support.
	 */
	s5pv310_pd_enable(&s5pv310_device_pd[PD_MFC].dev);
	s5pv310_pd_enable(&s5pv310_device_pd[PD_G3D].dev);
	s5pv310_pd_enable(&s5pv310_device_pd[PD_LCD0].dev);
	s5pv310_pd_enable(&s5pv310_device_pd[PD_LCD1].dev);
	s5pv310_pd_enable(&s5pv310_device_pd[PD_CAM].dev);
	s5pv310_pd_enable(&s5pv310_device_pd[PD_TV].dev);
	s5pv310_pd_enable(&s5pv310_device_pd[PD_GPS].dev);
#endif

#ifdef CONFIG_I2C_S3C2410
	s3c_i2c0_set_platdata(NULL);
	i2c_register_board_info(0, i2c_devs0, ARRAY_SIZE(i2c_devs0));

#ifdef CONFIG_S3C_DEV_I2C1
	s3c_i2c1_set_platdata(NULL);
	i2c_register_board_info(1, i2c_devs1, ARRAY_SIZE(i2c_devs1));
#endif

#ifdef CONFIG_S3C_DEV_I2C2
	achro4210_i2c2_set_eint();
	s3c_i2c2_set_platdata(NULL);
	i2c_register_board_info(2, i2c_devs2, ARRAY_SIZE(i2c_devs2));
#endif
#if !defined(CONFIG_TOUCHSCREEN_ACHRO4210_MT_P)
#ifdef CONFIG_S3C_DEV_I2C4
	s3c_i2c4_set_platdata(NULL);
	i2c_register_board_info(4, i2c_devs4, ARRAY_SIZE(i2c_devs4));
#endif
#endif
#endif	// #ifdef CONFIG_I2C_S3C2410

#ifdef CONFIG_TOUCHSCREEN_EETI_EXC7200

	//TSP_SDA:jhkang
	if(gpio_request(S5PV310_GPL1(1), "GPL1"))printk("TOUCH SDA(GPL1.1) Port request error!!!\n");
	else	{
		s3c_gpio_setpull(S5PV310_GPL1(1), S3C_GPIO_PULL_UP);
		gpio_direction_input(S5PV310_GPL1(1));
		gpio_free(S5PV310_GPL1(1));
	}
	//TSP_SCL:jhkang
	if(gpio_request(S5PV310_GPL1(0), "GPL1"))printk("TOUCH SDA(GPL1.0)  Port request error!!!\n");
	else	{
		s3c_gpio_setpull(S5PV310_GPL1(0), S3C_GPIO_PULL_UP);
		gpio_direction_input(S5PV310_GPL1(0));
		gpio_free(S5PV310_GPL1(0));
	}
	//TSP_INT:jhkang	
	if (gpio_request(S5PV310_GPX1(0), "GPX1"))printk(KERN_ERR "#### failed to request GPX1_0 ####\n");
	else	{
		s3c_gpio_setpull(S5PV310_GPX1(0), S3C_GPIO_PULL_NONE);
		gpio_direction_input(S5PV310_GPX1(0));
		gpio_free(S5PV310_GPX1(0));
	}	
	//s3c_i2c4_set_platdata(NULL);
	i2c_register_board_info(8, i2c_devs8, ARRAY_SIZE(i2c_devs8));
#endif	

#ifdef CONFIG_TOUCHSCREEN_BT412B 
	achro4210_i2c8_gpio_init();
	i2c_register_board_info(8, i2c_devs8, ARRAY_SIZE(i2c_devs8));

#endif	

	s3c_rtc_base = ioremap(0x10070000, 0xff + 1);
	printk("s3c_rtc_base: %x\n", s3c_rtc_base);

	if (s3c_rtc_base == NULL) {
		printk("Error: no memory mapping for rtc\n");
	}
	__raw_writel(0x200, s3c_rtc_base + 0x40);
	iounmap(s3c_rtc_base);

	if(gpio_request(S5PV310_GPE3(0), "GPE3"))	
		printk(KERN_INFO "[%s:%d] fail GPE3(0)\n", __FUNCTION__, __LINE__);
	s3c_gpio_setpull(S5PV310_GPE3(0), S3C_GPIO_PULL_NONE);
	gpio_direction_output(S5PV310_GPE3(0), 1); 

#ifdef CONFIG_VIDEO_FIMC
	/* fimc */
	s3c_fimc0_set_platdata(&fimc_plat);
	s3c_fimc1_set_platdata(&fimc_plat);
	s3c_fimc2_set_platdata(&fimc_plat);
	s3c_fimc3_set_platdata(&fimc_plat);
#ifdef CONFIG_ITU_A
	smdkv310_cam0_reset(1);
#endif
#ifdef CONFIG_ITU_B
	smdkv310_cam1_reset(1);
#endif
#ifdef CONFIG_VIDEO_FIMC_MIPI
	s3c_csis0_set_platdata(NULL);
	s3c_csis1_set_platdata(NULL);
	smdkv310_cam1_reset(1);
	smdkv310_cam1_reset(1);
#endif
#endif

#ifdef CONFIG_VIDEO_MFC5X
#ifdef CONFIG_S5PV310_DEV_PD
	s5p_device_mfc.dev.parent = &s5pv310_device_pd[PD_MFC].dev;
#endif
#endif

#ifdef CONFIG_VIDEO_FIMG2D
	s5p_fimg2d_set_platdata(&fimg2d_data);
#ifdef CONFIG_S5PV310_DEV_PD
	s5p_device_fimg2d.dev.parent = &s5pv310_device_pd[PD_LCD0].dev;
#endif
#endif
#ifdef CONFIG_S3C_DEV_HSMMC
	s3c_sdhci0_set_platdata(&smdkc210_hsmmc0_pdata);
#endif
#ifdef CONFIG_S3C_DEV_HSMMC1
	s3c_sdhci1_set_platdata(&smdkc210_hsmmc1_pdata);
#endif
#ifdef CONFIG_S3C_DEV_HSMMC2
	s3c_sdhci2_set_platdata(&smdkc210_hsmmc2_pdata);
#endif
#ifdef CONFIG_S3C_DEV_HSMMC3
	s3c_sdhci3_set_platdata(&smdkc210_hsmmc3_pdata);
#endif
#ifdef CONFIG_S5P_DEV_MSHC
	s3c_mshci_set_platdata(&smdkc210_mshc_pdata);
#endif

#ifdef CONFIG_DEV_THERMAL
	s5p_tmu_set_platdata(NULL);
#endif

#if defined(CONFIG_VIDEO_TVOUT)
	s5p_hdmi_hpd_set_platdata(&hdmi_hpd_data);
	s5p_hdmi_cec_set_platdata(&hdmi_cec_data);

#ifdef CONFIG_S5PV310_DEV_PD
	s5p_device_tvout.dev.parent = &s5pv310_device_pd[PD_TV].dev;
#endif
#endif

#ifdef CONFIG_S5PV310_DEV_PD
#ifdef CONFIG_FB_S3C
	s3c_device_fb.dev.parent = &s5pv310_device_pd[PD_LCD0].dev;
#endif
#endif

#ifdef CONFIG_S5PV310_DEV_PD
#ifdef CONFIG_VIDEO_FIMC
	s3c_device_fimc0.dev.parent = &s5pv310_device_pd[PD_CAM].dev;
	s3c_device_fimc1.dev.parent = &s5pv310_device_pd[PD_CAM].dev;
	s3c_device_fimc2.dev.parent = &s5pv310_device_pd[PD_CAM].dev;
	s3c_device_fimc3.dev.parent = &s5pv310_device_pd[PD_CAM].dev;
#endif
#endif

	platform_add_devices(smdkc210_devices, ARRAY_SIZE(smdkc210_devices));

#if defined(CONFIG_S3C64XX_DEV_SPI) || defined(CONFIG_SPI_S5PV310)
	sclk = clk_get(spi1_dev, "sclk_spi");
	if (IS_ERR(sclk))	{
		dev_err(spi1_dev, "failed to get sclk for SPI-1\n");
	}

	prnt = clk_get(spi1_dev, "mout_mpll");
	if (IS_ERR(prnt))	{
		dev_err(spi1_dev, "failed to get prnt\n");
	}
	clk_set_parent(sclk, prnt);
	clk_put(prnt);

	if (!gpio_request(S5PV310_GPB(5), "SPI_CS1")) {
		gpio_direction_output(S5PV310_GPB(5), 1);
		s3c_gpio_cfgpin(S5PV310_GPB(5), S3C_GPIO_SFN(1));
		s3c_gpio_setpull(S5PV310_GPB(5), S3C_GPIO_PULL_UP);

#if defined(CONFIG_SPI_S5PV310)
		s3cspi_set_slaves(BUSNUM(1), ARRAY_SIZE(s3c_slv_pdata_1), s3c_slv_pdata_1);
#else
		s5pv310_spi_set_info(1, S5PV310_SPI_SRCCLK_SCLK, ARRAY_SIZE(spi1_csi));
#endif
	}
#if defined(CONFIG_SPI_S5PV310)
	spi_register_board_info(s3c_spi_devs, ARRAY_SIZE(s3c_spi_devs));
#else
	spi_register_board_info(spi1_board_info, ARRAY_SIZE(spi1_board_info));
#endif

#if defined(CONFIG_TDMB_MTV818)
	// TDMB MTV818 INT
	if(gpio_request(S5PV310_GPX0(2), "MTV818 INT"))	printk("MTV818 INT(GPX0.2) Port request error!!!\n");
	else	{
		s3c_gpio_setpull(S5PV310_GPX0(2), S3C_GPIO_PULL_NONE);
		gpio_direction_input(S5PV310_GPX0(2));
		gpio_free(S5PV310_GPX0(2));
	}

	// TDMB Enable
	if(gpio_request(S5PV310_GPD0(3), "MTV818 Reset"))		printk("MTV818 Reset(GPD0.3) Port request error!!!\n");
	else	{
		s3c_gpio_setpull(S5PV310_GPD0(3), S3C_GPIO_PULL_NONE);
		gpio_direction_output(S5PV310_GPD0(3), 0);
		gpio_free(S5PV310_GPD0(3));
	}
#endif

#endif	//#if defined(CONFIG_S3C64XX_DEV_SPI)

#if defined(CONFIG_BOARD_ACHRO4210)
	pm_power_off = achro4210_power_off;

	// PS_HOLD Enable (LOW-> HIGH)
	(*(unsigned long *)(S5PV310_VA_PMU + 0x330C)) = 0x5300; 

#if defined(CONFIG_FB_S3C_TRULY1280)
	// LCD Disable
	if(gpio_request(S5PV310_GPD0(1), "LCD_EN"))		printk("LCD_EN(GPD0.1) Port request error!!!\n");
	else	{
		gpio_direction_output(S5PV310_GPD0(1), 1); /* LCD_EN High(Invert) : Lcd Disable */
		gpio_free(S5PV310_GPD0(1));
	}
#endif

#if defined(CONFIG_BACKLIGHT_PWM)
	// PWM Port Init
	if(gpio_request(S5PV310_GPD0(2), "PWM2"))	printk("PWM0(GPD0.2) Port request error!!!\n");
	else	{
		s3c_gpio_setpull(S5PV310_GPD0(2), S3C_GPIO_PULL_NONE);
		gpio_direction_output(S5PV310_GPD0(2), 1); 
	}
	achro4210_backlight_register();
#endif

#if defined(CONFIG_FB_S3C_TRULY1280)
	s3cfb_set_platdata(NULL);
	s3cfb_set_platdata(&truly1280_data);
#elif defined(CONFIG_FB_S3C_A070VW08)
	s3cfb_set_platdata(NULL);
	s3cfb_set_platdata(&a070vw08_data);	
#elif defined(CONFIG_FB_S3C_LMS700KF06)	
	s3cfb_set_platdata(NULL);
	s3cfb_set_platdata(&lms700kf06_data);	
#endif
#if defined(CONFIG_FB_S3C_HDMI_UI_FB)
	s3cfb_set_platdata(NULL);
	s3cfb_set_platdata(&hdmi_ui_fb_data);
#endif

#if defined(CONFIG_ACHRO_HEADSET)
	platform_device_register(&achro_device_jack);
#endif
#ifdef CONFIG_MTD_NAND
	achro4210_nand_init();
#endif
#ifdef CONFIG_SMSC911X
	achro4210_smsc911x_init();
#endif
#endif	//#if defined(CONFIG_BOARD_ACHRO4210)
}
/*[------------------------------------------------------------------------------------------------------]*/
#if defined(CONFIG_S5P_MEM_CMA)
static void __init s5pv310_reserve(void)
{
	static struct cma_region regions[] = {
#ifdef CONFIG_ANDROID_PMEM_MEMSIZE_PMEM
		{
			.name = "pmem",
			.size = CONFIG_ANDROID_PMEM_MEMSIZE_PMEM * SZ_1K,
			.start = 0,
		},
#endif
#ifdef CONFIG_ANDROID_PMEM_MEMSIZE_PMEM_GPU1
		{
			.name = "pmem_gpu1",
			.size = CONFIG_ANDROID_PMEM_MEMSIZE_PMEM_GPU1 * SZ_1K,
			.start = 0,
		},
#endif
#ifdef CONFIG_VIDEO_SAMSUNG_MEMSIZE_FIMD
		{
			.name = "fimd",
			.size = CONFIG_VIDEO_SAMSUNG_MEMSIZE_FIMD * SZ_1K,
			.start = 0
		},
#endif
#ifdef CONFIG_VIDEO_SAMSUNG_MEMSIZE_MFC
		{
			.name = "mfc",
			.size = CONFIG_VIDEO_SAMSUNG_MEMSIZE_MFC * SZ_1K,
			.start = 0,
		},
#endif
#ifdef CONFIG_VIDEO_SAMSUNG_MEMSIZE_MFC0
		{
			.name = "mfc0",
			.size = CONFIG_VIDEO_SAMSUNG_MEMSIZE_MFC0 * SZ_1K,
			{
				.alignment = 1 << 17,
			},
			.start = 0,
		},
#endif
#ifdef CONFIG_VIDEO_SAMSUNG_MEMSIZE_MFC1
		{
			.name = "mfc1",
			.size = CONFIG_VIDEO_SAMSUNG_MEMSIZE_MFC1 * SZ_1K,
			{
				.alignment = 1 << 17,
			},
			.start = 0,
		},
#endif
#ifdef CONFIG_VIDEO_SAMSUNG_MEMSIZE_FIMC0
		{
			.name = "fimc0",
			.size = CONFIG_VIDEO_SAMSUNG_MEMSIZE_FIMC0 * SZ_1K,
			.start = 0
		},
#endif
#ifdef CONFIG_VIDEO_SAMSUNG_MEMSIZE_FIMC1
		{
			.name = "fimc1",
			.size = CONFIG_VIDEO_SAMSUNG_MEMSIZE_FIMC1 * SZ_1K,
			.start = 0
		},
#endif
#ifdef CONFIG_VIDEO_SAMSUNG_MEMSIZE_FIMC2
		{
			.name = "fimc2",
			.size = CONFIG_VIDEO_SAMSUNG_MEMSIZE_FIMC2 * SZ_1K,
			.start = 0,
		},
#endif
#ifdef CONFIG_VIDEO_SAMSUNG_MEMSIZE_FIMC3
		{
			.name = "fimc3",
			.size = CONFIG_VIDEO_SAMSUNG_MEMSIZE_FIMC3 * SZ_1K,
			.start = 0,
		},
#endif
#ifdef CONFIG_AUDIO_SAMSUNG_MEMSIZE_SRP
		{
			.name = "srp",
			.size = CONFIG_AUDIO_SAMSUNG_MEMSIZE_SRP * SZ_1K,
			.start = 0,
		},
#endif
#ifdef CONFIG_VIDEO_SAMSUNG_MEMSIZE_JPEG
		{
			.name = "jpeg",
			.size = CONFIG_VIDEO_SAMSUNG_MEMSIZE_JPEG * SZ_1K,
			.start = 0,
		},
#endif
#ifdef CONFIG_VIDEO_SAMSUNG_MEMSIZE_FIMG2D
		{
			.name = "fimg2d",
			.size = CONFIG_VIDEO_SAMSUNG_MEMSIZE_FIMG2D * SZ_1K,
			.start = 0,
		},
#endif
		{}
	};

	static const char map[] __initconst =
		"android_pmem.0=pmem;android_pmem.1=pmem_gpu1;s3cfb.0=fimd;"
		"s3c-fimc.0=fimc0;s3c-fimc.1=fimc1;s3c-fimc.2=fimc2;"
		"s3c-fimc.3=fimc3;s3c-mfc=mfc,mfc0,mfc1;s5p-rp=srp;"
		"s5p-jpeg=jpeg;"
		"s5p-fimg2d=fimg2d";
	int i = 0;
	unsigned int bank0_end = meminfo.bank[4].start +
		meminfo.bank[4].size;

	printk(KERN_ERR "mem infor: bank4 start-> 0x%x, bank4 size-> 0x%x\n",\
			(int)meminfo.bank[4].start, (int)meminfo.bank[4].size);
	for (i = ARRAY_SIZE(regions) - 1; i >= 0; i--) {
		if (!regions[i].name)
			continue;
		if (regions[i].start == 0) {
			regions[i].start = bank0_end - regions[i].size;
			bank0_end = regions[i].start;
		}
		printk(KERN_ERR "CMA reserve : %s, addr is 0x%x, size is 0x%x\n",
				regions[i].name, regions[i].start, regions[i].size);
	}

	cma_set_defaults(regions, map);
	cma_early_regions_reserve(NULL);
}
#endif
/*[------------------------------------------------------------------------------------------------------]*/
MACHINE_START(SMDKC210, "ACHRO4210")
.phys_io		= S3C_PA_UART & 0xfff00000,
	.io_pg_offst	= (((u32)S3C_VA_UART) >> 18) & 0xfffc,
	.boot_params	= S5P_PA_SDRAM + 0x100,
	.init_irq		= s5pv310_init_irq,
	.map_io			= smdkc210_map_io,
	.init_machine	= smdkc210_machine_init,
	.timer			= &s5pv310_timer,
	MACHINE_END
