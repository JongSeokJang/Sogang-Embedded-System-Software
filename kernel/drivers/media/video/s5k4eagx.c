/* linux/drivers/media/video/s5k4eagx.c
 *
 * Copyright (c) 2010 Hardkernel Co., Ltd.
 * 		http://www.hardkernel.com/
 *
 * Driver for S5K4EAGX (SXGA camera) from Samsung Electronics
 * 1/4" 5Mp CMOS Image Sensor SoC with an Embedded Image Processor
 * supporting PVI
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/version.h>
#include <media/v4l2-device.h>
#include <media/v4l2-subdev.h>
#include <media/v4l2-i2c-drv.h>
#include <media/s5k4eagx_platform.h>
#include <mach/gpio.h>
#include <linux/slab.h>

#ifdef CONFIG_VIDEO_SAMSUNG_V4L2
#include <linux/videodev2_samsung.h>
#endif

#include "s5k4eagx.h"

#define S5K4EAGX_DRIVER_NAME	"S5K4EAGX"

/* Default resolution & pixelformat. plz ref s5k4eagx_platform.h */
#define DEFAULT_RES		WVGA	/* Index of resoultion */
#define DEFAUT_FPS_INDEX	S5K4EAGX_15FPS
#define DEFAULT_FMT		V4L2_PIX_FMT_UYVY	/* YUV422 */
#define SENSOR_JPEG_SNAPSHOT_MEMSIZE	1024*1024*6

/*
 * Specification
 * Parallel : ITU-R. 656/601 YUV422, RGB565, RGB888 (Up to VGA), RAW10
 * Serial : MIPI CSI2 (single lane) YUV422, RGB565, RGB888 (Up to VGA), RAW10
 * Resolution : 1280 (H) x 1024 (V)
 * Image control : Brightness, Contrast, Saturation, Sharpness, Glamour
 * Effect : Mono, Negative, Sepia, Aqua, Sketch
 * FPS : 15fps @full resolution, 30fps @VGA, 24fps @720p
 * Max. pixel clock frequency : 48MHz(upto)
 * Internal PLL (6MHz to 27MHz input frequency)
 */

/* Camera functional setting values configured by user concept */
struct s5k4eagx_userset {
	signed int exposure_bias;	/* V4L2_CID_EXPOSURE */
	unsigned int ae_lock;
	unsigned int awb_lock;
	unsigned int auto_wb;	/* V4L2_CID_AUTO_WHITE_BALANCE */
	unsigned int manual_wb;	/* V4L2_CID_WHITE_BALANCE_PRESET */
	unsigned int wb_temp;	/* V4L2_CID_WHITE_BALANCE_TEMPERATURE */
	unsigned int effect;	/* Color FX (AKA Color tone) */
	unsigned int contrast;	/* V4L2_CID_CONTRAST */
	unsigned int saturation;	/* V4L2_CID_SATURATION */
	unsigned int sharpness;		/* V4L2_CID_SHARPNESS */
	unsigned int glamour;
};
struct s5k4eagx_jpeg_param {
	u32 enable;
	u32 quality;
	u32 main_size;		/* Main JPEG file size */
	u32 thumb_size;		/* Thumbnail file size */
	u32 main_offset;
	u32 thumb_offset;
	u32 postview_offset;
};

struct s5k4eagx_state {
	struct s5k4eagx_platform_data *pdata;
	struct v4l2_subdev sd;
	struct v4l2_pix_format pix;
	struct v4l2_fract timeperframe;
	struct s5k4eagx_userset userset;
	struct s5k4eagx_jpeg_param jpeg;
	int freq;	/* MCLK in KHz */
	int is_mipi;
	int isize;
	int ver;
	int fps;
};


int g_alive5=0;
int g_s5k4eagx_width=640;
int g_s5k4eagx_height=480;


static inline struct s5k4eagx_state *to_state(struct v4l2_subdev *sd)
{
	return container_of(sd, struct s5k4eagx_state, sd);
}

/*
 * S5K4EAGX register structure : 2bytes address, 2bytes value
 * retry on write failure up-to 5 times
 */

 
static inline int s5k4eagx_write(struct v4l2_subdev *sd, u16 addr, u16 val)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct i2c_msg msg[1];
	unsigned char reg[4];
	int err = 0;
	int retry = 0;


	if (!client->adapter)
		return -ENODEV;

again:
	msg->addr = client->addr;
	msg->flags = 0;
	msg->len = 4;
	msg->buf = reg;

	reg[0] = addr >> 8;
	reg[1] = addr & 0xff;
	reg[2] = val >> 8;
	reg[3] = val & 0xff;

	err = i2c_transfer(client->adapter, msg, 1);
	if (err >= 0)
		return err;	/* Returns here on success */

	/* abnormal case: retry 5 times */
	if (retry < 5) {
		dev_err(&client->dev, "%s: address: 0x%02x%02x, " \
			"value: 0x%02x%02x\n", __func__, \
			reg[0], reg[1], reg[2], reg[3]);
		retry++;
		goto again;
	}

	return err;
}

