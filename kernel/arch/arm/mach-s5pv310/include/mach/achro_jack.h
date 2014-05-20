
#ifndef __ACHRO_JACK_H
#define __ACHRO_JACK_H

#define GPIO_HEADSET     S5PV310_GPX3(0)
#define EINT_HEADSET     IRQ_EINT(24)

enum {
	JACK_NO_DEVICE			= 0x0,
	HEADSET_4_POLE_DEVICE	= 0x01 << 0,
	HEADSET_3_POLE_DEVICE	= 0x01 << 1,

	TVOUT_DEVICE			= 0x01 << 2,
	UNKNOWN_DEVICE			= 0x01 << 3,
};

enum {
	JACK_DETACHED		= 0x0,
	JACK_ATTACHED		= 0x1,
};

struct achro_gpio_info 
{
	int	eint;
	int	gpio;
};

struct achro_jack_port
{
	struct achro_gpio_info	det_jack;
};

struct achro_jack_platform_data
{
	struct 	achro_jack_port	*port;
	int		nheadsets;
};

#endif
