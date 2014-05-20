/* 
 *
 * Sain touch driver
 *
 * Copyright (C) 2009 Sain, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
// Version 2.0.0 : using reg data file (2010/11/05)
// Version 2.0.1 : syntxt bug fix (2010/11/09)
// Version 2.0.2 : Save status cmd delay bug (2010/11/10)
// Version 2.0.3 : modify delay 10ms -> 50ms for clear hw calibration bit
//		: modify SAIN_TOTAL_NUMBER_OF_Y register (read only -> read/write )
//		: modify SUPPORTED FINGER NUM register (read only -> read/write )
// Version 2.0.4 : [20101116]
//	Modify Firmware Upgrade routine.
// Version 2.0.5 : [20101118]
//	add esd timer function & some bug fix.
//	you can select request_threaded_irq or request_irq, setting USE_THREADED_IRQ.
// Version 2.0.6 : [20101123]
//	add ABS_MT_WIDTH_MAJOR Report
// Version 2.0.7 : [20101201]
//	Modify sain_early_suspend() / sain_late_resume() routine.
// Version 2.0.8 : [20101216]
//	add using spin_lock option
// Version 2.0.9 : [20101216]
//	Test Version
// Version 2.0.10 : [20101217]
//	add USE_THREAD_METHOD option. if  USE_THREAD_METHOD = 0, you use workqueue
// Version 2.0.11 : [20101229]
//	add USE_UPDATE_SYSFS option for update firmware. && TOUCH_MODE == 1 mode.
// Version 2.0.13 : [20110125]
//	modify esd timer routine
// Version 2.0.14 : [20110217]
//	esd timer bug fix. (kernel panic)
//	sysfs bug fix.
// Version 2.0.15 : [20110315]
//	add power off delay ,250ms
// Version 2.0.16 : [20110316]
//	add upgrade method using isp


#include <linux/module.h>
#include <linux/input.h>
#include <linux/i2c.h>		// I2C_M_NOSTART
#include <linux/miscdevice.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/hrtimer.h>
#include <linux/ioctl.h>
#include <linux/earlysuspend.h>
#include <linux/string.h>
#include <linux/semaphore.h>
#include <linux/kthread.h>
#include <linux/timer.h>
#include <linux/workqueue.h>
#include <linux/firmware.h>

#include <asm/io.h>
#include <linux/delay.h>
//#include <mach/vreg.h>          /* set a vreg */
#include <mach/gpio.h>
#include <plat/gpio-cfg.h>

#include <linux/slab.h>
//#include <asm/gpio.h>

#include "sain_touch.h"
#include "sain_touch_reg_data.h"
#if TOUCH_ONESHOT_UPGRADE
#include "sain_touch_firmware.h"
#endif

#include "achro4210_touch.h"
#include "sain_touch_gpio_i2c.h"
#include "achro4210_sain_touch_sysfs.h"


#include <linux/regulator/machine.h>

#define	SYSTEM_MAX_X_RESOLUTION	800
#define	SYSTEM_MAX_Y_RESOLUTION	480

#define	SAIN_DEBUG				0
#define SAIN_DRIVER_NAME        "sain_touch"
#define	SAIN_INIT_RETRY_CNT		10
#define	SAIN_ESD_TIMER_INTERVAL	3
#define	SAIN_SCAN_RATE_HZ		100
#define	TS_DRIVER_NAME			"sain_touch"
#define	TS_RESET_OUT			(S5PV310_GPC0(3))
#define GPIO_TOUCH_IRQ  		gpio_to_irq(S5PV310_GPX1(0))
#define GPIO_TOUCH_PIN_NUM  	S5PV310_GPX1(0)
#define	USE_THREAD_METHOD		0
#define	USE_UPDATE_SYSFS		0
#define	SUPPORTED_FINGER_NUM	5
#define	SUPPORTED_BUTTON_NUM	4
#define	ICON_BUTTON_UNCHANGE	0
#define	ICON_BUTTON_DOWN		1
#define	ICON_BUTTON_UP			2
#define	I2C_SUCCESS				0
#define	TOUCH_MODE				1
#define	TOUCH_V_FLIP			0x01
#define	TOUCH_H_FLIP			0x02
#define	TOUCH_XY_SWAP			0x04

#define swap_v(a, b, t)	((t) = (a), (a) = (b), (b) = (t))
#define swap_16(s) (((((s) & 0xff) << 8) | (((s) >> 8) & 0xff)))

#if	(!USE_THREAD_METHOD)
static struct workqueue_struct *sain_workqueue;
#endif

#if	SAIN_ESD_TIMER_INTERVAL
static struct workqueue_struct *sain_tmr_workqueue;
#endif

#if	SAIN_DEBUG
#define	sain_debug_msg(fmt, args...)	printk(KERN_INFO "[%s:%s:%d]\n",fmt, __FUNCTION__, __LINE__)//printk(KERN_INFO "[%-18s:%5d]" fmt, __FUNCTION__, __LINE__, ## args)
#else
#define	sain_debug_msg(fmt, args...)	do{}while(0)
#endif

inline int smbus_write_byte(struct i2c_client *client, u8 reg);
inline int smbus_write_word_data(struct i2c_client *client, u8 reg, u16 value);
u8 check_byte_size(u8 reg);
#define ts_write_cmd(client,reg) smbus_write_byte(client, reg)
#define ts_write_reg(client,reg,value) smbus_write_word_data(client, reg, value)

inline int smbus_write_byte(struct i2c_client *client, u8 reg)
{
	int ret;
	unsigned char wdata;
	ret = achro4210_touch_write(reg, &wdata,  1); 
	return ret;
}
inline int smbus_write_word_data(struct i2c_client *client, u8 reg, u16 value)
{
	int ret;
	u8 send_size=0;
	u8 msgbuf[20];
	send_size = check_byte_size(reg);
	if(send_size == 1){	ret = achro4210_touch_write(reg, &value, 1); }
	else if(send_size == 2){
		msgbuf[0] = value & 0xff;
		msgbuf[1] = value >> 8;
		ret = achro4210_touch_write(reg, msgbuf,2);
	}
	return ret;
}

inline int ts_read_data(struct i2c_client *client, u8 reg, u8 *values, u8 length)
{
	int ret;
	u8 msgbuf[20],i;
	ret = achro4210_touch_array_read(reg, msgbuf, length);
	for(i=0;i<length;i++){
		*(values + i) = msgbuf[i];
	}
	return ret;
}



inline s32 ts_read_firmware_data(struct i2c_client *client, u8 reg, u8 *values, u8 length)
{
	s32 ret;
	if((ret = i2c_master_send(client , &reg , 1)) < 0)	return ret;	// select register
	mdelay(1);		// for setup tx transaction.
	if((ret = i2c_master_recv(client , values , length)) < 0)	return ret;
	return length;
}

#define ts_write_data(client,reg,values,length) i2c_smbus_write_i2c_block_data(client, reg, length, values)

u8 check_byte_size(u8 reg) {
	u8 ret = 0;

	switch (reg) {
	case TOUCH_ONESHOT_UPGRADE: //TOUCH_USING_ISP_METHOD SAIN_REST_CMD:
	case SAIN_WAKEUP_CMD:
	case SAIN_IDLE_CMD:
	case SAIN_SLEEP_CMD:
	case SAIN_CLEAR_INT_STATUS_CMD:
	case SAIN_CALIBRATE_CMD:
	case SAIN_SAVE_STATUS_CMD:
	case SAIN_RECALL_FACTORY_CMD:
		ret = 0;
		break;

		// 0x10~12
	case SAIN_TOUCH_MODE:
		ret = 1;
		break;

	case SAIN_X_RESOLUTION:
	case SAIN_Y_RESOLUTION:
	case SAIN_INT_ENABLE_FLAG:
	case SAIN_CHIP_REVISION:
	case SAIN_EEPROM_INFO:
		// 0x20~21
	case SAIN_TOTAL_NUMBER_OF_X:
	case SAIN_TOTAL_NUMBER_OF_Y:
	case SAIN_SUPPORTED_FINGER_NUM:
	case SAIN_AFE_FREQUENCY:

		// 0x30~33
	case SAIN_CALIBRATION_REF:
	case SAIN_CALIBRATION_DEFAULT_N:
	case SAIN_NUMBER_OF_CALIBRATION:
	case SAIN_CALIBRATION_ACCURACY:
	case SAIN_PERIODICAL_INTERRUPT_INTERVAL:
	case SAIN_POINT_STATUS_REG:
	case SAIN_ICON_STATUS_REG: //icon event - four icon
	case SAIN_EEPROM_INFO_REG:
	case SAIN_DATA_VERSION_REG:
	case SAIN_FIRMWARE_VERSION: //SAIN_ERASE_FLASH
		//0xF0
		ret = 2;
		break;

	}
	return ret;

}

typedef	struct	
{
	u16	x;
	u16	y;
	//u16	width;	
	u8	width;	
	u8	sub_status;
}_ts_sain_coord;

typedef	struct	
{
	u16	status;
#if (TOUCH_MODE == 1)
	u16	event_flag;
#else
	u8	finger_cnt;
	u8	time_stamp;
#endif	
	_ts_sain_coord	coord[SUPPORTED_FINGER_NUM];

}_ts_sain_point_info;

