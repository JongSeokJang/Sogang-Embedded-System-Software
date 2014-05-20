/* linux/arch/arm/mach-s5pv310/setup-fb.c
 *
 * Copyright (c) 2010 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com/
 *
 * Base FIMD controller configuration
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/gpio.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/types.h>
#include <plat/clock.h>
#include <plat/gpio-cfg.h>
#include <mach/regs-clock.h>
#include <mach/regs-gpio.h>
#include <linux/io.h>
#include <mach/map.h>
#include <mach/gpio.h>
/* #include <mach/pd.h> */

extern	void 	SYSTEM_POWER_CONTROL	(int power, int val);	// achro4210-sysfs.c

#if defined(CONFIG_FB_S3C_TRULY1280) || defined(CONFIG_FB_S3C_HDMI_UI_FB) ||defined(CONFIG_FB_S3C_A070VW08) ||defined(CONFIG_FB_S3C_LMS700KF06)

struct platform_device; /* don't need the contents */

void s3cfb_cfg_gpio(struct platform_device *pdev)
{
	int i;
	u32 reg;

	for (i = 0; i < 8; i++) {
		s3c_gpio_cfgpin(S5PV310_GPF0(i), S3C_GPIO_SFN(2));
		s3c_gpio_setpull(S5PV310_GPF0(i), S3C_GPIO_PULL_NONE);
		s5p_gpio_set_drvstr(S5PV310_GPF0(i), S5P_GPIO_DRVSTR_LV1);
	}

	for (i = 0; i < 8; i++) {
		s3c_gpio_cfgpin(S5PV310_GPF1(i), S3C_GPIO_SFN(2));
		s3c_gpio_setpull(S5PV310_GPF1(i), S3C_GPIO_PULL_NONE);
		s5p_gpio_set_drvstr(S5PV310_GPF1(i), S5P_GPIO_DRVSTR_LV1);
	}

	for (i = 0; i < 8; i++) {
		s3c_gpio_cfgpin(S5PV310_GPF2(i), S3C_GPIO_SFN(2));
		s3c_gpio_setpull(S5PV310_GPF2(i), S3C_GPIO_PULL_NONE);
		s5p_gpio_set_drvstr(S5PV310_GPF2(i), S5P_GPIO_DRVSTR_LV1);
	}

	for (i = 0; i < 4; i++) {
		s3c_gpio_cfgpin(S5PV310_GPF3(i), S3C_GPIO_SFN(2));
		s3c_gpio_setpull(S5PV310_GPF3(i), S3C_GPIO_PULL_NONE);
		s5p_gpio_set_drvstr(S5PV310_GPF3(i), S5P_GPIO_DRVSTR_LV1);
	}

	/* Set FIMD0 bypass */
	reg = __raw_readl(S3C_VA_SYS + 0x0210); //addr => 0x1001_0210
	reg |= (1<<1);
	__raw_writel(reg, S3C_VA_SYS + 0x0210);


}

int s3cfb_clk_on(struct platform_device *pdev, struct clk **s3cfb_clk)
{
	struct clk *sclk = NULL;
	struct clk *clk_source = NULL;
	struct clk *lcd_clk = NULL;

	u32 rate = 0;

	lcd_clk = clk_get(NULL, "fimd0"); /*  CLOCK GATE IP ENABLE */
	if (IS_ERR(lcd_clk)) {
		dev_err(&pdev->dev, "failed to get ip clk for fimd0\n");
		goto err_clk0;
	}
	clk_enable(lcd_clk);
	clk_put(lcd_clk);

	sclk = clk_get(&pdev->dev, "sclk_fimd");
	if (IS_ERR(sclk)) {
		dev_err(&pdev->dev, "failed to get sclk for fimd\n");
		goto err_clk1;
	}
	
	clk_source = clk_get(&pdev->dev, "mout_mpll");
	if (IS_ERR(clk_source)) {
		dev_err(&pdev->dev, "failed to get clock source\n");
		goto err_clk2;
	}

	clk_set_parent(sclk, clk_source);

	rate = clk_round_rate(sclk,  160000000);

	dev_info(&pdev->dev, "try to setting fimd sclk rate to %d\n", rate);

	if (!rate)
		rate = 160000000;

	clk_set_rate(sclk, rate);

	dev_info(&pdev->dev, "setting fimd sclk rate to %d\n", rate);

	clk_put(clk_source);

	clk_enable(sclk);

	*s3cfb_clk = sclk;

	return 0;

err_clk2:
	clk_put(clk_source);
err_clk1:
	clk_put(sclk);
err_clk0:
	clk_put(lcd_clk);

	return -EINVAL;
}