static int s5k4eagx_i2c_write(struct v4l2_subdev *sd, unsigned char i2c_data[],
				unsigned char length)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	unsigned char buf[length], i;
	struct i2c_msg msg = {client->addr, 0, length, buf};

	for (i = 0; i < length; i++)
		buf[i] = i2c_data[i];

	return i2c_transfer(client->adapter, &msg, 1) == 1 ? 0 : -EIO;
}

static const char *s5k4eagx_querymenu_wb_preset[] = {
	"WB Tungsten", "WB Fluorescent", "WB sunny", "WB cloudy", NULL
};

static const char *s5k4eagx_querymenu_effect_mode[] = {
	"Effect Sepia", "Effect Aqua", "Effect Monochrome",
	"Effect Negative", "Effect Sketch", NULL
};

static const char *s5k4eagx_querymenu_ev_bias_mode[] = {
	"-3EV",	"-2,1/2EV", "-2EV", "-1,1/2EV",
	"-1EV", "-1/2EV", "0", "1/2EV",
	"1EV", "1,1/2EV", "2EV", "2,1/2EV",
	"3EV", NULL
};

static struct v4l2_queryctrl s5k4eagx_controls[] = {
#if 0
	{
		/*
		 * For now, we just support in preset type
		 * to be close to generic WB system,
		 * we define color temp range for each preset
		 */
		.id = V4L2_CID_WHITE_BALANCE_TEMPERATURE,
		.type = V4L2_CTRL_TYPE_INTEGER,
		.name = "White balance in kelvin",
		.minimum = 0,
		.maximum = 10000,
		.step = 1,
		.default_value = 0,	/* FIXME */
	},
	{
		.id = V4L2_CID_WHITE_BALANCE_PRESET,
		.type = V4L2_CTRL_TYPE_MENU,
		.name = "White balance preset",
		.minimum = 0,
		.maximum = ARRAY_SIZE(s5k4eagx_querymenu_wb_preset) - 2,
		.step = 1,
		.default_value = 0,
	},
	{
		.id = V4L2_CID_AUTO_WHITE_BALANCE,
		.type = V4L2_CTRL_TYPE_BOOLEAN,
		.name = "Auto white balance",
		.minimum = 0,
		.maximum = 1,
		.step = 1,
		.default_value = 0,
	},
	{
		.id = V4L2_CID_EXPOSURE,
		.type = V4L2_CTRL_TYPE_MENU,
		.name = "Exposure bias",
		.minimum = 0,
		.maximum = ARRAY_SIZE(s5k4eagx_querymenu_ev_bias_mode) - 2,
		.step = 1,
		.default_value = (ARRAY_SIZE(s5k4eagx_querymenu_ev_bias_mode) \
				- 2) / 2,	/* 0 EV */
	},
#endif
	{
		.id = V4L2_CID_COLORFX,
		.type = V4L2_CTRL_TYPE_MENU,
		.name = "Image Effect",
		.minimum = 0,
		.maximum = ARRAY_SIZE(s5k4eagx_querymenu_effect_mode) - 2,
		.step = 1,
		.default_value = 0,
	},
#if 0
	{
		.id = V4L2_CID_CONTRAST,
		.type = V4L2_CTRL_TYPE_INTEGER,
		.name = "Contrast",
		.minimum = 0,
		.maximum = 4,
		.step = 1,
		.default_value = 2,
	},
	{
		.id = V4L2_CID_SATURATION,
		.type = V4L2_CTRL_TYPE_INTEGER,
		.name = "Saturation",
		.minimum = 0,
		.maximum = 4,
		.step = 1,
		.default_value = 2,
	},
	{
		.id = V4L2_CID_SHARPNESS,
		.type = V4L2_CTRL_TYPE_INTEGER,
		.name = "Sharpness",
		.minimum = 0,
		.maximum = 4,
		.step = 1,
		.default_value = 2,
	},
#endif	
};

const char **s5k4eagx_ctrl_get_menu(u32 id)
{
	switch (id) {
	case V4L2_CID_WHITE_BALANCE_PRESET:
		return s5k4eagx_querymenu_wb_preset;

	case V4L2_CID_COLORFX:
		return s5k4eagx_querymenu_effect_mode;

	case V4L2_CID_EXPOSURE:
		return s5k4eagx_querymenu_ev_bias_mode;

	default:
		return v4l2_ctrl_get_menu(id);
	}
}

static inline struct v4l2_queryctrl const *s5k4eagx_find_qctrl(int id)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(s5k4eagx_controls); i++)
		if (s5k4eagx_controls[i].id == id)
			return &s5k4eagx_controls[i];

	return NULL;
}

static int s5k4eagx_queryctrl(struct v4l2_subdev *sd, struct v4l2_queryctrl *qc)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(s5k4eagx_controls); i++) {
		if (s5k4eagx_controls[i].id == qc->id) {
			memcpy(qc, &s5k4eagx_controls[i], \
				sizeof(struct v4l2_queryctrl));
			return 0;
		}
	}

	return -EINVAL;
}