typedef	struct
{
	u16 chip_revision; 
	u16 chip_firmware_version;
	u16 chip_reg_data_version;		  
	u32 chip_fw_size;	
	u32 MaxX;
	u32 MaxY;
	u32 MinX;
	u32 MinY;
	u32 Orientation;
	u8 gesture_support;
	u8 multi_fingers;
}_ts_capa_info;

typedef struct
{
	struct input_dev *input_dev;
	struct task_struct *task;
	wait_queue_head_t	wait;
	struct work_struct  work;
	struct work_struct  tmr_work;
	struct i2c_client *client;
	struct semaphore update_lock;    
	u32 i2c_dev_addr;
	_ts_capa_info	cap_info;
	
	bool is_valid_event;
	_ts_sain_point_info touch_info;
	_ts_sain_point_info reported_touch_info;    	
	u16 icon_event_reg;
	u16 chip_int_mask;    
	u16 event_type;
	u32 int_gpio_num;
	u32 irq;
	u8 button[SUPPORTED_BUTTON_NUM];

#if	SAIN_ESD_TIMER_INTERVAL
	u8	use_esd_timer;
	struct semaphore esd_lock;    
	struct timer_list esd_timeout_tmr;		//for repeated card detecting work
	struct timer_list *p_esd_timeout_tmr;		//for repeated card detecting work
#endif

#ifdef CONFIG_HAS_EARLYSUSPEND	
	struct early_suspend early_suspend;
#endif
} sain_touch_dev;

static struct i2c_device_id sain_idtable[] = {
    {TS_DRIVER_NAME, 0},  
    { }   
};

MODULE_DEVICE_TABLE(i2c, sain_idtable);//kang insert

// HISEOB : Inverting Keys
u32 BUTTON_MAPPING_KEY[SUPPORTED_BUTTON_NUM] = {KEY_MENU, KEY_HOME, KEY_BACK, KEY_SEARCH};

static int sain_touch_probe(struct i2c_client *client, const struct i2c_device_id *i2c_id);
static int sain_touch_remove(struct i2c_client *client);

#if (TOUCH_MODE == 1)
static void	sain_report_data(sain_touch_dev *touch_dev, int id);
#endif

#ifdef CONFIG_HAS_EARLYSUSPEND
static void sain_early_suspend(struct early_suspend *h);
static void sain_late_resume(struct early_suspend *h);
#endif

#if	SAIN_ESD_TIMER_INTERVAL
static void ts_esd_timer_start(u16 sec, sain_touch_dev* touch_dev);
static void ts_esd_timer_stop(sain_touch_dev* touch_dev);
static void ts_esd_timer_init(sain_touch_dev* touch_dev);
static void ts_esd_timeout_handler(unsigned long data);
#endif

static struct i2c_driver sain_touch_driver = {
	.probe     = sain_touch_probe,
	.remove    = sain_touch_remove,
	.id_table  = sain_idtable,
	.driver    = {
		.name  = SAIN_DRIVER_NAME,
	},
};

static bool ts_get_samples (sain_touch_dev* touch_dev)
{
	int i;
	sain_debug_msg("ts_get_samples+\r\n");
	if (gpio_get_value(touch_dev->int_gpio_num)) {
		return false;
	}

#if (TOUCH_MODE == 1)
	
	memset(&touch_dev->touch_info, 0x0, sizeof(_ts_sain_point_info));	

	if (ts_read_data (touch_dev->client, SAIN_POINT_STATUS_REG, (u8*)(&touch_dev->touch_info), 4)< 0)
	{
		sain_debug_msg("error read point info using i2c.-\r\n");
        	return false;
	}
	sain_debug_msg("status reg = 0x%x , event_flag = 0x%04x\r\n", touch_dev->touch_info.status, touch_dev->touch_info.event_flag);

	if(touch_dev->touch_info.status == 0x0)
	{
		sain_debug_msg("periodical esd repeated int occured\r\n");
		return true;
	}
	
	if(sain_bit_test(touch_dev->touch_info.status, BIT_ICON_EVENT))
	{
		udelay(20);
		if (ts_read_data (touch_dev->client, SAIN_ICON_STATUS_REG, (u8*)(&touch_dev->icon_event_reg), 2) < 0)
		{
			printk(KERN_INFO "error read icon info using i2c.\n");
      			return false;
		}
		return true;
	}

	if(!sain_bit_test(touch_dev->touch_info.status, BIT_PT_EXIST))
	{
		//ts_write_cmd(touch_dev->client, SAIN_CLEAR_INT_STATUS_CMD);
		for(i=0; i < SUPPORTED_FINGER_NUM; i++)
		{		
			if(sain_bit_test(touch_dev->reported_touch_info.coord[i].sub_status, SUB_BIT_EXIST))
			{		
				
				input_report_abs(touch_dev->input_dev, ABS_MT_TOUCH_MAJOR, 0);
				input_report_abs(touch_dev->input_dev, ABS_MT_WIDTH_MAJOR, 0);	
				input_report_abs(touch_dev->input_dev, ABS_MT_POSITION_X, touch_dev->reported_touch_info.coord[i].x);
				input_report_abs(touch_dev->input_dev, ABS_MT_POSITION_Y, touch_dev->reported_touch_info.coord[i].y);
			        input_mt_sync(touch_dev->input_dev);	
				touch_dev->reported_touch_info.coord[i].sub_status = 0;
			}
		}	
		input_sync(touch_dev->input_dev);
		return true;
	}	
	
	for(i=0; i < SUPPORTED_FINGER_NUM; i++)
	{
		if(sain_bit_test(touch_dev->touch_info.event_flag, i))
		{			
			udelay(20);
			if (ts_read_data (touch_dev->client, SAIN_POINT_STATUS_REG+2+i, (u8*)(&touch_dev->touch_info.coord[i]), sizeof(_ts_sain_coord))< 0)
			{
				sain_debug_msg("error read point info using i2c.-\r\n");
		        	return false;
			}
			sain_bit_clr(touch_dev->touch_info.event_flag, i);
			if(touch_dev->touch_info.event_flag == 0)	
			{
				//ts_write_cmd(touch_dev->client, SAIN_CLEAR_INT_STATUS_CMD);
				sain_report_data(touch_dev, i);
				return true;
			}
			else
				sain_report_data(touch_dev, i);
		}		
	}	


#else
	if (ts_read_data (touch_dev->client, SAIN_POINT_STATUS_REG, (u8*)(&touch_dev->touch_info), sizeof(_ts_sain_point_info))< 0)
	{
		sain_debug_msg("error read point info using i2c.-\r\n");
        	return false;
	}
	sain_debug_msg("status reg = 0x%x , point cnt = %d, time stamp = %d\r\n", touch_dev->touch_info.status, 
		touch_dev->touch_info.finger_cnt, touch_dev->touch_info.time_stamp);

	if(touch_dev->touch_info.status == 0x0 && touch_dev->touch_info.finger_cnt == 100)
	{
		sain_debug_msg("periodical esd repeated int occured\r\n");
		return true;
	}
	


	for(i=0; i< SUPPORTED_BUTTON_NUM; i++)	touch_dev->button[i] = ICON_BUTTON_UNCHANGE;

	if(sain_bit_test(touch_dev->touch_info.status, BIT_ICON_EVENT))
	{
		udelay(10);
		if (ts_read_data (touch_dev->client, SAIN_ICON_STATUS_REG, (u8*)(&touch_dev->icon_event_reg), 2) < 0)
		{
			//printK(KERN_INFO "error read icon info using i2c.\n");
        		return false;
		}
	}
#endif		
	sain_debug_msg("ts_get_samples-\r\n");
	
	return true;
}


static irqreturn_t ts_int_handler(int irq, void *dev)
{
	sain_touch_dev* touch_dev = (sain_touch_dev*)dev;
	sain_debug_msg("interrupt occured +\r\n");
	disable_irq_nosync(irq);
#if USE_THREAD_METHOD	
	up(&touch_dev->update_lock); 	
#else
	queue_work(sain_workqueue, &touch_dev->work);
#endif	// USE_THREAD_METHOD
	return IRQ_HANDLED;
}

static bool ts_read_coord (sain_touch_dev * hDevice)
{
	sain_touch_dev* touch_dev = (sain_touch_dev*)hDevice;
	//sain_debug_msg("ts_read_coord+\r\n");

	if(ts_get_samples(touch_dev)==false)
	{
		return false;
	}
	ts_write_cmd(touch_dev->client, SAIN_CLEAR_INT_STATUS_CMD);
	return true;
}

//
static void ts_power_control(u8 ctl)
{
	if(ctl == 0) //power off
	{
	}
	else if(ctl == 1)	//power on
	{
	}
	else if(ctl == 2)	//reset pin low
	{
		gpio_set_value(TS_RESET_OUT, 0);	mdelay(10);
	}
	else if(ctl == 3)	//reset pin high
	{
		gpio_set_value(TS_RESET_OUT, 1);	mdelay(10);
	}


}

