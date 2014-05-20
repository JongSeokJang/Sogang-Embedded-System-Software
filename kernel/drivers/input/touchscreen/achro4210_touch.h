//[*]--------------------------------------------------------------------------------------------------[*]
/*
 *	
 * Huins_Achro4210_touch Header file
 *
 */
//[*]--------------------------------------------------------------------------------------------------[*]
#ifndef	_ACHRO4210_TOUCH_H_
#define	_ACHRO4210_TOUCH_H_

//[*]--------------------------------------------------------------------------------------------------[*]
#ifdef CONFIG_HAS_EARLYSUSPEND
	#include <linux/earlysuspend.h>
#endif

//[*]--------------------------------------------------------------------------------------------------[*]
#define ACHRO4210_TOUCH_DEVICE_NAME 	"achro4210-touch"

//[*]--------------------------------------------------------------------------------------------------[*]
#define	TOUCH_PRESS			1
#define	TOUCH_RELEASE			0
		
//[*]--------------------------------------------------------------------------------------------------[*]
//  Touch Configuration
//[*]--------------------------------------------------------------------------------------------------[*]
#define	TS_RESET_OUT			(S5PV310_GPC0(3))

// Interrupt Check port(Old Board)
// Touch Interrupt define
//#define	ACHRO4210_TOUCH_IRQ 		gpio_to_irq(S5PV310_GPB(1))
//#define	S5PV310_GPBDAT			(S5P_VA_GPIO + 0x44)
//#define	GET_INT_STATUS()		(((*(unsigned long *)S5PV310_GPBDAT) & 0x02) ? 1 : 0)

// Interrupt Check port(New Board)
// Touch Interrupt define
#define	ACHRO4210_TOUCH_IRQ 		gpio_to_irq(S5PV310_GPX1(0))
#define	S5PV310_GPX1DAT			(S5PV310_VA_GPIO2 + 0x0C24)
#define	GET_INT_STATUS()		(((*(unsigned long *)S5PV310_GPX1DAT) & 0x01) ? 1 : 0)
#define	TS_IRQ				(S5PV310_GPX1(0))

#define	TS_ABS_MIN_X			0
#define	TS_ABS_MIN_Y			0

#define	TS_ABS_MAX_X			480
#define	TS_ABS_MAX_Y			800

#if 0
#define	TS_X_THRESHOLD			150
#define	TS_Y_THRESHOLD			150
	
#define	TS_DATA_CNT				11
		
// touch register
#define	TS_DATA					0x01
#define	TS_MODULE_ID			0x02
#define	TS_SENSITIVITY_CTL		0x03
#define	TS_SLEEP_CTL			0x04
#define	TS_RECALIBRATION		0x05
#define	TS_RESET				0x06
#define	TS_SOFT_RESET			0x44

#define	TS_BOOTMODE_RESET		0xB0

// Firmware Upgrade Mode		
#define	TOUCH_MODE_NORMAL		0
#define	TOUCH_MODE_BOOT			1

#define	MAX_FW_SIZE				(64 * 1024)		// 64 Kbytes
#define	MIN_FW_SIZE				(10 * 1024)		// 10 Kbytes
#endif

//[*]--------------------------------------------------------------------------------------------------[*]

//[*]--------------------------------------------------------------------------------------------------[*]
#define	PERIOD_10MS					(HZ/100)	// 10ms
#define	PERIOD_20MS					(HZ/50)		// 20ms
#define	PERIOD_50MS					(HZ/20)		// 50ms

//[*]--------------------------------------------------------------------------------------------------[*]
#define	TOUCH_STATE_BOOT			0
#define	TOUCH_STATE_RESUME			1

//[*]--------------------------------------------------------------------------------------------------[*]
// Touch hold event
//[*]--------------------------------------------------------------------------------------------------[*]
//#define	SW_TOUCH_HOLD				0x09

//[*]--------------------------------------------------------------------------------------------------[*]
typedef	struct	achro4210_touch__t	{
	struct	input_dev		*driver;

	// seqlock_t
	seqlock_t				lock;
	unsigned int			seq;

	// timer
	struct  timer_list		penup_timer;

	// data store
	unsigned int			status;
	unsigned int			x;
	unsigned int			y;

	unsigned char			keydata;
	unsigned char			enable;
	unsigned char			key_enable;
	unsigned char			*fw;
	unsigned int			fw_size;
	unsigned int			fw_ver;
	unsigned int			fw_rev;
	
	unsigned char			rd[20];

	// sysfs used
	unsigned char			sampling_rate;
	unsigned char			sensitivity;	// touch sensitivity (0-255) : default 0x14

	#ifdef CONFIG_HAS_EARLYSUSPEND
		struct	early_suspend		power;
	#endif

}	achro4210_touch_t;

extern	achro4210_touch_t	achro4210_touch;

//[*]--------------------------------------------------------------------------------------------------[*]
#endif