static int s5k4eagx_querymenu(struct v4l2_subdev *sd, struct v4l2_querymenu *qm)
{
	struct v4l2_queryctrl qctrl;

	qctrl.id = qm->id;
	s5k4eagx_queryctrl(sd, &qctrl);

	return v4l2_ctrl_query_menu(qm, &qctrl, s5k4eagx_ctrl_get_menu(qm->id));
}

/*
 * Clock configuration
 * Configure expected MCLK from host and return EINVAL if not supported clock
 * frequency is expected
 * 	freq : in Hz
 * 	flag : not supported for now
 */
static int s5k4eagx_s_crystal_freq(struct v4l2_subdev *sd, u32  freq, u32 flags)
{
	int err = -EINVAL;

	return err;
}

static int s5k4eagx_g_fmt(struct v4l2_subdev *sd, struct v4l2_format *fmt)
{
	int err = 0;

	return err;
}
#include <plat/s5pv310.h>

#include <plat/gpio-cfg.h>
#include <mach/gpio.h>
static int _s5k4eagx_init(struct v4l2_subdev *sd);

static int reset(struct v4l2_subdev *sd)
{
	printk("%s \n",__FUNCTION__);
		gpio_set_value(S5PV310_GPJ1(4), 1);

		//reset  --> L 
		gpio_set_value(S5PV310_GPX1(3), 0);
		mdelay(50);
		//reset  --> H			
		gpio_set_value(S5PV310_GPX1(3), 1);
		mdelay(500);

		_s5k4eagx_init(sd);

return 0;
	
}

static int s5k4eagx_set_frame_size(struct v4l2_subdev *sd, int width,int height)
{
	int err = 0;

	printk("s5k4eagx_set_frame_size : %d\n",width);

	g_s5k4eagx_width=width;
	g_s5k4eagx_height=height;

	//reset(sd);

	if(width == 640)
	{
		 err = __s5k4eagx_init_4bytes(sd, \
				(unsigned char **) s5k4eagx_preview_preset_2, S5K4EAGX_PREVIEW_PRESET_2);
	}
	else if(width == 720)
	{
		err = __s5k4eagx_init_4bytes(sd, \
				(unsigned char **) s5k4eagx_movie_preset_0, S5K4EAGX_MOVIE_PRESET_0);
	}
	else if(width == 800)
	{
		err = __s5k4eagx_init_4bytes(sd, \
				(unsigned char **) s5k4eagx_preview_preset_800x480, S5K4EAGX_PREVIEW_PRESET_800x480);
	}
 	else if(width == 1600  )
	{
			err = __s5k4eagx_init_4bytes(sd, \
					(unsigned char **) s5k4eagx_capture_preset_0, S5K4EAGX_CAPTURE_PRESET_0);
				mdelay(100);
					printk("width == 1600  ) %d\n",width);
	}
	else if(width == 1280 )
	{
		err = __s5k4eagx_init_4bytes(sd, \
				(unsigned char **) s5k4eagx_preview_preset_1280x720, S5K4EAGX_PREVIEW_PRESET_2592x1944);
				mdelay(100);
					printk("width == 1280  ) %d\n",width);
	}
 	else if(width == 2048  )
	{
		err = __s5k4eagx_init_4bytes(sd, \
					(unsigned char **) s5k4eagx_capture_preset_0, S5K4EAGX_CAPTURE_PRESET_0);
				mdelay(100);
				
	}
	 else if(width == 2592 && height==1944)
 	{
		 err = __s5k4eagx_init_4bytes(sd, \
					 (unsigned char **) s5k4eagx_capture_preset_0, S5K4EAGX_CAPTURE_PRESET_0);
		 printk("reg.set %dx%d \n",width,height);

			mdelay(100);
 	}
	 
	else if(width == 2048 || width == 1536 )
	{

	 	err=__s5k4eagx_init_4bytes(sd,s5k4eagx_capture_jpeg, \
			S5K4EAGX_CAPTURE_JPEG);
  	}		
	return err;

}
	

