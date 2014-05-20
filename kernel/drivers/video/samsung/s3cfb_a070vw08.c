/* linux/drivers/video/samsung/s3cfb_a070vw08.c
 *
 * Copyright (c) 2010 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com/
 *
 * A070VW08 7.0" WVGA Landscape LCD module driver for the SMDK
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include "s3cfb.h"
static struct s3cfb_lcd a070vw08 = {
	.width	= 800,
	.height	= 480,
	.bpp	= 32,
	.freq	= 60,

	.timing = {
		.h_fp	= 210,
		.h_bp	= 46,
		.h_sw	= 10,
		.v_fp	= 22,
		.v_fpe	= 1,
		.v_bp	= 23,
		.v_bpe	= 1,
		.v_sw	= 8,
/*
                .h_fp   = 128,
                .h_bp   = 128,
                .h_sw   = 256,
                .v_fp   = 22,
                .v_fpe  = 1,
                .v_bp   = 21,
                .v_bpe  = 1,
                .v_sw   = 45,
*/

	},

	.polarity = {
		.rise_vclk	= 0,
		.inv_hsync	= 1,
		.inv_vsync	= 1,
		.inv_vden	= 0,
	},
};	

/* name should be fixed as 's3cfb_set_lcd_info' */
void s3cfb_set_lcd_info(struct s3cfb_global *ctrl)
{
	a070vw08.init_ldi = NULL;
	ctrl->lcd = &a070vw08;
}
