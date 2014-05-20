//[*]----------------------------------------------------------------------------------------------[*]
//
//
// 
//  Huins-4210 gpio i2c driver 
// 
//
//[*]----------------------------------------------------------------------------------------------[*]
#ifndef	_EP0700_TOUCH_GPIO_I2C_H_
#define	_EP0700_TOUCH_GPIO_I2C_H_

//[*]----------------------------------------------------------------------------------------------[*]
extern	int			achro4210_touch_write			(unsigned char addr, unsigned char *wdata, unsigned char wsize);
extern	int 			achro4210_touch_read			(unsigned char addr, unsigned char *rdata, unsigned char rsize);
extern	int			achro4210_touch_array_read		(unsigned char addr, unsigned char *rdata, unsigned char rsize);

extern	int achro4210_touch_short_write	(unsigned char addr, unsigned short *wdata, unsigned char wsize);

//extern	int			achro4210_touch_bootmode_write	(unsigned char addr, unsigned char *wdata, unsigned char wsize);
//extern	int 			achro4210_touch_bootmode_read	(unsigned char *rdata, unsigned char rsize);

extern	void			achro4210_touch_port_init		(void);

//[*]----------------------------------------------------------------------------------------------[*]
#endif	
//[*]----------------------------------------------------------------------------------------------[*]
