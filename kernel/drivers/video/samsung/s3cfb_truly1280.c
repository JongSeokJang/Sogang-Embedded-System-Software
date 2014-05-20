/* linux/drivers/video/samsung/s3cfb_truly1280.c
 *
 * TRULY Display TFT1280800 10.1" WSVGA Display Panel Support
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include "s3cfb.h"

static struct s3cfb_lcd truly1280 = {
	.width = 1280,
	.height = 800,
	.bpp = 24,
	.freq = 60,

#if 0
	.timing = {
		.h_fp = 56,
		.h_bp = 76,
		.h_sw = 28,
		.v_fp = 3,
		.v_fpe = 2,
		.v_bp = 14,
		.v_bpe = 2,
		.v_sw = 6,
	},
#endif

	.timing = {
		.h_fp = 40,
		.h_bp = 40,
		.h_sw = 20,
		.v_fp = 2,
		.v_fpe = 1,
		.v_bp = 2,
		.v_bpe = 1,
		.v_sw = 1,
	},
	
	.polarity = {
		.rise_vclk = 1,
		.inv_hsync = 1,
		.inv_vsync = 1,
		.inv_vden = 0,
	},
};

/* name should be fixed as 's3cfb_set_lcd_info' */
void s3cfb_set_lcd_info(struct s3cfb_global *ctrl)
{
	truly1280.init_ldi	= NULL;
	
	ctrl->lcd = &truly1280;
}

