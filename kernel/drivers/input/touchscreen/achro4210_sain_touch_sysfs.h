//[*]--------------------------------------------------------------------------------------------------[*]
/*
 *	
 * _ACHRO4210_TOUCH_SYSFS_H_ Header file
 *
 */
//[*]--------------------------------------------------------------------------------------------------[*]

#ifndef	_ACHRO4210_TOUCH_SYSFS_H_
#define	_ACHRO4210_TOUCH_SYSFS_H_

extern	int	achro4210_touch_sysfs_create		(struct platform_device *pdev);
extern	void	achro4210_touch_sysfs_remove		(struct platform_device *pdev);
extern	void	achro4210_touch_fw_upgrade_check	(void);

//[*]--------------------------------------------------------------------------------------------------[*]
#endif
//[*]--------------------------------------------------------------------------------------------------[*]