static int s5k4eagx_s_fmt(struct v4l2_subdev *sd, struct v4l2_format *fmt)
{
	int err = 0;

	struct s5k4eagx_state *state =
		container_of(sd, struct s5k4eagx_state, sd);
	struct i2c_client *client = v4l2_get_subdevdata(sd);

	dev_dbg(&client->dev, "%s: pixelformat = 0x%x (%c%c%c%c),"
		" colorspace = 0x%x, width = %d, height = %d\n",
		__func__, fmt->fmt.pix.pixelformat,
	fmt->fmt.pix.pixelformat,
	fmt->fmt.pix.pixelformat >> 8,
	fmt->fmt.pix.pixelformat >> 16,
	fmt->fmt.pix.pixelformat >> 24,
	fmt->fmt.pix.colorspace,
	fmt->fmt.pix.width, fmt->fmt.pix.height);

	if (fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_JPEG &&
	fmt->fmt.pix.colorspace != V4L2_COLORSPACE_JPEG) {
	dev_err(&client->dev,
		"%s: mismatch in pixelformat and colorspace\n",
		__func__);
	return -EINVAL;
	}

	if (fmt->fmt.pix.colorspace == V4L2_COLORSPACE_JPEG) {

	} else {
	}

	printk("## s5k4eagx_s_fmt :M%d %d %d \n",g_alive5,fmt->fmt.pix.width,fmt->fmt.pix.height);	
	
	if(g_alive5 != 5) return 0;
	err =  s5k4eagx_set_frame_size(sd, fmt->fmt.pix.width,fmt->fmt.pix.height);	
	return 0;

}
static int s5k4eagx_enum_framesizes(struct v4l2_subdev *sd,
					struct v4l2_frmsizeenum *fsize)
{
	int err = 0;
	fsize->type = V4L2_FRMSIZE_TYPE_DISCRETE;
	fsize->discrete.width=g_s5k4eagx_width;
	fsize->discrete.height=g_s5k4eagx_height;
	printk("## s5k4eagx_enum_framesizes %d %d \n",g_s5k4eagx_width,g_s5k4eagx_height);


	return err;
}

static int s5k4eagx_enum_frameintervals(struct v4l2_subdev *sd,
					struct v4l2_frmivalenum *fival)
{
	int err = 0;

	return err;
}

static int s5k4eagx_enum_fmt(struct v4l2_subdev *sd,
				struct v4l2_fmtdesc *fmtdesc)
{
	int err = 0;

	return err;
}

static int s5k4eagx_try_fmt(struct v4l2_subdev *sd, struct v4l2_format *fmt)
{
	int err = 0;

	return err;
}

static int s5k4eagx_g_parm(struct v4l2_subdev *sd, struct v4l2_streamparm *param)
{
	int err = 0;

	return err;
}

static int s5k4eagx_s_parm(struct v4l2_subdev *sd, struct v4l2_streamparm *param)
{
	int err = 0;

	return err;
}

static int s5k4eagx_g_ctrl(struct v4l2_subdev *sd, struct v4l2_control *ctrl)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct s5k4eagx_state *state = to_state(sd);
	struct s5k4eagx_userset userset = state->userset;
	int err = 0;
	switch (ctrl->id) {
	case V4L2_CID_EXPOSURE:
		ctrl->value = userset.exposure_bias;
		err = 0;
		break;
	case V4L2_CID_AUTO_WHITE_BALANCE:
		ctrl->value = userset.auto_wb;
		err = 0;
		break;
	case V4L2_CID_WHITE_BALANCE_PRESET:
		ctrl->value = userset.manual_wb;
		err = 0;
		break;
	case V4L2_CID_WHITE_BALANCE_TEMPERATURE:
		ctrl->value = userset.wb_temp;
		err = 0;
		break;
	case V4L2_CID_COLORFX:
		ctrl->value = userset.effect;
		err = 0;
		break;
	case V4L2_CID_CONTRAST:
		ctrl->value = userset.contrast;
		err = 0;
		break;
	case V4L2_CID_SATURATION:
		ctrl->value = userset.saturation;
		err = 0;
		break;
	case V4L2_CID_SHARPNESS:
		ctrl->value = userset.saturation;
		err = 0;
		break;
	case V4L2_CID_CAM_JPEG_MEMSIZE:
		ctrl->value = SENSOR_JPEG_SNAPSHOT_MEMSIZE;
		break;
	case V4L2_CID_CAM_JPEG_MAIN_OFFSET:
		ctrl->value =0;
		break;			
	case V4L2_CID_CAM_JPEG_POSTVIEW_OFFSET:
		ctrl->value = 0;
		break;
	case V4L2_CID_CAM_JPEG_MAIN_SIZE:
		ctrl->value =SENSOR_JPEG_SNAPSHOT_MEMSIZE;// state->jpeg.main_size;
		break;
	case V4L2_CID_CAM_JPEG_QUALITY:
		ctrl->value = 0;
		break;	
	default:
		err= -ENOIOCTLCMD;
		dev_err(&client->dev, "%s: no such ctrl\n", __func__);
		break;
	}

	return err;
}
void s5k4eagx_reset(int power_up);

static int _s5k4eagx_init(struct v4l2_subdev *sd)
{
	int err = -1;
	err = __s5k4eagx_init_4bytes(sd, \
		(unsigned char **) s5k4eagx_init_reg41, S5K4EAGX_INIT_REGS41);
	err = __s5k4eagx_init_2bytes(sd, \
		(unsigned short **) s5k4eagx_init_reg42, S5K4EAGX_INIT_REGS42);
	err = __s5k4eagx_init_2bytes(sd, \
		(unsigned short **) s5k4eagx_init_reg43, S5K4EAGX_INIT_REGS43);
	err = __s5k4eagx_init_2bytes(sd, \
		(unsigned short **) s5k4eagx_init_reg44, S5K4EAGX_INIT_REGS44);
	err = __s5k4eagx_init_2bytes(sd, \
		(unsigned short **) s5k4eagx_init_reg45, S5K4EAGX_INIT_REGS45);
	err = __s5k4eagx_init_2bytes(sd, \
		(unsigned short **) s5k4eagx_init_reg46, S5K4EAGX_INIT_REGS46);

return err;
}
unsigned short i2c_read_reg(struct i2c_client *client, unsigned short reg_h, unsigned short reg);