static bool ts_mini_init_touch(sain_touch_dev * touch_dev)
{
	u16	reg_val;
	int i;	
	if(touch_dev == NULL)	
	{
		//printK(KERN_INFO "ts_mini_init_touch : error (touch_dev == NULL?)\r\n");
		return false;		
	}	
	if (ts_write_reg(touch_dev->client, SAIN_INT_ENABLE_FLAG, 0x0)!=I2C_SUCCESS)	goto fail_mini_init;	
		mdelay(10);
	if (ts_write_cmd(touch_dev->client, SAIN_CLEAR_INT_STATUS_CMD)!=I2C_SUCCESS)		goto fail_mini_init;
	if (ts_write_reg(touch_dev->client, SAIN_X_RESOLUTION, (u16)(touch_dev->cap_info.MaxX))!=I2C_SUCCESS) goto fail_mini_init;
	if (ts_write_reg(touch_dev->client, SAIN_Y_RESOLUTION, (u16)(touch_dev->cap_info.MaxY))!=I2C_SUCCESS) goto fail_mini_init;   
	if (ts_write_reg(touch_dev->client, SAIN_SUPPORTED_FINGER_NUM, (u16)touch_dev->cap_info.multi_fingers)!=I2C_SUCCESS) goto fail_mini_init;	
	reg_val = TOUCH_MODE;
	if (ts_write_reg(touch_dev->client, SAIN_TOUCH_MODE, reg_val)!=I2C_SUCCESS)	goto fail_mini_init;	// soft calibration
	if (ts_write_cmd(touch_dev->client, SAIN_CALIBRATE_CMD)!=I2C_SUCCESS)		goto fail_mini_init;
	if (ts_write_reg(touch_dev->client, SAIN_INT_ENABLE_FLAG, touch_dev->chip_int_mask)!=I2C_SUCCESS)	goto fail_mini_init;		
	//---------------------------------------------------------------------
	// read garbage data
	for(i=0; i<10; i++)
	{
		ts_write_cmd(touch_dev->client, SAIN_CLEAR_INT_STATUS_CMD);
	}

#if	SAIN_ESD_TIMER_INTERVAL	
	if(touch_dev->use_esd_timer)
	{
		if (ts_write_reg(touch_dev->client, SAIN_PERIODICAL_INTERRUPT_INTERVAL, SAIN_SCAN_RATE_HZ*SAIN_ESD_TIMER_INTERVAL)!=I2C_SUCCESS)	
			goto fail_mini_init;
		ts_esd_timer_start(SAIN_ESD_TIMER_INTERVAL*3, touch_dev);
	}
#endif		
	
	return true;
fail_mini_init:
	printk(KERN_ERR "error mini init\n");
	return false;
}


void touch_reset_gpio_set(void)
{
	if(gpio_request(TS_RESET_OUT,"TS_RESET_OUT"))	{
		printk("%s : request port error!\n", __FUNCTION__);
	}
	gpio_direction_output(TS_RESET_OUT, 1);	
	s3c_gpio_setpull(TS_RESET_OUT, S3C_GPIO_PULL_DOWN);
}
#if	SAIN_ESD_TIMER_INTERVAL

static void sain_touch_tmr_work(struct work_struct *work)
{
	sain_touch_dev *touch_dev = container_of(work, sain_touch_dev, tmr_work);	
	
	printk(KERN_INFO "tmr queue work ++\r\n");
	if(touch_dev == NULL)
	{
		printk(KERN_INFO "touch dev == NULL ?\r\n");
		goto fail_time_out_init;
	}
	#if (!USE_THREAD_METHOD)
		cancel_work_sync(&touch_dev->work);
	#endif
	down(&touch_dev->esd_lock);
	disable_irq(touch_dev->client->irq);
	printk(KERN_INFO "error. timeout occured. maybe ts device dead. so reset & reinit.\r\n");
	ts_power_control (0);	//power off
	mdelay(800);		//must
	ts_power_control (1);	//power on
		
	mdelay(500);
	
	if(ts_mini_init_touch(touch_dev) == false)
		goto fail_time_out_init;
	up(&touch_dev->esd_lock);
	enable_irq(touch_dev->client->irq);	
	printk(KERN_INFO "tmr queue work ----\r\n");
	return;
fail_time_out_init:
	printk(KERN_INFO "tmr work : restart error\r\n");	
	ts_esd_timer_start(SAIN_ESD_TIMER_INTERVAL*3, touch_dev);
	up(&touch_dev->esd_lock);
	enable_irq(touch_dev->client->irq);
}

static void ts_esd_timer_start(u16 sec, sain_touch_dev* touch_dev)
{	
	if(touch_dev->p_esd_timeout_tmr != NULL)	del_timer(touch_dev->p_esd_timeout_tmr);
	touch_dev->p_esd_timeout_tmr = NULL;

	init_timer(&(touch_dev->esd_timeout_tmr));
	touch_dev->esd_timeout_tmr.data = (unsigned long)(touch_dev);
   	touch_dev->esd_timeout_tmr.function = ts_esd_timeout_handler;			
	touch_dev->esd_timeout_tmr.expires = jiffies + HZ*sec;	
	touch_dev->p_esd_timeout_tmr = &touch_dev->esd_timeout_tmr;
	add_timer(&touch_dev->esd_timeout_tmr);
}

static void ts_esd_timer_stop(sain_touch_dev* touch_dev)
{
	if(touch_dev->p_esd_timeout_tmr) del_timer(touch_dev->p_esd_timeout_tmr);
	touch_dev->p_esd_timeout_tmr = NULL;
}

static void ts_esd_timer_modify(u16 sec, sain_touch_dev* touch_dev)
{
	mod_timer(&touch_dev->esd_timeout_tmr, jiffies + (HZ*sec));
}

static void ts_esd_timer_init(sain_touch_dev* touch_dev)
{
	init_timer(&(touch_dev->esd_timeout_tmr));
	touch_dev->esd_timeout_tmr.data = (unsigned long)(touch_dev);
   	touch_dev->esd_timeout_tmr.function = ts_esd_timeout_handler;		
	touch_dev->p_esd_timeout_tmr=NULL;
}

static void ts_esd_timeout_handler(unsigned long data)
{
	sain_touch_dev* touch_dev = (sain_touch_dev*)data;
	touch_dev->p_esd_timeout_tmr=NULL;	
	queue_work(sain_tmr_workqueue, &touch_dev->tmr_work);
}
#endif

#if	TOUCH_ONESHOT_UPGRADE
bool ts_check_need_upgrade(u16 curVersion, u16 curRegVersion)
{
	u16	newVersion;
	newVersion = (u16) (m_firmware_data[0] | (m_firmware_data[1]<<8));

	printk(KERN_INFO "cur Version = 0x%x, new Version = 0x%x\n", curVersion, newVersion);
	
	if(curVersion < newVersion)		return true;
	else if(curVersion > newVersion)	return false;
	if(m_firmware_data[0x3FFE] == 0xff && m_firmware_data[0x3FFF] == 0xff)	return false;

	newVersion = (u16) (m_firmware_data[0x3FFE] | (m_firmware_data[0x3FFF]<<8));
			
	if(curRegVersion < newVersion)	return true;

	return false;
}


#define	TC_FIRMWARE_SIZE	(16*1024)
#define	TC_PAGE_SZ		64
#define	TC_SECTOR_SZ		8

