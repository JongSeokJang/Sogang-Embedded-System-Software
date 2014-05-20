/*
 *  Copyright (C) 2009 Samsung Electronics
 *  Minkyu Kang <mk7.kang@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __MAX17040_BATTERY_H_
#define __MAX17040_BATTERY_H_

#define GPIO_CHARGER_STATUS		S5PV310_GPX2(6)
#define GPIO_CHARGER_ENABLE		S5PV310_GPC0(1)
#define	GPIO_CHARGER_AC_ONLINE	S5PV310_GPX3(3)
#define	GPIO_CHARGER_USB_ONLINE	S5PV310_GPX3(4)
#define GPIO_CHARGER_LED		S5PV310_GPE4(1)
struct max17040_platform_data {
	int (*battery_online)(void);
	int (*charger_ac_online)(void);
	int (*charger_usb_online)(void);
	int (*charger_enable)(void);
	int (*charger_done)(void);
	void (*charger_disable)(void);
};

#endif