static int s5k4eagx_s_ctrl(struct v4l2_subdev *sd, struct v4l2_control *ctrl)
{
#ifdef S5K4EAGX_COMPLETE
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct s5k4eagx_state *state = to_state(sd);
//	struct s5k4eagx_userset userset = state->userset;
	int err = -EINVAL;
	int i;
	switch (ctrl->id) {
		/*
	case V4L2_CID_EXPOSURE:
		dev_dbg(&client->dev, "%s: V4L2_CID_EXPOSURE\n", \
			__func__);
		err = s5k4eagx_write_regs(sd, s5k4eagx_regs_ev_bias[ctrl->value]);
		break;
	case V4L2_CID_AUTO_WHITE_BALANCE:
		dev_dbg(&client->dev, "%s: V4L2_CID_AUTO_WHITE_BALANCE\n", \
			__func__);
		err = s5k4eagx_write_regs(sd, \
			s5k4eagx_regs_awb_enable[ctrl->value]);
		break;
	case V4L2_CID_WHITE_BALANCE_PRESET:
		dev_dbg(&client->dev, "%s: V4L2_CID_WHITE_BALANCE_PRESET\n", \
			__func__);
		err = s5k4eagx_write_regs(sd, \
			s5k4eagx_regs_wb_preset[ctrl->value]);
		break;
	case V4L2_CID_WHITE_BALANCE_TEMPERATURE:
		dev_dbg(&client->dev, \
			"%s: V4L2_CID_WHITE_BALANCE_TEMPERATURE\n", __func__);
		err = s5k4eagx_write_regs(sd, \
			s5k4eagx_regs_wb_temperature[ctrl->value]);
		break;
	case V4L2_CID_COLORFX:
		dev_dbg(&client->dev, "%s: V4L2_CID_COLORFX\n", __func__);
		err = s5k4eagx_write_regs(sd, \
			s5k4eagx_regs_color_effect[ctrl->value]);
		break;
	case V4L2_CID_CONTRAST:
		dev_dbg(&client->dev, "%s: V4L2_CID_CONTRAST\n", __func__);
		err = s5k4eagx_write_regs(sd, \
			s5k4eagx_regs_contrast_bias[ctrl->value]);
		break;
	case V4L2_CID_SATURATION:
		dev_dbg(&client->dev, "%s: V4L2_CID_SATURATION\n", __func__);
		err = s5k4eagx_write_regs(sd, \
			s5k4eagx_regs_saturation_bias[ctrl->value]);
		break;
	case V4L2_CID_SHARPNESS:
		dev_dbg(&client->dev, "%s: V4L2_CID_SHARPNESS\n", __func__);
		err = s5k4eagx_write_regs(sd, \
			s5k4eagx_regs_sharpness_bias[ctrl->value]);
		break;
		*/
	case V4L2_CID_FOCUS_AUTO:
		err = __s5k4eagx_init_4bytes(sd, \
			(unsigned char **)s5k4eagx_regs_focus[ctrl->value], S5K4EAGX_FOCUS);
		for(i=0;i<30;i++){
			err=i2c_read_reg(client,0x7000, 0x2b2a); //read AF status reg.

			if(err == 0x0000){
				printk("Idle AF search \n");
				mdelay(100);
			}
			else if(err == 0x0001){
				printk("AF search in progress\n");
				mdelay(100);
			}
			else if(err == 0x0002){
				printk("AF search success\n");
				return 1;
			}
			else if(err == 0x0003){
				printk("Low confidence position\n");
				return 0;
			}
			else if(err == 0x0004){
				printk("AF search is cancelled\n");
				return 1;
			}
			else {
				return 1;
			}
		}
		return 0;
		break;
			
	case V4L2_CID_CAM_ZOOM_CONTROL:
		err = __s5k4eagx_init_4bytes(sd, \
			(unsigned char **)s5k4eagx_regs_zoom[ctrl->value], S5K4EAGX_ZOOM);
		return 0;

		break;
		
	case V4L2_CID_COLORFX:
		err = __s5k4eagx_init_4bytes(sd, \
			(unsigned char **)s5k4eagx_regs_color_effect[ctrl->value], S5K4EAGX_COLOR);
		return 0;

		break;
#if 0
	case V4L2_CID_CAM_OUT_PRESET:  //ydongyol: 2010.11.18 

		switch(ctrl->value)
			{
			
			case PRESET_PREVIEW_0:
				err = __s5k4eagx_init_4bytes(sd, \
					(unsigned char **) s5k4eagx_preview_preset_0, S5K4EAGX_PREVIEW_PRESET_0);
				break;
			case PRESET_PREVIEW_1:
				err = __s5k4eagx_init_4bytes(sd, \
					(unsigned char **) s5k4eagx_preview_preset_1, S5K4EAGX_PREVIEW_PRESET_1);
				break;
			case PRESET_PREVIEW_2:
				err = __s5k4eagx_init_4bytes(sd, \
					(unsigned char **) s5k4eagx_preview_preset_2, S5K4EAGX_PREVIEW_PRESET_2);
				break;
			case PRESET_PREVIEW_3:
				err = __s5k4eagx_init_4bytes(sd, \
					(unsigned char **) s5k4eagx_preview_preset_3, S5K4EAGX_PREVIEW_PRESET_3);
				break;
			case PRESET_CAPTURE_0:
				err = __s5k4eagx_init_4bytes(sd, \
					(unsigned char **) s5k4eagx_capture_preset_0, S5K4EAGX_CAPTURE_PRESET_0);
				break;
			case PRESET_CAPTURE_1:
				err = __s5k4eagx_init_4bytes(sd, \
					(unsigned char **) s5k4eagx_capture_preset_1, S5K4EAGX_CAPTURE_PRESET_1);
				break;
			case PRESET_CAPTURE_2:
				err = __s5k4eagx_init_4bytes(sd, \
					(unsigned char **) s5k4eagx_capture_preset_2, S5K4EAGX_CAPTURE_PRESET_2);
				break;
			case PRESET_CAPTURE_3:
				err = __s5k4eagx_init_4bytes(sd, \
					(unsigned char **) s5k4eagx_capture_preset_3, S5K4EAGX_CAPTURE_PRESET_3);
				break;
			case PRESET_PREVIEW_DEFAULT:
				err = __s5k4eagx_init_4bytes(sd, \
					(unsigned char **) s5k4eagx_preview_preset_2, S5K4EAGX_PREVIEW_PRESET_2);
				break;
			case PRESET_CAPTURE_DEFAULT:
					err = __s5k4eagx_init_4bytes(sd, \
					(unsigned char **) s5k4eagx_capture_preset_0, S5K4EAGX_CAPTURE_PRESET_0);
				break;
									
			default:
				break;
			}
		break;
#endif	
	case V4L2_CID_CAMERA_SCENE_MODE:			
		return 0;
		break;
	case V4L2_CID_CAM_PREVIEW_ONOFF:
		return 0;
		break;
//			case V4L2_CID_CAMERA_RETURN_FOCUS:
//				return 0;
//				break;
	case V4L2_CID_CAMERA_CAPTURE :
		printk("s5k4eagx_capture_jpeg\n");
		err=__s5k4eagx_init_4bytes(sd,s5k4eagx_capture_jpeg, \
			S5K4EAGX_CAPTURE_JPEG);
			mdelay(300);
		return 0;
		break;
	case V4L2_CID_CAM_JPEG_QUALITY:
		state->jpeg.quality = ctrl->value;
//				err = s5k5cagx_set_jpeg_quality(sd);
		return 0;
		break;

	default:
		dev_err(&client->dev, "%s: no such control\n", __func__);
		break;
	}

//	_error_check(client);

	if (err < 0)
		goto out;
	else
		return 0;

out:
	dev_dbg(&client->dev, "%s: vidioc_s_ctrl failed\n", __func__);
	return err;
#else
	return 0;
#endif
}