u8 ts_upgrade_firmware(sain_touch_dev* touch_dev, const u8 *firmware_data, u32 size)
{
	u16 flash_addr;
	u32 i;
	u8  * verify_data;
	int	retry_cnt = 0;
#if	TOUCH_USING_ISP_METHOD
	u8	i2c_buffer[TC_PAGE_SZ+2];
	unsigned short addr;
#endif	

	verify_data = (u8*)kzalloc(size, GFP_KERNEL);
	if(verify_data == NULL)
	{
		printk(KERN_ERR "cannot alloc verify buffer\n");
		return false;
	}

#if	(TOUCH_USING_ISP_METHOD==0)

	do{
		printk(KERN_INFO "reset command\n");
		if (ts_write_cmd(touch_dev->client, SAIN_REST_CMD)!=I2C_SUCCESS)
		{
			printk(KERN_INFO "failed to reset\n");
			goto fail_upgrade;
		}
		
		printk(KERN_INFO "Erase Flash\n");
		if (ts_write_reg(touch_dev->client, SAIN_ERASE_FLASH, 0xaaaa)!=I2C_SUCCESS)
		{
			printk(KERN_INFO "failed to erase flash\n");
			goto fail_upgrade;
		}
		
		mdelay(500);

		printk(KERN_INFO "writing firmware data\n");			
			
		for(flash_addr= 0; flash_addr< size; )
		{
				
			for(i=0; i< TC_PAGE_SZ/TC_SECTOR_SZ; i++)
			{
				printk(KERN_INFO "addr = %04x, len = %d\n", flash_addr, TC_SECTOR_SZ);
				if(ts_write_data(touch_dev->client,SAIN_WRITE_FLASH,&firmware_data[flash_addr],TC_SECTOR_SZ)<0)
				{
					printk(KERN_INFO"error : write sain tc firmare\n");
					goto fail_upgrade;
				}		
				flash_addr+= TC_SECTOR_SZ;	
				udelay(100);
			}
			mdelay(20);			
		}

		printk(KERN_INFO "read firmware data\n");					
		for(flash_addr= 0; flash_addr< size; )
		{
				
			for(i=0; i< TC_PAGE_SZ/TC_SECTOR_SZ; i++)
			{
				printk(KERN_INFO "addr = %04x, len = %d\n", flash_addr, TC_SECTOR_SZ);
				if(ts_read_firmware_data(touch_dev->client,SAIN_READ_FLASH,&verify_data[flash_addr],TC_SECTOR_SZ)<0)
				{
					printk(KERN_INFO "error : read sain tc firmare\n");
					goto fail_upgrade;
				}		
				flash_addr+= TC_SECTOR_SZ;			
			}
		}

		// verify
		printk(KERN_INFO "verify firmware data\n");
		if(memcmp((u8*)&firmware_data[0], (u8*)&verify_data[0], size) == 0)
		{
			printk(KERN_INFO "upgrade finished\n");
			kfree(verify_data);
			return true;
		}
		printk(KERN_INFO "upgrade fail : so retry... (%d)\n", ++retry_cnt);
		
	}while(1);

fail_upgrade:
	kfree(verify_data);
	return false;
	
#else		//ISP

	
	addr = touch_dev->client->addr;
	// 7 bit eeprom addr
	touch_dev->client->addr = 0x50;

	//must be reset pin low
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++
	ts_power_control(2);	//reset pin low
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++
	
	for(flash_addr= 0; flash_addr< size; flash_addr+=TC_PAGE_SZ)
	{

		printk(KERN_INFO "firmware write : addr = %04x, len = %d\n", flash_addr, TC_PAGE_SZ);
		i2c_buffer[0] = (flash_addr>>8)&0xff;	//addr_h
		i2c_buffer[1] = (flash_addr)&0xff;	//addr_l

		if(ts_write_firmware_data(touch_dev->client, i2c_buffer, TC_PAGE_SZ+2)<0)	
		{
			printk(KERN_INFO"error : write sain tc firmare\n");
			goto fail_upgrade;
		}		
		mdelay(20);			
	}

	printk(KERN_INFO "read firmware data\n");					
	for(flash_addr= 0; flash_addr< size; flash_addr+=TC_PAGE_SZ)
	{
		printk(KERN_INFO "firmware read : addr = %04x, len = %d\n", flash_addr, TC_PAGE_SZ);
		i2c_buffer[0] = (flash_addr>>8)&0xff;	//addr_h
		i2c_buffer[1] = (flash_addr)&0xff;	//addr_l
		if(ts_read_firmware_data(touch_dev->client, i2c_buffer,&verify_data[flash_addr],TC_PAGE_SZ)<0)
		{
			printk(KERN_INFO "error : read sain tc firmare\n");
			goto fail_upgrade;
		}				
	
	}
	// verify
	printk(KERN_INFO "verify firmware data\n");
	if(memcmp((u8*)&firmware_data[0], (u8*)&verify_data[0], size) == 0)
	{
		printk(KERN_INFO "upgrade finished\n");
		//must be reset pin high
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++
		ts_power_control(3);	//reset pin high
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++
		touch_dev->client->addr = addr;
		kfree(verify_data);
		return true;
	}
	printk(KERN_INFO "upgrade fail : so retry... (%d)\n", ++retry_cnt);


fail_upgrade:
	//must be reset pin high
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++
	ts_power_control(3);	//reset pin high
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++
	touch_dev->client->addr = addr;
	kfree(verify_data);
	return false;

	
#endif


	
}


#endif


