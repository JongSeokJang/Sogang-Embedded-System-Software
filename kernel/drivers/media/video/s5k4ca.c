/*
 * Driver for S5K4CA (QXGA camera) from Samsung Electronics
 * 
 * Copyright (C) 2010, jssong <jssong@huins.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/version.h>
#include <linux/slab.h>
#include <media/v4l2-device.h>
#include <media/v4l2-subdev.h>
#include <media/v4l2-i2c-drv.h>
#include <media/s5k4ca_platform.h>
#include <linux/regulator/consumer.h>
#ifdef CONFIG_VIDEO_SAMSUNG_V4L2
#include <linux/videodev2_samsung.h>
#endif
#include "s5k4ca.h"

#if defined(S5K4CA_DEBUG_MSG)
#define CAM_DEBUG() printk("\nCAMERA_DEBUG : %s, %s\n", __FILE__, __func__)
#define S5K4CA_LOG printk
#else
#define CAM_DEBUG() do{} while(0);
#define S5K4CA_LOG
#endif

#define S5K4CA_DRIVER_NAME	"S5K4CA"

static int s5k4ca_init(struct v4l2_subdev *sd, u32 val);
extern void s5k4ca_power_control(int select);
/****************************************************************************/
/* // kskim added */
/****************************************************************************/
static int s5k4ca_sensor_write(struct i2c_client *client, unsigned short subaddr, unsigned short val)
{
	unsigned char buf[4];
	struct i2c_msg msg = { client->addr, 0, 4, buf };
	int ret;

	buf[0] = (subaddr >> 8);
	buf[1] = (subaddr & 0xFF);
	buf[2] = (val >> 8);
	buf[3] = (val & 0xFF);

	ret = i2c_transfer(client->adapter, &msg, 1) == 1 ? 0 : -EIO;

	if (ret == -EIO)
		printk("s5k4ca_sensor_write error\n");
	return ret;
}

static int s5k4ca_sensor_read(struct i2c_client *client, unsigned short subaddr, unsigned short *data)
{
	int ret;
	unsigned char buf[2];
	struct i2c_msg msg = { client->addr, 0, 2, buf };

	buf[0] = (subaddr >> 8);
	buf[1] = (subaddr & 0xFF);

	ret = i2c_transfer(client->adapter, &msg, 1) == 1 ? 0 : -EIO;
	if (ret == -EIO)
		goto error;

	msg.flags = I2C_M_RD;

	ret = i2c_transfer(client->adapter, &msg, 1) == 1 ? 0 : -EIO;
	if (ret == -EIO)
		goto error;

	*data = ((buf[0] << 8) | buf[1]);

error:
	printk("s5k4ca_sensor_read error\n");
	return ret;
}

static void s5k4ca_sensor_get_id(struct i2c_client *client)
{
    unsigned short id = 0;

	CAM_DEBUG();

    s5k4ca_sensor_write(client, 0x002C, 0x7000);
    s5k4ca_sensor_write(client, 0x002E, 0x01FA);
    s5k4ca_sensor_read(client, 0x0F12, &id);
	
	S5K4CA_LOG("---------------------------------------------------------\n");
    S5K4CA_LOG("Sensor ID(0x%04x) is %s!\n", id, (id == 0x4CA4) ? "Valid" : "Invalid");
	S5K4CA_LOG("---------------------------------------------------------\n");
}

static int s5k4ca_sensor_write_list(struct i2c_client *client, struct samsung_short_t *list, int count)
{
	int i, ret;
	ret = 0;

	CAM_DEBUG();

	for (i = 0; i < count; i++)
		ret = s5k4ca_sensor_write(client, list[i].subaddr, list[i].value);

	return ret;
}

static int s5k4ca_queryctrl(struct v4l2_subdev *sd, struct v4l2_queryctrl *qc)
{
	CAM_DEBUG();
	
	return 0;
}

static int s5k4ca_querymenu(struct v4l2_subdev *sd, struct v4l2_querymenu *qm)
{
	CAM_DEBUG();
	
	return 0;
}

static int s5k4ca_s_crystal_freq(struct v4l2_subdev *sd, u32 freq, u32 flags)
{
	int err = -EINVAL;
	
	CAM_DEBUG();

	return err;
}

static int s5k4ca_g_fmt(struct v4l2_subdev *sd, struct v4l2_format *fmt)
{
	int err = 0;

	CAM_DEBUG();

	return err;
}

static int s5k4ca_s_fmt(struct v4l2_subdev *sd, struct v4l2_format *fmt)
{
	int err = 0;

	CAM_DEBUG();

	return err;
}
static int s5k4ca_enum_framesizes(struct v4l2_subdev *sd, struct v4l2_frmsizeenum *fsize)
{
	int err = 0;
	
	CAM_DEBUG();
	fsize->discrete.width = 800;
	fsize->discrete.height = 600;

	return err;
}

static int s5k4ca_enum_frameintervals(struct v4l2_subdev *sd, struct v4l2_frmivalenum *fival)
{
	int err = 0;

	CAM_DEBUG();

	return err;
}