int s3cfb_clk_off(struct platform_device *pdev, struct clk **clk)
{
	struct clk *lcd_clk = NULL;

	lcd_clk = clk_get(NULL, "fimd0"); /*  CLOCK GATE IP ENABLE */
	if (IS_ERR(lcd_clk)) {
		printk(KERN_ERR "failed to get ip clk for fimd0\n");
		goto err_clk0;
	}
	clk_disable(lcd_clk);
	clk_put(lcd_clk);

	clk_disable(*clk);
	clk_put(*clk);

	*clk = NULL;

	return 0;
err_clk0:
	clk_put(lcd_clk);

	return -EINVAL;
}

void s3cfb_get_clk_name(char *clk_name)
{
	strcpy(clk_name, "sclk_fimd");
}




int s3cfb_backlight_on(struct platform_device *pdev)
{

#if defined(CONFIG_FB_S3C_TRULY1280)
	int err;

	err = gpio_request(S5PV310_GPD0(2), "PWM2");
	if (err) {
		printk(KERN_ERR "LCD Backlight PWM Control(GPD0.2)\n");
		return err;
	}
	s3c_gpio_cfgpin(S5PV310_GPD0(2), S3C_GPIO_SFN(2));
	gpio_free(S5PV310_GPD0(2));


	err = gpio_request(S5PV310_GPD0(1), "LCD_EN");
	if (err) {
		printk(KERN_ERR "LCD Enable Control(GPD0.1)\n");
		return err;
	}
	gpio_direction_output(S5PV310_GPD0(1), 0); /* LCD_EN Low(Invert) */
	gpio_free(S5PV310_GPD0(1));

	SYSTEM_POWER_CONTROL(2, 0);	// VLED 12V ON
#endif	

#if defined(CONFIG_FB_S3C_A070VW08)
	int err;

	err = gpio_request(S5PV310_GPD0(2), "PWM2");
	if (err) {
		printk(KERN_ERR "LCD Backlight PWM Control(GPD0.2)\n");
		return err;
	}
	s3c_gpio_cfgpin(S5PV310_GPD0(2), S3C_GPIO_SFN(2));
	gpio_free(S5PV310_GPD0(2));

#if defined(CONFIG_FB_S3C_A070VW08)
	SYSTEM_POWER_CONTROL(2, 0);	// VLED 12V ON
#endif	 
#endif	 

#if defined(CONFIG_FB_S3C_LMS700KF06)
	printk(KERN_INFO "[%s:%d]\n", __FUNCTION__, __LINE__);
	mdelay(100);
	gpio_set_value(S5PV310_GPD0(2), 1);
#endif	 

	return 0;
}

int s3cfb_backlight_off(struct platform_device *pdev)
{

#if defined(CONFIG_FB_S3C_TRULY1280)
	int err;

	err = gpio_request(S5PV310_GPD0(2), "PWM2");
	if (err) {
		printk(KERN_ERR "LCD Backlight PWM Control(GPD0.2)\n");
		return err;
	}

	gpio_direction_output(S5PV310_GPD0(2), 1); /* BL pwm Low(Invert) */
	gpio_free(S5PV310_GPD0(2));

	err = gpio_request(S5PV310_GPD0(1), "LCD_EN");
	if (err) {
		printk(KERN_ERR "LCD Enable Control(GPD0.1)\n");
		return err;
	}
	gpio_direction_output(S5PV310_GPD0(1), 1); /* LCD_EN High(Invert) */
	gpio_free(S5PV310_GPD0(1));

	SYSTEM_POWER_CONTROL(2, 1);	// VLED 12V OFF
#endif

#if defined(CONFIG_FB_S3C_A070VW08)
	int err;
	
	err = gpio_request(S5PV310_GPD0(2), "PWM2");
	if (err) {
		printk(KERN_ERR "LCD Backlight PWM Control(GPD0.2)\n");
		return err;
	}
	gpio_direction_output(S5PV310_GPD0(2), 1); /* BL pwm Low(Invert) */
	gpio_free(S5PV310_GPD0(2));	

#if defined(CONFIG_FB_S3C_A070VW08)
	SYSTEM_POWER_CONTROL(2, 1);	// VLED 12V OFF
#endif
#endif

	#if defined(CONFIG_FB_S3C_LMS700KF06)
	printk(KERN_INFO "[%s:%d]\n", __FUNCTION__, __LINE__);
	gpio_set_value(S5PV310_GPD0(2), 0);
	mdelay(100);
	#endif

	return 0;
}