bool ts_init_touch(sain_touch_dev* touch_dev)
{
	u16	reg_val;
	int	i;
	u16 	SetMaxX = SYSTEM_MAX_X_RESOLUTION; //Max Position range from 0x0002 to 0x1fff
	u16 	SetMaxY = SYSTEM_MAX_Y_RESOLUTION; //Max Position range from 0x0002 to 0x1fff  
	u16	SupportedFingerNum = SUPPORTED_FINGER_NUM;

	u16 	CurMaxX =1024;//800;// 
	u16 	CurMaxY =1920;//480;//

	unsigned char test_val=0;
     

	u16 	chip_revision;
	u16 	chip_firmware_version;
	u16	chip_reg_data_version;
	u16	chip_eeprom_info;
	s16	stmp;
	int ret_val=0;
	s32 temp;

	sain_debug_msg("sain touch init +\r\n");

	if(touch_dev == NULL)
	{
		printk(KERN_ERR "error touch_dev == null?\r\n");
		goto fail_init;
	}

	sain_debug_msg("disable interrupt\r\n");

	for(i=0; i<SAIN_INIT_RETRY_CNT; i++)
	{
		if (ts_write_reg(touch_dev->client, SAIN_INT_ENABLE_FLAG, 0x0)==I2C_SUCCESS){printk(KERN_INFO "[%s:%d]I2C Success\n", __FUNCTION__, __LINE__);break;}
		mdelay(10);
	}	

	if(i==SAIN_INIT_RETRY_CNT)
	{
		printk(KERN_INFO "fail to write interrupt register\r\n");
		goto fail_init;
	}
	sain_debug_msg("successfully disable int. (retry cnt = %d)\r\n", i);

	sain_debug_msg("send reset command\r\n");
	if (ts_write_cmd(touch_dev->client, SAIN_REST_CMD)!=I2C_SUCCESS){	
		goto fail_init;
	}
	/* get chip revision id */
	if (ts_read_data(touch_dev->client, SAIN_CHIP_REVISION, (u8*)&chip_revision, 2)<0)
	{
		printk(KERN_INFO "fail to read chip revision\r\n");
		goto fail_init;
	}	
	printk(KERN_INFO "sain touch chip revision id = %x\r\n", chip_revision);
	touch_dev->cap_info.chip_fw_size = 16*1024;
	if(chip_firmware_version >=0x0a && chip_firmware_version <= 0x0b)
		touch_dev->cap_info.chip_fw_size = 16*1024;

	touch_dev->cap_info.multi_fingers = SUPPORTED_FINGER_NUM;	


	/* get chip firmware version */
	if (ts_read_data(touch_dev->client, SAIN_FIRMWARE_VERSION, (u8*)&chip_firmware_version, 2)<0){
		goto fail_init;
	}
	
	printk(KERN_INFO "sain touch chip firmware version = %x\r\n", chip_firmware_version);
	
#if	TOUCH_ONESHOT_UPGRADE
	chip_reg_data_version = 0xffff;
	if(chip_revision >= 0x0a && chip_firmware_version >= 0x68)
	{
		if (ts_read_data(touch_dev->client, SAIN_DATA_VERSION_REG, (u8*)&chip_reg_data_version, 2)<0){
			goto fail_init;
		}
		sain_debug_msg("touch reg data version = %d\r\n", chip_reg_data_version);
		printk(KERN_INFO "touch reg data version = %d\r\n", chip_reg_data_version);
	}
	
 	if(ts_check_need_upgrade(chip_firmware_version, chip_reg_data_version)==true)
   	{
   		printk(KERN_INFO "start upgrade firmware\n");
		ts_upgrade_firmware(touch_dev, &m_firmware_data[2], touch_dev->cap_info.chip_fw_size);
		mdelay(10);  

		// must h/w reset (cold reset) and mdelay(500); 
		ts_power_control (0);	//power off
		mdelay(800);	
		ts_power_control (1);	//power on	
		mdelay(500);
	
		/* get chip revision id */
		if (ts_read_data(touch_dev->client, SAIN_CHIP_REVISION, (u8*)&chip_revision, 2)<0)
		{
			printk(KERN_INFO "fail to read chip revision\r\n");
			goto fail_init;
		}		
		printk(KERN_INFO "sain touch chip revision id = %x\r\n", chip_revision);
		
		/* get chip firmware version */
		if (ts_read_data(touch_dev->client, SAIN_FIRMWARE_VERSION, (u8*)&chip_firmware_version, 2)<0) {
			goto fail_init;		
		}
		printk(KERN_INFO "sain touch chip renewed firmware version = %x\r\n", chip_firmware_version);


		if(chip_revision < 0x0a || chip_firmware_version < 0x68)
		{
		 // h/w calibration
			if (ts_write_reg(touch_dev->client, SAIN_TOUCH_MODE, 0x07)!=I2C_SUCCESS){
				 goto fail_init;}
			if (ts_write_cmd(touch_dev->client, SAIN_CALIBRATE_CMD)!=I2C_SUCCESS){	goto fail_init;}
			if (ts_write_cmd(touch_dev->client, SAIN_REST_CMD)!=I2C_SUCCESS){	goto fail_init;}
			
			mdelay(3000); 
			//<----------------------------------------------------------

			if (ts_write_reg(touch_dev->client, SAIN_TOUCH_MODE, 0x00)!=I2C_SUCCESS){ goto fail_init;}
			if (ts_write_cmd(touch_dev->client, SAIN_REST_CMD)!=I2C_SUCCESS){ goto fail_init;}
 		}
 
	  }
 #endif

	

	if(chip_revision >= 0x0a && chip_firmware_version >= 0x68)
	{
		if (ts_read_data(touch_dev->client, SAIN_DATA_VERSION_REG, (u8*)&chip_reg_data_version, 2)<0){ goto fail_init;}
//		sain_debug_msg("touch reg data version = %d\r\n", chip_reg_data_version);

		if(chip_reg_data_version < m_reg_data[SAIN_DATA_VERSION_REG].reg_val)
		{
			sain_debug_msg("write new reg data( %d < %d)\r\n", chip_reg_data_version, m_reg_data[SAIN_DATA_VERSION_REG].reg_val);
			for(i=0; i < MAX_REG_COUNT; i++)
			{
				if(m_reg_data[i].valid == 1)
				{
					if(ts_write_reg(touch_dev->client, (u16)i, (u16)(m_reg_data[i].reg_val))!=I2C_SUCCESS){ goto fail_init;}
					if(i == SAIN_TOTAL_NUMBER_OF_X || i == SAIN_TOTAL_NUMBER_OF_Y ||i == SAIN_AFE_FREQUENCY)	mdelay(50);	//for clear hw calibration bit
					if(ts_read_data(touch_dev->client, (u16)i, &stmp, 2)<0){ goto fail_init;}
					if(memcmp((char*)&m_reg_data[i].reg_val, (char*)&stmp, 2)!=0)		//if(m_reg_data[i].reg_val != stmp)
						printk(KERN_WARNING "register data is different. (addr = 0x%02X , %d != %d)\r\n", i, m_reg_data[i].reg_val, stmp);					
				}
			}
			sain_debug_msg("done new reg data( %d < %d)\r\n", chip_reg_data_version, m_reg_data[SAIN_DATA_VERSION_REG].reg_val);		
			if (ts_write_cmd(touch_dev->client, SAIN_SAVE_STATUS_CMD)!=I2C_SUCCESS){ goto fail_init;			}
			mdelay(1000);	// for fusing eeprom
		}
		if (ts_read_data(touch_dev->client, SAIN_EEPROM_INFO_REG, (u8*)&chip_eeprom_info, 2)<0){ goto fail_init;}
		sain_debug_msg("touch eeprom info = 0x%04X\r\n", chip_eeprom_info);

		if(sain_bit_test(chip_eeprom_info, 0))		// hw calibration bit
		{
		 // h/w calibration
			if (ts_write_reg(touch_dev->client, SAIN_TOUCH_MODE, 0x07)!=I2C_SUCCESS){ goto fail_init;}
			if (ts_write_cmd(touch_dev->client, SAIN_CALIBRATE_CMD)!=I2C_SUCCESS){	goto fail_init;}
			if (ts_write_cmd(touch_dev->client, SAIN_REST_CMD)!=I2C_SUCCESS){	goto fail_init;}
			mdelay(1); 		
			ts_write_cmd(touch_dev->client, SAIN_CLEAR_INT_STATUS_CMD);
			// wait for h/w calibration
			do{
				mdelay(1000); 
				if (ts_read_data(touch_dev->client, SAIN_EEPROM_INFO_REG, (u8*)&chip_eeprom_info, 2)<0){ goto fail_init;}
				sain_debug_msg("touch eeprom info = 0x%04X\r\n", chip_eeprom_info);
				if(!sain_bit_test(chip_eeprom_info, 0))	break;
			}while(1);
			//<----------------------------------------------------------
			if (ts_write_reg(touch_dev->client, SAIN_TOUCH_MODE, 0x00)!=I2C_SUCCESS) {	goto fail_init;}
			if (ts_write_cmd(touch_dev->client, SAIN_REST_CMD)!=I2C_SUCCESS) {	goto fail_init;}
			if (ts_write_cmd(touch_dev->client, SAIN_SAVE_STATUS_CMD)!=I2C_SUCCESS) {	goto fail_init;}
			mdelay(1000);	// for fusing eeprom
			if (ts_write_cmd(touch_dev->client, SAIN_REST_CMD)!=I2C_SUCCESS){	goto fail_init;}
		}
	}

	touch_dev->cap_info.chip_revision = (u16)chip_revision;
	touch_dev->cap_info.chip_firmware_version = (u16)chip_firmware_version;
	touch_dev->cap_info.chip_reg_data_version = (u16)chip_reg_data_version;
 
	/* initialize */	
	if (ts_write_reg(touch_dev->client, SAIN_X_RESOLUTION, (u16)(SetMaxX))!=I2C_SUCCESS) {	goto fail_init;}
	if (ts_write_reg(touch_dev->client, SAIN_Y_RESOLUTION, (u16)(SetMaxY))!=I2C_SUCCESS) {	goto fail_init;}

	if (ts_read_data(touch_dev->client, SAIN_X_RESOLUTION, (u8*)&CurMaxX, 2)<0) {
		goto fail_init;
	}
	sain_debug_msg("test_val touch max x = %d\r\n", test_val);
	if (ts_read_data(touch_dev->client, SAIN_Y_RESOLUTION,(u8*)&CurMaxY, 2)<0)
		goto fail_init;
	sain_debug_msg("touch max y = %d\r\n", CurMaxY);
	touch_dev->cap_info.MinX = (u32)0;
	touch_dev->cap_info.MinY = (u32)0;
	touch_dev->cap_info.MaxX = (u32)CurMaxX;
	touch_dev->cap_info.MaxY = (u32)CurMaxY;

	if(touch_dev->cap_info.chip_revision >= 0x0a && touch_dev->cap_info.chip_firmware_version >= 0x75)
	{
		if (ts_write_reg(touch_dev->client, SAIN_SUPPORTED_FINGER_NUM, (u16)SupportedFingerNum)!=I2C_SUCCESS) {	goto fail_init;}
		if (ts_read_data(touch_dev->client, SAIN_SUPPORTED_FINGER_NUM, (u8*)&SupportedFingerNum, 2)<0) {	goto fail_init;}
		sain_debug_msg("supported finger num = %d\r\n", SupportedFingerNum); 
	}	
	
	touch_dev->cap_info.gesture_support = 0;
	touch_dev->cap_info.multi_fingers = SupportedFingerNum;
    		
	sain_debug_msg("set other configuration\r\n");

	reg_val = TOUCH_MODE;
	if (ts_write_reg(touch_dev->client, SAIN_TOUCH_MODE, reg_val)!=I2C_SUCCESS)	{	goto fail_init;}
	// soft calibration
	if (ts_write_cmd(touch_dev->client, SAIN_CALIBRATE_CMD)!=I2C_SUCCESS)		{	goto fail_init;}
	
	reg_val = 0;
	sain_bit_set(reg_val, BIT_PT_CNT_CHANGE);			
	sain_bit_set(reg_val, BIT_DOWN);
	sain_bit_set(reg_val, BIT_MOVE);
	sain_bit_set(reg_val, BIT_UP);
	
	if(SUPPORTED_BUTTON_NUM > 0)		sain_bit_set(reg_val, BIT_ICON_EVENT);
	
	touch_dev->chip_int_mask = reg_val;
	if (ts_write_reg(touch_dev->client, SAIN_INT_ENABLE_FLAG, touch_dev->chip_int_mask)!=I2C_SUCCESS)	{	goto fail_init;}
	//---------------------------------------------------------------------
	// read garbage data
	for(i=0; i<10; i++)
	{
		ts_write_cmd(touch_dev->client, SAIN_CLEAR_INT_STATUS_CMD);
	}
#if	SAIN_ESD_TIMER_INTERVAL	
	if(touch_dev->cap_info.chip_revision >= 0x0a && touch_dev->cap_info.chip_firmware_version >= 0x69)
	{
		if (ts_write_reg(touch_dev->client, SAIN_PERIODICAL_INTERRUPT_INTERVAL, SAIN_SCAN_RATE_HZ*SAIN_ESD_TIMER_INTERVAL)!=I2C_SUCCESS)	{	goto fail_init;}
    		sema_init(&touch_dev->esd_lock, 1);
		touch_dev->use_esd_timer = 1;
		ts_esd_timer_init(touch_dev);	
		ts_esd_timer_start(SAIN_ESD_TIMER_INTERVAL*3, touch_dev);	
		printk(KERN_INFO " ts_esd_timer_start\n");	
	}
	else
		touch_dev->use_esd_timer = 0;
#endif


	sain_debug_msg("successfully initialized\r\n");
	return true;
	
fail_init:
	sain_debug_msg("failed initiallize\r\n");
	if (ts_write_cmd(touch_dev->client, SAIN_REST_CMD)!=I2C_SUCCESS);
	return false;

}