int
__s5k4eagx_init_4bytes(struct v4l2_subdev *sd, unsigned char *reg[], int total)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	int err = -EINVAL, i;
	unsigned char *item;

	for (i = 0; i < total; i++) {
		item = (unsigned char *) &reg[i];
		if (item[0] == REG_DELAY) {
			mdelay(item[1]);
			err = 0;
		} else {
			err = s5k4eagx_i2c_write(sd, item, 4);
			mdelay(1);
		}

		if (err < 0)
			v4l_info(client, "%s: register set failed\n", \
			__func__);
	}

	return 0;
}

static int
__s5k4eagx_init_2bytes(struct v4l2_subdev *sd, unsigned short *reg[], int total)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	int err = -EINVAL, i;
	unsigned short *item;

	for (i = 0; i < total; i++) {
		item = (unsigned short *) &reg[i];
		if (item[0] == REG_DELAY) {
			mdelay(item[1]);
			err = 0;
		} else {
			err = s5k4eagx_write(sd, item[0], item[1]);
			mdelay(1);
		}

		if (err < 0)
			v4l_info(client, "%s: register set failed\n", \
			__func__);
	}

	return err;
}

static unsigned short i2c_read_reg(struct i2c_client *client, unsigned short reg_h, unsigned short reg)
{
	int ret;
	unsigned char i2c_data[10];

	//{0x00, 0x28, 0x70, 0x00},  
	i2c_data[0]= 0x00;
	i2c_data[1]= 0x2c;
	i2c_data[2]= reg_h >>8;		//0x70;
	i2c_data[3]= reg_h & 0xff;	//0x00;

	i2c_master_send(client,i2c_data,4);
	

	i2c_data[0]= 0x00;
	i2c_data[1]= 0x2e;
	i2c_data[2]= (unsigned char)((reg>>8) & 0xff);
	i2c_data[3]= (unsigned char)(reg & 0xff);	
	

	i2c_master_send(client,i2c_data,4);

	i2c_data[0]=0x0f;
	i2c_data[1]=0x12;
	i2c_master_send(client,i2c_data,2);			
	

	ret = i2c_master_recv(client,i2c_data,2);

#if 0
	for(i=0;i<2;i++)
	printk("retdata %d => %x \n",i,i2c_data[i]);
#endif

#if 0
		if (ret < 0)
			printk( "%s: err %d\n", __func__, ret);
#endif
	
		return i2c_data[0]<<8 | i2c_data[1];
}