int s3cfb_lcd_on(struct platform_device *pdev)
{
	int err;

#if defined(CONFIG_FB_S3C_TRULY1280)
	err = gpio_request(S5PV310_GPL0(0), "LCD_VDDA_EN");
	if (err) {
		printk(KERN_ERR "Color Enable Control(GPL0.0)\n");
		return err;
	}
	gpio_direction_output(S5PV310_GPL0(0), 0); 
	gpio_free(S5PV310_GPL0(0));

	err = gpio_request(S5PV310_GPL0(1), "LCD_CABC_EN");
	if (err) {
		printk(KERN_ERR "CABC Enable Control(GPL0.1)\n");
		return err;
	}
	gpio_direction_output(S5PV310_GPL0(1), 0); 
	gpio_free(S5PV310_GPL0(1));


	err = gpio_request(S5PV310_GPL0(2), "LVDS_PWR_DWN_B");
	if (err) {
		printk(KERN_ERR "LVDS Enable Control(GPL0.2)\n");
		return err;
	}
	gpio_direction_output(S5PV310_GPL0(2), 1); 
	gpio_free(S5PV310_GPL0(2));
#endif

#if defined(CONFIG_FB_S3C_A070VW08)
	err = gpio_request(S5PV310_GPL0(5), "LCD_EN");
	if (err) {
		printk(KERN_ERR "LCD Enable Control(GPL0.5)\n");
		return err;
	}
	gpio_direction_output(S5PV310_GPL0(5), 1); /* LCD_EN */
	gpio_free(S5PV310_GPL0(5));
#endif

#if defined(CONFIG_FB_S3C_LMS700KF06)	
	err = gpio_request(S5PV310_GPL0(1), "LCD_BL_EN");
	if (err) {
		printk(KERN_ERR "LCD BL Enable Control(GPL0.1)\n");
		return err;
	}
	gpio_direction_output(S5PV310_GPL0(1), 1); /* LCD_BL_EN*/
	gpio_free(S5PV310_GPL0(1));
#endif

	return 0;
}

int s3cfb_lcd_off(struct platform_device *pdev)
{
	int err;

#if defined(CONFIG_FB_S3C_TRULY1280)
	err = gpio_request(S5PV310_GPL0(2), "LVDS_PWR_DWN_B");
	if (err) {
		printk(KERN_ERR "LVDS Enable Control(GPL0.2)\n");
		return err;
	}
	gpio_direction_output(S5PV310_GPL0(2), 0); 
	gpio_free(S5PV310_GPL0(2));
#endif

#if defined(CONFIG_FB_S3C_A070VW08)
	err = gpio_request(S5PV310_GPL0(5), "LCD_EN");
	if (err) {
		printk(KERN_ERR "LCD Enable Control(GPL0.5)\n");
		return err;
	}
	gpio_direction_output(S5PV310_GPL0(5), 0); /* LCD_EN */
	gpio_free(S5PV310_GPL0(5));
#endif

#if defined(CONFIG_FB_S3C_LMS700KF06)	
	err = gpio_request(S5PV310_GPL0(1), "LCD_BL_EN");
	if (err) {
		printk(KERN_ERR "LCD BL EN Control(GPL0,1)\n");
		return err;
	}
	gpio_direction_output(S5PV310_GPL0(1), 0); /* LCD_BL_EN*/
	gpio_free(S5PV310_GPL0(1));
#endif
	return 0;
}

#endif