static int s5k4ca_enum_fmt(struct v4l2_subdev *sd, struct v4l2_fmtdesc *fmtdesc)
{
	int err = 0;

	CAM_DEBUG();

	return err;
}

static int s5k4ca_try_fmt(struct v4l2_subdev *sd, struct v4l2_format *fmt)
{
	int err = 0;

	CAM_DEBUG();

	return err;
}

static int s5k4ca_g_parm(struct v4l2_subdev *sd, struct v4l2_streamparm *param)
{
	int err = 0;

	CAM_DEBUG();

	return err;
}


static int s5k4ca_s_parm(struct v4l2_subdev *sd, struct v4l2_streamparm *param)
{
	CAM_DEBUG();

	return 0;
}

static int s5k4ca_g_ctrl(struct v4l2_subdev *sd, struct v4l2_control *ctrl)
{
//	CAM_DEBUG();
	
	return 0;
}

static int s5k4ca_s_ctrl(struct v4l2_subdev *sd, struct v4l2_control *ctrl)
{
	CAM_DEBUG();
	
	return 0;
}

static int s5k4ca_init(struct v4l2_subdev *sd, u32 val)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	int count = 0;

	CAM_DEBUG();
	
	count = sizeof(s5k4ca_init0) / sizeof(s5k4ca_init0[0]);
	S5K4CA_LOG("init count0 : %d\n", count);
	s5k4ca_sensor_write_list(client,s5k4ca_init0, count);
	msleep(100);

	s5k4ca_sensor_get_id(client);

	count = sizeof(s5k4ca_init1) / sizeof(s5k4ca_init1[0]);
	S5K4CA_LOG("init count1 : %d\n", count);
	s5k4ca_sensor_write_list(client,s5k4ca_init1, count);


	return 0;
}

static int s5k4ca_s_config(struct v4l2_subdev *sd, int irq, void *platform_data)
{
	CAM_DEBUG();
	
	return 0;
}

static const struct v4l2_subdev_core_ops s5k4ca_core_ops = {
	.init = s5k4ca_init,	/* initializing API */
	.s_config = s5k4ca_s_config,	/* Fetch platform data */
	.queryctrl = s5k4ca_queryctrl,
	.querymenu = s5k4ca_querymenu,
	.g_ctrl = s5k4ca_g_ctrl,
	.s_ctrl = s5k4ca_s_ctrl,
};

static const struct v4l2_subdev_video_ops s5k4ca_video_ops = {
	.s_crystal_freq = s5k4ca_s_crystal_freq,
	.g_fmt = s5k4ca_g_fmt,
	.s_fmt = s5k4ca_s_fmt,
	.enum_framesizes = s5k4ca_enum_framesizes,
	.enum_frameintervals = s5k4ca_enum_frameintervals,
	.enum_fmt = s5k4ca_enum_fmt,
	.try_fmt = s5k4ca_try_fmt,
	.g_parm = s5k4ca_g_parm,
	.s_parm = s5k4ca_s_parm,
};

static const struct v4l2_subdev_ops s5k4ca_ops = {
	.core = &s5k4ca_core_ops,
	.video = &s5k4ca_video_ops,
};

static int s5k4ca_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct v4l2_subdev *sd;
#if defined(CONFIG_BOARD_ACHRO4210)
	s5k4ca_power_control(1);
#endif
	CAM_DEBUG();

	
printk("S5K4CA PROBE\n");	
	sd = kzalloc(sizeof(struct v4l2_subdev), GFP_KERNEL);
	if (sd == NULL)
		return -ENOMEM;

	strcpy(sd->name, S5K4CA_DRIVER_NAME);

printk("I2C name = %s\n", client->name);
	/* Registering subdev */
	v4l2_i2c_subdev_init(sd, client, &s5k4ca_ops);

	dev_info(&client->dev, "s5k4ca has been probed\n");
	
	return 0;
}


static int s5k4ca_remove(struct i2c_client *client)
{
	struct v4l2_subdev *sd = i2c_get_clientdata(client);
#if defined(CONFIG_BOARD_ACHRO4210)
	s5k4ca_power_control(0);
#endif
	CAM_DEBUG();
	
	v4l2_device_unregister_subdev(sd);
	kfree(sd);
	
	return 0;
}

static const struct i2c_device_id s5k4ca_id[] = {
	{ S5K4CA_DRIVER_NAME, 0 },
	{ },
};
MODULE_DEVICE_TABLE(i2c, s5k4ca_id);

static struct v4l2_i2c_driver_data v4l2_i2c_data = {
	.name = S5K4CA_DRIVER_NAME,
	.probe = s5k4ca_probe,
	.remove = s5k4ca_remove,
	.id_table = s5k4ca_id,
};

MODULE_DESCRIPTION("HUINS S5K4CA UXGA camera driver");
MODULE_AUTHOR("jssong <jssong@huins.com>");
MODULE_LICENSE("GPL");