static int read_device_id(struct i2c_client *client)
{
	int id1;
	int id2;

	v4l_info(client,"Version infomation reg 0x01e4 :0x%x \n"	,id1=i2c_read_reg(client,0x7000, 0x1e4));
	v4l_info(client,"Revision information reg 0x01e6 :0x%x \n"	,id2=i2c_read_reg(client,0x7000, 0x1e6));

	if((id1 == 0x4ea) && ((id2 == 0x11) || id2 == 0x12)) return 0;

return -1;
}
static int _error_check(struct i2c_client *client)
{
	int err=0;
	printk("##error check ***\n");
	printk("##2a8 : %x\n", i2c_read_reg(client,0x7000, 0x2a8));
	printk("##2ae : %x\n", i2c_read_reg(client,0x7000, 0x2ae));
	printk("##2c0 : %x\n", i2c_read_reg(client,0x7000, 0x2c0));
	printk("##250 : %x\n", i2c_read_reg(client,0x7000, 0x250));
	printk("##26e : %x\n", i2c_read_reg(client,0x7000, 0x26e));
	printk("##2b2a af : %x\n", i2c_read_reg(client,0x7000, 0x2b2a));
	printk("##2bc af : %x\n", i2c_read_reg(client,0x7000, 0x2bc));
	printk("##***************\n");

return err;
}

static int s5k4eagx_connected_check(struct i2c_client *client)
{
	int id;
// id check
	id=read_device_id(client);

	if(id <0) return -1;
	return 0;
}


static int s5k4eagx_init(struct v4l2_subdev *sd, u32 val)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	int err;
	printk("s5k4eagx_init\n");

	struct s5k4eagx_state *state =
		container_of(sd, struct s5k4eagx_state, sd);

	state->jpeg.enable = 0;
	state->jpeg.quality = 100;
	state->jpeg.main_offset = 0;
	state->jpeg.main_size = 0;
	state->jpeg.thumb_offset = 0;
	state->jpeg.thumb_size = 0;
	state->jpeg.postview_offset = 0;
	
	if(s5k4eagx_connected_check(client)<0) {
		printk("Not Connected\n");
		v4l_err(client, "%s: camera initialization failed\n", __func__);
		g_alive5= 0;
		return -1;
		}
	v4l_info(client, "%s: camera initialization start\n", __func__);

	err = _s5k4eagx_init(sd);
	//	
	err = s5k4eagx_set_frame_size(sd, g_s5k4eagx_width,g_s5k4eagx_height);

	if (err < 0) {
		v4l_err(client, "%s: camera initialization failed\n", \
			__func__);
		return -EIO;	/* FIXME */
	}
	v4l_info(client, "%s: camera initialization done\n", __func__);

	
	g_alive5 = 5;
	return 5;
}

/*
 * s_config subdev ops
 * With camera device, we need to re-initialize every single opening time
 * therefor,it is not necessary to be initialized on probe time.
 * except for version checking
 * NOTE: version checking is optional
 */
static int s5k4eagx_s_config(struct v4l2_subdev *sd, int irq, void *platform_data)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct s5k4eagx_state *state = to_state(sd);
	struct s5k4eagx_platform_data *pdata;

	dev_info(&client->dev, "fetching platform data\n");

	pdata = client->dev.platform_data;

	if (!pdata) {
		dev_err(&client->dev, "%s: no platform data\n", __func__);
		return -ENODEV;
	}

	/*
	 * Assign default format and resolution
	 * Use configured default information in platform data
	 * or without them, use default information in driver
	 */
	if (!(pdata->default_width && pdata->default_height)) {
		/* TODO: assign driver default resolution */
	} else {
		state->pix.width = pdata->default_width;
		state->pix.height = pdata->default_height;
	}

	if (!pdata->pixelformat)
		state->pix.pixelformat = DEFAULT_FMT;
	else
		state->pix.pixelformat = pdata->pixelformat;

	if (!pdata->freq)
		state->freq = 24000000;	/* 48MHz default */
	else
		state->freq = pdata->freq;

	if (!pdata->is_mipi) {
		state->is_mipi = 0;
		dev_info(&client->dev, "parallel mode\n");
	} else
		state->is_mipi = pdata->is_mipi;

	return 0;
}