#if (TOUCH_MODE == 1)
static void	sain_report_data(sain_touch_dev *touch_dev, int id)
{
	int i;
	u32 x, y, width;
	u32 tmp;

	if(id >= SUPPORTED_FINGER_NUM || id < 0)
	{
		return;
	}

	x = touch_dev->touch_info.coord[id].x;
	y = touch_dev->touch_info.coord[id].y;

	
	 /* transformation from touch to screen orientation */
	if (touch_dev->cap_info.Orientation & TOUCH_V_FLIP)
	{
		y = touch_dev->cap_info.MaxY + touch_dev->cap_info.MinY - y;			               
	}
	if (touch_dev->cap_info.Orientation & TOUCH_H_FLIP)
	{
		x = touch_dev->cap_info.MaxX + touch_dev->cap_info.MinX - x;			               
	}
	if (touch_dev->cap_info.Orientation & TOUCH_XY_SWAP)
	{	
		swap_v(x, y, tmp);
	}

	touch_dev->reported_touch_info.coord[id].x = x;
	touch_dev->reported_touch_info.coord[id].y = y;				
	touch_dev->reported_touch_info.coord[id].width = touch_dev->touch_info.coord[id].width;
	touch_dev->reported_touch_info.coord[id].sub_status = touch_dev->touch_info.coord[id].sub_status;

	for(i=0; i< SUPPORTED_FINGER_NUM; i++)
	{
		if(sain_bit_test(touch_dev->reported_touch_info.coord[i].sub_status, SUB_BIT_EXIST)
			||sain_bit_test(touch_dev->reported_touch_info.coord[i].sub_status, SUB_BIT_DOWN)
			||sain_bit_test(touch_dev->reported_touch_info.coord[i].sub_status, SUB_BIT_MOVE))			
		{
//
			if(touch_dev->reported_touch_info.coord[i].width == 0)	touch_dev->reported_touch_info.coord[i].width = 5;
			input_report_abs(touch_dev->input_dev, ABS_MT_TOUCH_MAJOR, (u32)touch_dev->reported_touch_info.coord[i].width);
			input_report_abs(touch_dev->input_dev, ABS_MT_WIDTH_MAJOR, (u32)touch_dev->reported_touch_info.coord[i].width);					
			input_report_abs(touch_dev->input_dev, ABS_MT_POSITION_X, touch_dev->reported_touch_info.coord[i].x);
			input_report_abs(touch_dev->input_dev, ABS_MT_POSITION_Y, touch_dev->reported_touch_info.coord[i].y);
			input_mt_sync(touch_dev->input_dev);	
		}
		else if(sain_bit_test(touch_dev->reported_touch_info.coord[i].sub_status, SUB_BIT_UP))			
		{
//
			input_report_abs(touch_dev->input_dev, ABS_MT_TOUCH_MAJOR, 0);
			input_report_abs(touch_dev->input_dev, ABS_MT_WIDTH_MAJOR, 0);	
			input_report_abs(touch_dev->input_dev, ABS_MT_POSITION_X, touch_dev->reported_touch_info.coord[i].x);
			input_report_abs(touch_dev->input_dev, ABS_MT_POSITION_Y, touch_dev->reported_touch_info.coord[i].y);

		        input_mt_sync(touch_dev->input_dev);	
			touch_dev->reported_touch_info.coord[i].sub_status = 0;
		}
		else{
			touch_dev->reported_touch_info.coord[i].sub_status = 0;
		}
	}

	input_sync(touch_dev->input_dev);
}
#endif	// TOUCH_MODE == 1


#if USE_THREAD_METHOD	
static int sain_touch_thread(void *pdata)
#else
static void sain_touch_work(struct work_struct *work)
#endif	// USE_THREAD_METHOD
{
	bool read_coord_continued;
	int i;
	u32 x, y, width;
	u32 tmp;
	u8 reported = false;
#if USE_THREAD_METHOD
	sain_touch_dev *touch_dev = (sain_touch_dev*)pdata;   
#if 0
	struct task_struct *tsk = current;
	//struct sched_param param = { .sched_priority = MAX_RT_PRIO-1 };
	struct sched_param param = { .sched_priority = 1 };
	
	sched_setscheduler(tsk, SCHED_FIFO, &param);
#endif
	printk(KERN_INFO "touch thread started.. \r\n");
#else
	sain_touch_dev *touch_dev = container_of(work, sain_touch_dev, work);
#endif

#if USE_THREAD_METHOD	
	for (;;)
	{
		down(&touch_dev->update_lock);
#endif	// USE_THREAD_METHOD

		sain_debug_msg("sain_touch_thread : semaphore signalled\r\n");
		
#if	SAIN_ESD_TIMER_INTERVAL	
		if(touch_dev->use_esd_timer)
		{
		    	down(&touch_dev->esd_lock);
			ts_esd_timer_stop(touch_dev);	
		}
#endif				
		read_coord_continued = true;
		do
		{
			if (ts_read_coord(touch_dev)==false)
			{
//				printk(KERN_WARNING "couldn't read touch_dev sample\r\n");
				goto continue_read_samples;
			}

#if (TOUCH_MODE == 1)
			// invalid : maybe periodical repeated int.
			if(touch_dev->touch_info.status == 0x0)	{		
				goto continue_read_samples;
			}
#else
			// invalid : maybe periodical repeated int.
			if(touch_dev->touch_info.status == 0x0 && touch_dev->touch_info.finger_cnt == 100){
				goto continue_read_samples;
			}
#endif	//TOUCH_MODE == 1
			reported = false;

			if(sain_bit_test(touch_dev->touch_info.status, BIT_ICON_EVENT))
			{
				for(i=0; i<SUPPORTED_BUTTON_NUM; i++)
				{
					if(sain_bit_test(touch_dev->icon_event_reg, (BIT_O_ICON0_DOWN+i)))
					{
						touch_dev->button[i] = ICON_BUTTON_DOWN;
						input_report_key(touch_dev->input_dev, BUTTON_MAPPING_KEY[i], 1);					
						reported = true;						
						sain_debug_msg("button down = %d \r\n", i);
					}
				}

				for(i=0; i<SUPPORTED_BUTTON_NUM; i++)
				{
					if(sain_bit_test(touch_dev->icon_event_reg, (BIT_O_ICON0_UP+i)))
					{
						touch_dev->button[i] = ICON_BUTTON_UP;	
						input_report_key(touch_dev->input_dev, BUTTON_MAPPING_KEY[i], 0);					
						reported = true;		
						sain_debug_msg("button up = %d \r\n", i);
					}
				}
			}

			// if button press or up event occured...
			if(reported == true)
			{
#if (TOUCH_MODE == 1)
				//input_sync(touch_dev->input_dev);
				for(i=0; i< SUPPORTED_FINGER_NUM; i++)
				{

					if(sain_bit_test(touch_dev->reported_touch_info.coord[i].sub_status, SUB_BIT_EXIST))			
					{
						input_report_abs(touch_dev->input_dev, ABS_MT_TOUCH_MAJOR, 0);
						input_report_abs(touch_dev->input_dev, ABS_MT_WIDTH_MAJOR, 0);	
						input_report_abs(touch_dev->input_dev, ABS_MT_POSITION_X, touch_dev->reported_touch_info.coord[i].x);
						input_report_abs(touch_dev->input_dev, ABS_MT_POSITION_Y, touch_dev->reported_touch_info.coord[i].y);	
					        input_mt_sync(touch_dev->input_dev);	
					}
					touch_dev->reported_touch_info.coord[i].sub_status = 0;
				}
				input_sync(touch_dev->input_dev);
				//goto continue_read_samples;
			}
#else					
//
				for(i=0; i< SUPPORTED_FINGER_NUM; i++)
				{

					if(sain_bit_test(touch_dev->reported_touch_info.coord[i].sub_status, SUB_BIT_EXIST))			
					{

						//input_report_abs(touch_dev->input_dev,ABS_MT_TRACKING_ID,i);
						input_report_abs(touch_dev->input_dev, ABS_MT_TOUCH_MAJOR, 0);
						input_report_abs(touch_dev->input_dev, ABS_MT_WIDTH_MAJOR, 0);	
						input_report_abs(touch_dev->input_dev, ABS_MT_POSITION_X, touch_dev->reported_touch_info.coord[i].x);
						input_report_abs(touch_dev->input_dev, ABS_MT_POSITION_Y, touch_dev->reported_touch_info.coord[i].y);												
					        input_mt_sync(touch_dev->input_dev);	
					}
				}
				memset(&touch_dev->reported_touch_info, 0x0, sizeof(_ts_sain_point_info));				
				input_sync(touch_dev->input_dev);
				udelay(100);				
				goto continue_read_samples;				
			}


			if (touch_dev->touch_info.finger_cnt > SUPPORTED_FINGER_NUM){
				touch_dev->touch_info.finger_cnt = SUPPORTED_FINGER_NUM;
			}

			if(!sain_bit_test(touch_dev->touch_info.status, BIT_PT_EXIST))
			{
			
				for(i=0; i< SUPPORTED_FINGER_NUM; i++)
				{
					if(sain_bit_test(touch_dev->reported_touch_info.coord[i].sub_status, SUB_BIT_EXIST))
					{
						//input_report_abs(touch_dev->input_dev,ABS_MT_TRACKING_ID,i);
						input_report_abs(touch_dev->input_dev, ABS_MT_TOUCH_MAJOR, 0);
						input_report_abs(touch_dev->input_dev, ABS_MT_WIDTH_MAJOR, 0);	
						input_report_abs(touch_dev->input_dev, ABS_MT_POSITION_X, touch_dev->reported_touch_info.coord[i].x);
						input_report_abs(touch_dev->input_dev, ABS_MT_POSITION_Y, touch_dev->reported_touch_info.coord[i].y);						
					        input_mt_sync(touch_dev->input_dev);	
					}
				}				
				memset(&touch_dev->reported_touch_info, 0x0, sizeof(_ts_sain_point_info));					
				input_sync(touch_dev->input_dev);
				goto continue_read_samples;
			}
		

			for(i=0; i< SUPPORTED_FINGER_NUM; i++)
			{
					
				if(sain_bit_test(touch_dev->touch_info.coord[i].sub_status, SUB_BIT_DOWN)
					|| sain_bit_test(touch_dev->touch_info.coord[i].sub_status, SUB_BIT_MOVE)
					|| sain_bit_test(touch_dev->touch_info.coord[i].sub_status, SUB_BIT_EXIST))				
				{
					x = touch_dev->touch_info.coord[i].x;
					y = touch_dev->touch_info.coord[i].y;
					
					 /* transformation from touch to screen orientation */
					if (touch_dev->cap_info.Orientation & TOUCH_V_FLIP)
					{
						y = touch_dev->cap_info.MaxY + touch_dev->cap_info.MinY - y;			               
					}
					if (touch_dev->cap_info.Orientation & TOUCH_H_FLIP)
					{
						x = touch_dev->cap_info.MaxX + touch_dev->cap_info.MinX - x;			               
					}
					if (touch_dev->cap_info.Orientation & TOUCH_XY_SWAP)
					{					
				                 swap_v(x, y, tmp);
					}
					touch_dev->touch_info.coord[i].x = x;
					touch_dev->touch_info.coord[i].y = y;

					sain_debug_msg("finger [%02d] x = %d, y = %d \r\n", i, x, y);

			
					//input_report_abs(touch_dev->input_dev,ABS_MT_TRACKING_ID,i);
					if(touch_dev->touch_info.coord[i].width == 0)	touch_dev->touch_info.coord[i].width = 5;
					input_report_abs(touch_dev->input_dev, ABS_MT_TOUCH_MAJOR, (u32)touch_dev->touch_info.coord[i].width);
					input_report_abs(touch_dev->input_dev, ABS_MT_WIDTH_MAJOR, (u32)touch_dev->touch_info.coord[i].width);					
					input_report_abs(touch_dev->input_dev, ABS_MT_POSITION_X, x);
				        input_report_abs(touch_dev->input_dev, ABS_MT_POSITION_Y, y);
				        input_mt_sync(touch_dev->input_dev);	


				}
				else if(sain_bit_test(touch_dev->touch_info.coord[i].sub_status, SUB_BIT_UP))			
				{
					sain_debug_msg("finger [%02d] up \r\n", i);
					memset(&touch_dev->touch_info.coord[i], 0x0, sizeof(_ts_sain_coord));	
					//input_report_abs(touch_dev->input_dev, ABS_MT_TRACKING_ID,i);
					input_report_abs(touch_dev->input_dev, ABS_MT_TOUCH_MAJOR, 0);
					input_report_abs(touch_dev->input_dev, ABS_MT_WIDTH_MAJOR, 0);	
					input_report_abs(touch_dev->input_dev, ABS_MT_POSITION_X, touch_dev->reported_touch_info.coord[i].x);
					input_report_abs(touch_dev->input_dev, ABS_MT_POSITION_Y, touch_dev->reported_touch_info.coord[i].y);
					input_mt_sync(touch_dev->input_dev);

				}		

				else
					memset(&touch_dev->touch_info.coord[i], 0x0, sizeof(_ts_sain_coord));	
	

			}		        
			memcpy((char*)&touch_dev->reported_touch_info, (char*)&touch_dev->touch_info, sizeof(_ts_sain_point_info));	
			input_sync(touch_dev->input_dev);
			
#endif	// TOUCH_MODE == 1
		continue_read_samples:
			
			//check_interrupt_pin, if high, enable int & wait signal
			if (gpio_get_value(touch_dev->int_gpio_num))
			{
				read_coord_continued = false;				
				enable_irq(touch_dev->client->irq);
#if	SAIN_ESD_TIMER_INTERVAL					
				if(touch_dev->use_esd_timer)
				{
					ts_esd_timer_start(SAIN_ESD_TIMER_INTERVAL*3, touch_dev);	
				    	up(&touch_dev->esd_lock);				    	
				}
#endif				
			} 
			else
			{
				sain_debug_msg("interrupt pin is still low, so continue read \r\n");			
			}
				
		}while(read_coord_continued);
#if USE_THREAD_METHOD	
	}
    return 0;
