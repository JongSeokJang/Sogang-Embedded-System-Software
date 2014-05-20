//[*]--------------------------------------------------------------------------------------------------[*]
/*
 *	
 * ACHRO4210 Dev Board key-pad header file (charles.park) 
 *
 */
//[*]--------------------------------------------------------------------------------------------------[*]
#ifndef	_ACHRO4210_KEYCODE_H_

#define	_ACHRO4210_KEYCODE_H_
	#if defined(CONFIG_KEYPAD_ACHRO4210)
		 #define	MAX_KEYCODE_CNT		7
	
		int ACHRO4210_Keycode[MAX_KEYCODE_CNT] = {
			KEY_MENU,		// SW_2
			KEY_BACK,		// SW_4
			KEY_HOME,		// SW_3
			KEY_VOLUMEDOWN,	// SW_8 DOWN
			KEY_VOLUMEUP,	// SW_8 UP
			KEY_POWER,		// SW_8 CLICK
			KEY_SEARCH		// SW_6
		};
	
		#if	defined(DEBUG_MSG)
			const char ACHRO4210_KeyMapStr[MAX_KEYCODE_CNT][20] = {
				"KEY_MENU\n",		// SW_2
				"KEY_BACK\n",		// SW_4
				"KEY_HOME\n",		// SW_3
				"KEY_VOLUMEDOWN\n",	// SW_8 DOWN
				"KEY_VOLUMEUP\n",	// SW_8 UP
				"KEY_POWER\n",		// SW_8 CLICK
				"KEY_SEARCH\n"		// SW_6
			};
		#endif	// DEBUG_MSG
	#endif	// defined(CONFIG_KEYPAD_ACHRO4210)

#endif		/* _ACHRO4210_KEYPAD_H_*/