static int s5k4eagx_sleep(struct v4l2_subdev *sd)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	int err = -EINVAL, i;

	v4l_info(client, "%s: sleep mode\n", __func__);

	for (i = 0; i < S5K4EAGX_SLEEP_REGS; i++) {
		if (s5k4eagx_sleep_reg[i][0] == REG_DELAY) {
			mdelay(s5k4eagx_sleep_reg[i][1]);
			err = 0;
		} else {
			err = s5k4eagx_write(sd, s5k4eagx_sleep_reg[i][0], \
				s5k4eagx_sleep_reg[i][1]);
		}

		if (err < 0)
			v4l_info(client, "%s: register set failed\n", __func__);
	}

	if (err < 0) {
		v4l_err(client, "%s: sleep failed\n", __func__);
		return -EIO;
	}

	return 0;
}

static int s5k4eagx_wakeup(struct v4l2_subdev *sd)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	int err = -EINVAL, i;

	v4l_info(client, "%s: wakeup mode\n", __func__);

	for (i = 0; i < S5K4EAGX_WAKEUP_REGS; i++) {
		if (s5k4eagx_wakeup_reg[i][0] == REG_DELAY) {
			mdelay(s5k4eagx_wakeup_reg[i][1]);
			err = 0;
		} else {
			err = s5k4eagx_write(sd, s5k4eagx_wakeup_reg[i][0], \
				s5k4eagx_wakeup_reg[i][1]);
		}

		if (err < 0)
			v4l_info(client, "%s: register set failed\n", __func__);
	}

	if (err < 0) {
		v4l_err(client, "%s: wake up failed\n", __func__);
		return -EIO;
	}

	return 0;
}

static int s5k4eagx_s_stream(struct v4l2_subdev *sd, int enable)
{
	return enable ? s5k4eagx_wakeup(sd) : s5k4eagx_sleep(sd);
}

static const struct v4l2_subdev_core_ops s5k4eagx_core_ops = {
	.init = s5k4eagx_init,	/* initializing API */
	.s_config = s5k4eagx_s_config,	/* Fetch platform data */
	.queryctrl = s5k4eagx_queryctrl,
	.querymenu = s5k4eagx_querymenu,
	.g_ctrl = s5k4eagx_g_ctrl,
	.s_ctrl = s5k4eagx_s_ctrl,
};

static const struct v4l2_subdev_video_ops s5k4eagx_video_ops = {
	.s_crystal_freq = s5k4eagx_s_crystal_freq,
	.g_fmt = s5k4eagx_g_fmt,
	.s_fmt = s5k4eagx_s_fmt,
	.enum_framesizes = s5k4eagx_enum_framesizes,
	.enum_frameintervals = s5k4eagx_enum_frameintervals,
	.enum_fmt = s5k4eagx_enum_fmt,
	.try_fmt = s5k4eagx_try_fmt,
	.g_parm = s5k4eagx_g_parm,
	.s_parm = s5k4eagx_s_parm,
	.s_stream = s5k4eagx_s_stream,
};

static const struct v4l2_subdev_ops s5k4eagx_ops = {
	.core = &s5k4eagx_core_ops,
	.video = &s5k4eagx_video_ops,
};
#define DELETE_THIS_12341 1

/*
 * s5k4eagx_probe
 * Fetching platform data is being done with s_config subdev call.
 * In probe routine, we just register subdev device
 */
static int s5k4eagx_probe(struct i2c_client *client,
			 const struct i2c_device_id *id)
{
	struct s5k4eagx_state *state;
	struct v4l2_subdev *sd;

	state = kzalloc(sizeof(struct s5k4eagx_state), GFP_KERNEL);
	if (state == NULL)
		return -ENOMEM;

	g_alive5=0;

	sd = &state->sd;
	strcpy(sd->name, S5K4EAGX_DRIVER_NAME);

	/* Registering subdev */
	v4l2_i2c_subdev_init(sd, client, &s5k4eagx_ops);

	dev_info(&client->dev, "s5k4eagx has been probed\n");

	return 0;
}


static int s5k4eagx_remove(struct i2c_client *client)
{
	struct v4l2_subdev *sd = i2c_get_clientdata(client);

	v4l2_device_unregister_subdev(sd);
	kfree(to_state(sd));
	g_alive5=0;

	return 0;
}

static const struct i2c_device_id s5k4eagx_id[] = {
	{ S5K4EAGX_DRIVER_NAME, 0 },
	{ },
};
MODULE_DEVICE_TABLE(i2c, s5k4eagx_id);

static struct v4l2_i2c_driver_data v4l2_i2c_data = {
	.name = S5K4EAGX_DRIVER_NAME,
	.probe = s5k4eagx_probe,
	.remove = s5k4eagx_remove,
	.id_table = s5k4eagx_id,
};

MODULE_DESCRIPTION("Samsung Electronics S5K4EAGX SXGA camera driver");
MODULE_AUTHOR("Dongsoo Nathaniel Kim<dongsoo45.kim@samsung.com>");
MODULE_LICENSE("GPL");

