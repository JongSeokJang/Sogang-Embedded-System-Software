//[*]--------------------------------------------------------------------------------------------------[*]
/*
 *	
 * ACHRO4210 Dev Board key-pad header file (charles.park) 
 *
 */
//[*]--------------------------------------------------------------------------------------------------[*]

#ifndef	_ACHRO4210_KEYPAD_H_
#define	_ACHRO4210_KEYPAD_H_

	#define TRUE 				1
	#define FALSE 				0
	
	#define	ON					1
	#define	OFF					0
	
	#define	KEY_PRESS			1
	#define	KEY_RELEASE			0
	
	#define	KEYPAD_STATE_BOOT	0
	#define	KEYPAD_STATE_RESUME	1
	
	// keypad sampling rate
	#define	PERIOD_10MS			(HZ/100)	// 10ms
	#define	PERIOD_20MS			(HZ/50)		// 20ms
	#define	PERIOD_50MS			(HZ/20)		// 50ms

	// power off timer	
	#define	PERIOD_1SEC			(1*HZ)		// 1sec
	#define	PERIOD_3SEC			(3*HZ)		// 3sec
	#define	PERIOD_5SEC			(5*HZ)		// 5sec

	// Keypad wake up delay
	#define	KEYPAD_WAKEUP_DELAY		100		// 1 sec delay
	
	//
    	// Power Off Enable
    	//
	#define PSHOLD_CONTROL          (*(unsigned long *)S5P_PS_HOLD_CONTROL)
	#define POWER_OFF_ENABLE()      {   PSHOLD_CONTROL = 0; }
	
	//
	// Power LED Port
	//
	#define	POWER_LED_PORT		(S5PV310_GPC1(2))
	#define	POWER_LED_STR		"POWER LED"

	typedef	struct	achro4210_keypad__t	{
		
		// keypad control
		struct input_dev	*driver;			// input driver
		struct timer_list 	rd_timer;			// keyscan timer

		// power off control
		struct timer_list	poweroff_timer;		// long power key process
		unsigned char		poweroff_flag;		// power key press flag

		// sysfs used		
		unsigned char		sampling_rate;		// 10 msec sampling
		unsigned char		poweroff_time;		// reset off
		
		unsigned int		wakeup_delay;		// key wakeup delay

	}	achro4210_keypad_t;
	
	extern	achro4210_keypad_t	achro4210_keypad;
#endif		/* _ACHRO4210_KEYPAD_H_*/