#endif
}

#ifdef CONFIG_HAS_EARLYSUSPEND
static void sain_late_resume(struct early_suspend *h)
{

	sain_touch_dev * touch_dev;
	touch_dev = container_of(h, sain_touch_dev, early_suspend);
	printk(KERN_INFO "resume++\r\n");
	if(touch_dev == NULL)	return;

	ts_write_cmd(touch_dev->client, SAIN_WAKEUP_CMD);
	mdelay(10);	
	if(ts_mini_init_touch(touch_dev)==false)	goto fail_resume;
	printk(KERN_INFO "wakeup\n");	
	enable_irq(touch_dev->client->irq);
	return;	
fail_resume:
	printk(KERN_ERR "failed to wakeup\n");
	enable_irq(touch_dev->client->irq);
	return;
}


static void sain_early_suspend(struct early_suspend *h)
{
	sain_touch_dev * touch_dev;
	touch_dev = container_of(h, sain_touch_dev, early_suspend);
	if(touch_dev == NULL)	return;
	
	printk(KERN_INFO "suspend++\n");
	disable_irq(touch_dev->client->irq);
	
#if	SAIN_ESD_TIMER_INTERVAL	
	if(touch_dev->use_esd_timer)
	{				
		down(&touch_dev->esd_lock);	
		ts_write_reg(touch_dev->client, SAIN_PERIODICAL_INTERRUPT_INTERVAL, 0);
		ts_esd_timer_stop(touch_dev);
		up(&touch_dev->esd_lock);		
		printk(KERN_INFO " ts_esd_timer_stop\n");			
	}
#endif		

#if (!USE_THREAD_METHOD)
	cancel_work_sync(&touch_dev->work);
#endif
	
	ts_write_reg(touch_dev->client, SAIN_INT_ENABLE_FLAG, 0x0);
	udelay(100);
	ts_write_cmd(touch_dev->client, SAIN_CLEAR_INT_STATUS_CMD);
	if (ts_write_cmd(touch_dev->client, SAIN_SLEEP_CMD)!=I2C_SUCCESS)
	{
		printk(KERN_ERR "failed to enter into sleep mode\n");
		return;
	}	
	
	return;
}

#endif	// CONFIG_HAS_EARLYSUSPEND



#if	USE_UPDATE_SYSFS

static ssize_t sain_show_info(struct device *dev,
				struct device_attribute *attr,
                                const char *buf)
{
	sain_touch_dev* touch_dev = dev_get_drvdata(dev);
	int count = 0;
	count += sprintf(buf, "sain ts chip info : revision = 0x%04x, fw version = 0x%04x, reg data version = 0x%04x\n",
		touch_dev->cap_info.chip_revision, touch_dev->cap_info.chip_firmware_version, touch_dev->cap_info.chip_reg_data_version);
	return count;
}

static ssize_t sain_update_fw(struct device *dev,
				struct device_attribute *attr,
                                const char *buf, size_t count)
{
	sain_touch_dev* touch_dev = dev_get_drvdata(dev);
	char *filename_ptr;
	int ret = 0;
	const struct firmware *fw = NULL;

	filename_ptr = kzalloc(count*2, GFP_KERNEL);

	if (!filename_ptr) {
		dev_err(dev, "cannot alloc memory\n");
		return -EINVAL;
	}

	ret = sscanf(buf, "%s", filename_ptr);
	if (ret != 1) {
		dev_err(dev, "invalid parameter\n");
		goto fail_fw;
	}

	ret = request_firmware(&fw, filename_ptr, dev);
	if (ret) {
		dev_err(dev, "cannot open firmware %s\n", filename_ptr);
		goto fail_fw;
	}

	if(fw->size != touch_dev->cap_info.chip_fw_size)
	{
		dev_err(dev, "invalid file size( %d != %d)\n", fw->size , touch_dev->cap_info.chip_fw_size);
		goto fail_upgrade_fw;
	}

	disable_irq(touch_dev->client->irq);
#if	SAIN_ESD_TIMER_INTERVAL	
	if(touch_dev->use_esd_timer)
	{
		ts_esd_timer_stop(touch_dev);
	}
#endif					
	dev_dbg(dev, "update firmware\n");
	ts_upgrade_firmware(touch_dev, fw, fw->size);

	dev_dbg(dev, "reset touch chip\n");	
	// must h/w reset (cold reset) and mdelay(500); 
	ts_power_control (0);	//power off
	mdelay(800);		//must
	ts_power_control (1);	//power on	
	mdelay(500);

	dev_dbg(dev, "initialize touch chip, do not touch.\n");	
	ts_init_touch(touch_dev);
	enable_irq(touch_dev->client->irq);
	release_firmware(fw);
	dev_dbg(dev, "finished update.\n");		
	return count;
fail_upgrade_fw:
	release_firmware(fw);	
fail_fw:
	kfree(filename_ptr);
	return -1;

}

static DEVICE_ATTR(sain_fw_manager, S_IRUGO|S_IWUGO, sain_show_info, sain_update_fw);

static struct attribute *sain_attrs[] = {
         &dev_sain_fw_manager.attr,
 	NULL
};

static const struct attribute_group sain_attr_group = 
{
        .attrs = sain_attrs,
};

#endif	// USE_UPDATE_SYSFS

static int sain_touch_probe(struct i2c_client *client, const struct i2c_device_id *i2c_id)
{
	int ret;
	sain_touch_dev* touch_dev;
	int i;

  	sain_debug_msg("sain_touch_probe+ \r\n");

	achro4210_touch_port_init();

  	sain_debug_msg("i2c check function \r\n");	
	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		printk(KERN_ERR "error : not compatible i2c function \r\n");
		ret = -ENODEV;
		goto err_check_functionality;
	}

  	sain_debug_msg("touch data alloc \r\n");	
	touch_dev = kzalloc(sizeof(sain_touch_dev), GFP_KERNEL);
	if (!touch_dev) {
	  	printk(KERN_ERR "unabled to allocate touch data \r\n");	
		ret = -ENOMEM;
		goto err_alloc_dev_data;
	}
	touch_dev->client = client;
	i2c_set_clientdata(client, touch_dev);

#if USE_THREAD_METHOD		
    	sema_init(&touch_dev->update_lock, 0);
#else
	INIT_WORK(&touch_dev->work, sain_touch_work);
#endif	// USE_THREAD_METHOD

	if(I2C_SMBUS_BLOCK_MAX < sizeof(_ts_sain_point_info))
	{
		printk(KERN_WARNING "max size error : i2c max size = %d, sain packet size = %d \r\n", I2C_SMBUS_BLOCK_MAX, sizeof(_ts_sain_point_info));	
		printk(KERN_WARNING "must modify I2C_SMBUS_BLOCK_MAX field in include/linux/i2c.h\r\n");
	}
	
#if USE_THREAD_METHOD		
	sain_debug_msg("touch thread create \r\n");	
	touch_dev->task = kthread_create(sain_touch_thread, touch_dev, "sain_touch_thread");
	if(touch_dev->task == NULL)
	{
		printk(KERN_ERR "unabled to create touch thread \r\n");
		ret = -1;
		goto err_kthread_create_failed;
	}	
#else
	sain_workqueue = create_singlethread_workqueue("sain_workqueue");
	if (!sain_workqueue)
	{
		printk(KERN_ERR "unabled to create touch thread \r\n");
		ret = -1;
		goto err_kthread_create_failed;
	}
#endif

#if	SAIN_ESD_TIMER_INTERVAL
	INIT_WORK(&touch_dev->tmr_work, sain_touch_tmr_work);
	
	sain_tmr_workqueue = create_singlethread_workqueue("sain_tmr_workqueue");
	if (!sain_tmr_workqueue)
	{
		printk(KERN_ERR "unabled to create touch tmr work queue \r\n");
		ret = -1;
		goto err_kthread_create_failed;
	}
#endif
	//wake_up_process( touch_dev->task );
	sain_debug_msg("allocate input device \r\n");
	touch_dev->input_dev = input_allocate_device();
	if (touch_dev->input_dev == 0) {
		printk(KERN_ERR "unabled to allocate input device \r\n");
		ret = -ENOMEM;
		goto err_input_allocate_device;
	}

	//initialize sain touch ic
	touch_dev->int_gpio_num = GPIO_TOUCH_PIN_NUM;	// for upgrade	

	memset(&touch_dev->reported_touch_info, 0x0, sizeof(_ts_sain_point_info));
	ts_init_touch(touch_dev);	
	
	touch_dev->input_dev->name = SAIN_DRIVER_NAME;
	//touch_dev->input_dev->phys = "sain_touch/input0";	// <- for compatability
   	touch_dev->input_dev->id.bustype = BUS_I2C;
   	touch_dev->input_dev->id.vendor = 0x0001;

   	touch_dev->input_dev->id.product = 0x0002;
   	touch_dev->input_dev->id.version = 0x0100;
	touch_dev->input_dev->dev.parent = &client->dev;
    
	set_bit(EV_SYN, touch_dev->input_dev->evbit);
	set_bit(EV_KEY, touch_dev->input_dev->evbit);
	set_bit(BTN_TOUCH, touch_dev->input_dev->keybit);
	set_bit(EV_ABS, touch_dev->input_dev->evbit);

	//HISEOB : Override Orientation
	touch_dev->cap_info.Orientation = 0;

	if(SUPPORTED_BUTTON_NUM > 0)
	{
		for(i=0; i< SUPPORTED_BUTTON_NUM; i++)	
	    		set_bit(BUTTON_MAPPING_KEY[i], touch_dev->input_dev->keybit);
	}	


	if (touch_dev->cap_info.Orientation & TOUCH_XY_SWAP)
	{
		input_set_abs_params(touch_dev->input_dev, ABS_MT_POSITION_Y, touch_dev->cap_info.MinX, touch_dev->cap_info.MaxX, 0, 0);		
		input_set_abs_params(touch_dev->input_dev, ABS_MT_POSITION_X, touch_dev->cap_info.MinY, touch_dev->cap_info.MaxY, 0, 0);
	}
	else
	{
		input_set_abs_params(touch_dev->input_dev, ABS_MT_POSITION_X, touch_dev->cap_info.MinX, touch_dev->cap_info.MaxX, 0, 0);		
		input_set_abs_params(touch_dev->input_dev, ABS_MT_POSITION_Y, touch_dev->cap_info.MinY, touch_dev->cap_info.MaxY, 0, 0);
	}

        input_set_abs_params(touch_dev->input_dev, ABS_TOOL_WIDTH, 0, 255, 0, 0);				
	input_set_abs_params(touch_dev->input_dev, ABS_MT_TOUCH_MAJOR, 0, 255, 0, 0);
	input_set_abs_params(touch_dev->input_dev, ABS_MT_WIDTH_MAJOR, 0, 255, 0, 0);

	sain_debug_msg("register %s input device \r\n", touch_dev->input_dev->name);
	ret = input_register_device(touch_dev->input_dev);
	if(ret) {
		printk(KERN_ERR "unable to register %s input device\r\n", touch_dev->input_dev->name);
        	goto err_input_register_device;
	}


	/* configure touchscreen interrupt gpio */
	ret = gpio_request(GPIO_TOUCH_PIN_NUM, "sain_irq_gpio");
	if (ret) {
				printk(KERN_ERR "unable to request gpio.(%s)\r\n", touch_dev->input_dev->name);
				goto err_request_irq;
	}

	ret = gpio_direction_input(GPIO_TOUCH_PIN_NUM);

	touch_dev->int_gpio_num = GPIO_TOUCH_PIN_NUM;

	sain_debug_msg("request irq (pin = %d) \r\n", touch_dev->int_gpio_num);

	if(touch_dev->client->irq != GPIO_TOUCH_IRQ)
	{
		printk(KERN_ERR "why irq doesn't match\n");
		touch_dev->client->irq = GPIO_TOUCH_IRQ;		
	}
	
	if (touch_dev->client->irq) {
		ret = request_irq(touch_dev->client->irq, ts_int_handler, IRQF_TRIGGER_LOW|IRQF_DISABLED, SAIN_DRIVER_NAME, touch_dev);
		if (ret) {
			printk(KERN_ERR "unable to register irq.(%s)\r\n", touch_dev->input_dev->name);
			goto err_request_irq;
		}	
	}
	touch_dev->irq = touch_dev->client->irq;
	//enable_irq(touch_dev->client->irq);	// need debug 
	dev_info(&client->dev, "sain touch probe.\r\n");

#ifdef CONFIG_HAS_EARLYSUSPEND
	touch_dev->early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN + 1;
	touch_dev->early_suspend.suspend = sain_early_suspend;
	touch_dev->early_suspend.resume = sain_late_resume;
	register_early_suspend(&touch_dev->early_suspend);
#endif

#if USE_THREAD_METHOD
	wake_up_process( touch_dev->task );
#endif

#if USE_UPDATE_SYSFS
 	ret = sysfs_create_group(&client->dev.kobj, &sain_attr_group);
	if (ret)
	{
		printk(KERN_ERR "cannot create sysfs\n");
		goto err_create_sysfs;
	}
#endif
	return 0;

err_request_irq:
	input_unregister_device(touch_dev->input_dev);
err_input_register_device:
	input_free_device(touch_dev->input_dev);
err_kthread_create_failed:	
err_input_allocate_device:	
	kfree(touch_dev);
err_alloc_dev_data:	
err_check_functionality:
err_create_sysfs:

	return ret;
}


static int sain_touch_remove(struct i2c_client *client)
{
	sain_touch_dev *touch_dev = i2c_get_clientdata(client);

	sain_debug_msg("sain_touch_remove+ \r\n");	
#ifdef CONFIG_HAS_EARLYSUSPEND
	unregister_early_suspend(&touch_dev->early_suspend);
#endif
	if (touch_dev->client->irq) {
		free_irq(touch_dev->client->irq, touch_dev);
	}

	gpio_free(TS_RESET_OUT);
	
	input_unregister_device(touch_dev->input_dev);
	input_free_device(touch_dev->input_dev);
	kfree(touch_dev);

	return 0;
}

static int __devinit sain_touch_init(void)
{
	return i2c_add_driver(&sain_touch_driver);    
}

static void __exit sain_touch_exit(void)
{
	i2c_del_driver(&sain_touch_driver);
}

module_init(sain_touch_init);
module_exit(sain_touch_exit);

MODULE_DESCRIPTION("touch-screen device driver using i2c interface");
MODULE_AUTHOR("sohnet <swjang@sain.co.kr>");
MODULE_LICENSE("GPL");
