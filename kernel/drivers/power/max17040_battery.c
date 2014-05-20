/*
 *  max17040_battery.c
 *  fuel-gauge systems for lithium-ion (Li+) batteries
 *
 *  Copyright (C) 2009 Samsung Electronics
 *  Minkyu Kang <mk7.kang@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/mutex.h>
#include <linux/err.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/power_supply.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/max17040_battery.h>
#include <linux/slab.h>

#include <mach/gpio.h>
#include <mach/regs-gpio.h>
#include <plat/gpio-cfg.h>

#define MAX17040_VCELL_MSB		0x02
#define MAX17040_VCELL_LSB		0x03
#define MAX17040_SOC_MSB		0x04
#define MAX17040_SOC_LSB		0x05
#define MAX17040_MODE_MSB		0x06
#define MAX17040_MODE_LSB		0x07
#define MAX17040_VER_MSB		0x08
#define MAX17040_VER_LSB		0x09
#define MAX17040_RCOMP_MSB		0x0C
#define MAX17040_RCOMP_LSB		0x0D
#define MAX17040_CMD_MSB		0xFE
#define MAX17040_CMD_LSB		0xFF
#define MAX17040_DELAY			msecs_to_jiffies(1000)
#define MAX17040_BATTERY_FULL	99

#define	POWER_OFF_VOLTAGE	3300000
#define	CHARGE_OFF_VOLTAGE	4100000

struct max17040_chip {
	struct i2c_client		*client;
	struct delayed_work		work;
	struct power_supply		battery;
	struct power_supply		ac;
	struct power_supply		usb;
	struct max17040_platform_data	*pdata;

	/* State Of Connect */
	int online;
	/* battery voltage */
	int vcell;
	/* battery capacity */
	int soc;
	/* State Of Charge */
	int status;
	int usb_online;
};

static int max17040_get_property(struct power_supply *psy,
			    enum power_supply_property psp,
			    union power_supply_propval *val)
{
	struct max17040_chip *chip = container_of(psy,
				struct max17040_chip, battery);

	switch (psp) {
	case POWER_SUPPLY_PROP_STATUS:
		val->intval = chip->status;
		break;
	case POWER_SUPPLY_PROP_ONLINE:
		val->intval = chip->online;
		break;
	case POWER_SUPPLY_PROP_VOLTAGE_NOW:
		val->intval = chip->vcell;
		break;
	case POWER_SUPPLY_PROP_PRESENT:
		val->intval = chip->pdata->battery_online();
		break;
	case POWER_SUPPLY_PROP_CAPACITY:
		val->intval = chip->soc;
		break;
	 case POWER_SUPPLY_PROP_TECHNOLOGY:
		val->intval = POWER_SUPPLY_TECHNOLOGY_LION;
		break;
	 case POWER_SUPPLY_PROP_HEALTH:
	 	if(chip->vcell  < 2850000)
			val->intval = POWER_SUPPLY_HEALTH_UNSPEC_FAILURE;
		else
			val->intval = POWER_SUPPLY_HEALTH_GOOD;
		break;
	case POWER_SUPPLY_PROP_TEMP:
		val->intval = 365;
		break;
	default:
		return -EINVAL;
	}
	return 0;
}

static int usb_get_property(struct power_supply *psy,
			    enum power_supply_property psp,
			    union power_supply_propval *val)
{
	int ret = 0;
	struct max17040_chip *chip = container_of(psy,
				struct max17040_chip, usb);

	switch (psp) {
	case POWER_SUPPLY_PROP_PRESENT:
	case POWER_SUPPLY_PROP_ONLINE:
		if(chip->pdata->charger_usb_online)
			val->intval =  chip->pdata->charger_usb_online();
		break;
	default:
		ret = -EINVAL;
		break;
	}
	return ret;
}

static int adapter_get_property(struct power_supply *psy,
			enum power_supply_property psp,
			union power_supply_propval *val)
{
	struct max17040_chip *chip = container_of(psy,
				struct max17040_chip, ac);
	int ret = 0;

	switch (psp) {
	case POWER_SUPPLY_PROP_PRESENT:
	case POWER_SUPPLY_PROP_ONLINE:
		if (chip->pdata->charger_ac_online)
			val->intval = chip->pdata->charger_ac_online();
		break;
	default:
		ret = -EINVAL;
		break;
	}
	return ret;
}
static int max17040_write_reg(struct i2c_client *client, int reg, u8 value)
{
	int ret;

	ret = i2c_smbus_write_byte_data(client, reg, value);

	if (ret < 0)
		dev_err(&client->dev, "%s: err %d\n", __func__, ret);

	return ret;
}

static int max17040_read_reg(struct i2c_client *client, int reg)
{
	int ret;

	ret = i2c_smbus_read_byte_data(client, reg);

	if (ret < 0)
		dev_err(&client->dev, "%s: err %d\n", __func__, ret);

	return ret;
}

static void max17040_reset(struct i2c_client *client)
{
	max17040_write_reg(client, MAX17040_CMD_MSB, 0x54);
	max17040_write_reg(client, MAX17040_CMD_LSB, 0x00);
}

static void max17040_get_vcell(struct i2c_client *client)
{
	struct max17040_chip *chip = i2c_get_clientdata(client);
	u8 msb;
	u8 lsb;

	msb = max17040_read_reg(client, MAX17040_VCELL_MSB);
	lsb = max17040_read_reg(client, MAX17040_VCELL_LSB);

	chip->vcell = (msb << 4) + (lsb >> 4);
	chip->vcell = (chip->vcell * 125) * 10;
	
	#if defined(MAX17040_DEBUG)
	if(chip->soc)
		printk(KERN_INFO "max17040: battery status = %d mv %d%%\n", chip->vcell, chip->soc);
	#endif
}

static void max17040_get_soc(struct i2c_client *client)
{
	struct max17040_chip *chip = i2c_get_clientdata(client);
	u8 msb;
	u8 lsb;

	msb = max17040_read_reg(client, MAX17040_SOC_MSB);
	lsb = max17040_read_reg(client, MAX17040_SOC_LSB);

	chip->soc = (msb < 100) ? msb : 100;
		
	if(chip->soc)	{
		if(chip->vcell < POWER_OFF_VOLTAGE)
			chip->soc = 0;
	} else {
		if(chip->vcell >= POWER_OFF_VOLTAGE)	{
			#if defined(MAX17040_DEBUG)
			printk(KERN_INFO "max17040: battery status = %d mv %d%%\n", chip->vcell, chip->soc);
			#endif
			chip->soc = 1;
		}
	}

	return;
}

static void max17040_get_version(struct i2c_client *client)
{
	u8 msb;
	u8 lsb;

	msb = max17040_read_reg(client, MAX17040_VER_MSB);
	lsb = max17040_read_reg(client, MAX17040_VER_LSB);

	dev_info(&client->dev, "MAX17040 Fuel-Gauge Ver %d%d\n", msb, lsb);
}

static void max17040_get_online(struct i2c_client *client)
{
	struct max17040_chip *chip = i2c_get_clientdata(client);
	int usb = 0, ac = 0;

	if (chip->pdata->charger_ac_online) {
		ac = chip->pdata->charger_ac_online();
	}
	if (chip->pdata->charger_usb_online) {
		usb = chip->pdata->charger_usb_online();
		chip->usb_online = usb;
	}

	if (usb == 0 && ac == 0) {
		#if defined(MAX17040_DEBUG)
		printk(KERN_INFO "max17040: power supply is off-line\n");
		#endif
		chip->online = 0;
	} else {
		#if defined(MAX17040_DEBUG)
		printk(KERN_INFO "max17040: power supply is on-line\n");
		#endif
		chip->online = 1;
	}
}

static void max17040_get_status(struct i2c_client *client)
{
	struct max17040_chip *chip = i2c_get_clientdata(client);
	int ret;

	if (!chip->pdata->charger_ac_online || !chip->pdata->charger_usb_online || !chip->pdata->charger_enable) {
		chip->status = POWER_SUPPLY_STATUS_UNKNOWN;
		return;
	}

	if(chip->online) {
		if(chip->pdata->charger_done()) {
			#if defined(MAX17040_DEBUG)
			printk(KERN_INFO "max17040: max8903 indicates charging is complete\n");
			#endif
			chip->pdata->charger_disable();
			if( (chip->soc >= MAX17040_BATTERY_FULL) && (chip->vcell >= CHARGE_OFF_VOLTAGE) ) {
				chip->status = POWER_SUPPLY_STATUS_FULL;
				#if defined(MAX17040_DEBUG)
				printk(KERN_INFO "max17040: battery status = %d mv %d%% \n", chip->vcell, chip->soc);
				#endif
			} else {
				#if defined(MAX17040_DEBUG)
				printk(KERN_INFO "max17040: battery status = %d mv %d%% \n", chip->vcell, chip->soc);
				printk(KERN_INFO "max17040: battery isn't full. re-enabling charger\n");
				#endif
				ret = chip->pdata->charger_enable();
				if(ret != 0) {
					chip->status = POWER_SUPPLY_STATUS_CHARGING;
					#if defined(MAX17040_DEBUG)
					printk(KERN_INFO "max17040: charging re-enabled\n");
					#endif
				} else {
					chip->status = POWER_SUPPLY_STATUS_NOT_CHARGING;
					#if defined(MAX17040_DEBUG)
					printk(KERN_INFO "max17040: fail to re-enable charging\n");
					#endif
				}
			}
		} else {
			chip->status = POWER_SUPPLY_STATUS_CHARGING;
			if( (chip->soc >= MAX17040_BATTERY_FULL) && (chip->vcell >= CHARGE_OFF_VOLTAGE) ) {
				chip->status = POWER_SUPPLY_STATUS_FULL;
				chip->pdata->charger_disable();
			}
		}
	} else {
		chip->pdata->charger_disable();
		chip->status = POWER_SUPPLY_STATUS_DISCHARGING;
	}
}

static void max17040_work(struct work_struct *work)
{
	struct max17040_chip *chip;
	int old_usb_online, old_online, old_vcell, old_soc;

	chip = container_of(work, struct max17040_chip, work.work);

	old_online = chip->online;
	old_usb_online = chip->usb_online;
	old_vcell = chip->vcell;
	old_soc = chip->soc;
	max17040_get_online(chip->client);
	max17040_get_vcell(chip->client);
	max17040_get_soc(chip->client);
	max17040_get_status(chip->client);

	if((old_vcell != chip->vcell) || (old_soc != chip->soc))
		power_supply_changed(&chip->battery);
	if(old_usb_online != chip->usb_online)
		power_supply_changed(&chip->usb);
	if(old_online != chip->online)
		power_supply_changed(&chip->ac);
	schedule_delayed_work(&chip->work, MAX17040_DELAY);
}


static enum power_supply_property max17040_battery_props[] = {
	POWER_SUPPLY_PROP_PRESENT,
	POWER_SUPPLY_PROP_STATUS,
	POWER_SUPPLY_PROP_VOLTAGE_NOW,
	POWER_SUPPLY_PROP_CAPACITY,
	POWER_SUPPLY_PROP_TECHNOLOGY,
	POWER_SUPPLY_PROP_HEALTH,
	POWER_SUPPLY_PROP_TEMP,
};

static enum power_supply_property adapter_get_props[] = {
	POWER_SUPPLY_PROP_PRESENT,
	POWER_SUPPLY_PROP_ONLINE,
};

static enum power_supply_property usb_get_props[] = {
	POWER_SUPPLY_PROP_PRESENT,
	POWER_SUPPLY_PROP_ONLINE,
};

static struct {
	int gpio;
	char* name;
	bool output;
	int value;
} gpios[] = {
	{ GPIO_CHARGER_AC_ONLINE,	"charger ac online", 0, 0 },
	{ GPIO_CHARGER_USB_ONLINE,	"charger usb online", 0, 0 },
	{ GPIO_CHARGER_STATUS,	"is charging", 0, 0 },
	{ GPIO_CHARGER_ENABLE,	"charger enable", 1, 0 },
	{ GPIO_CHARGER_LED, "charging indicator", 1, 0 },
};


static int __devinit max17040_probe(struct i2c_client *client,
			const struct i2c_device_id *id)
{
	struct i2c_adapter *adapter = to_i2c_adapter(client->dev.parent);
	struct max17040_chip *chip;
	int ret, i;

	if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_BYTE))
		return -EIO;

	chip = kzalloc(sizeof(*chip), GFP_KERNEL);
	if (!chip)
		return -ENOMEM;

	chip->client = client;
	chip->pdata = client->dev.platform_data;

	i2c_set_clientdata(client, chip);

	chip->battery.name		= "battery";
	chip->battery.type		= POWER_SUPPLY_TYPE_BATTERY;
	chip->battery.get_property	= max17040_get_property;
	chip->battery.properties	= max17040_battery_props;
	chip->battery.num_properties	= ARRAY_SIZE(max17040_battery_props);
	chip->battery.external_power_changed = NULL;

	chip->ac.name		= "ac";
	chip->ac.type		= POWER_SUPPLY_TYPE_MAINS;
	chip->ac.get_property	= adapter_get_property;
	chip->ac.properties	= adapter_get_props;
	chip->ac.num_properties	= ARRAY_SIZE(adapter_get_props);
	chip->ac.external_power_changed = NULL;

	chip->usb.name		= "usb";
	chip->usb.type		= POWER_SUPPLY_TYPE_USB;
	chip->usb.get_property	= usb_get_property;
	chip->usb.properties	= usb_get_props;
	chip->usb.num_properties	= ARRAY_SIZE(usb_get_props);
	chip->usb.external_power_changed = NULL;

	ret = power_supply_register(&client->dev, &chip->battery);
	if (ret)
		goto err_battery_failed;

	ret = power_supply_register(&client->dev, &chip->ac);
	if(ret)
		goto err_ac_failed;

	ret = power_supply_register(&client->dev, &chip->usb);
	if(ret)
		goto err_usb_failed;

	max17040_reset(client);
	max17040_get_version(client);

	INIT_DELAYED_WORK_DEFERRABLE(&chip->work, max17040_work);
	schedule_delayed_work(&chip->work, MAX17040_DELAY);

	for (i = 0; i < ARRAY_SIZE(gpios); i++) {
		ret = gpio_request(gpios[i].gpio, gpios[i].name);
		if (ret) {
			printk("%s gpio reqest err: %d\n", gpios[i].name, ret);
			i--;
			goto err_gpio_failed;
		}
		if (gpios[i].output) {
			ret = gpio_direction_output(gpios[i].gpio, 0);
			s3c_gpio_setpull(gpios[i].gpio, S3C_GPIO_PULL_NONE);
		} else {
			ret = gpio_direction_input(gpios[i].gpio);
			s3c_gpio_setpull(gpios[i].gpio, S3C_GPIO_PULL_UP);
		}
		if (ret)
			goto err_gpio_failed;
	}
	return 0;

err_gpio_failed:
	for (; i >= 0; i--)
		gpio_free(gpios[i].gpio);
err_usb_failed:
	power_supply_unregister(&chip->ac);
err_ac_failed:
	power_supply_unregister(&chip->battery);
err_battery_failed:
	dev_err(&client->dev, "failed: power supply register\n");
	i2c_set_clientdata(client, NULL);
	kfree(chip);

	return ret;
}

static int __devexit max17040_remove(struct i2c_client *client)
{
	struct max17040_chip *chip = i2c_get_clientdata(client);
	int i;

	for (i = ARRAY_SIZE(gpios) - 1; i >= 0; i--)
		gpio_free(gpios[i].gpio);
	power_supply_unregister(&chip->usb);
	power_supply_unregister(&chip->ac);
	power_supply_unregister(&chip->battery);
	cancel_delayed_work(&chip->work);
	i2c_set_clientdata(client, NULL);
	kfree(chip);
	return 0;
}

#ifdef CONFIG_PM

static int max17040_suspend(struct i2c_client *client,
		pm_message_t state)
{
	struct max17040_chip *chip = i2c_get_clientdata(client);

	cancel_delayed_work(&chip->work);
	return 0;
}

static int max17040_resume(struct i2c_client *client)
{
	struct max17040_chip *chip = i2c_get_clientdata(client);

	schedule_delayed_work(&chip->work, MAX17040_DELAY);
	return 0;
}

#else

#define max17040_suspend NULL
#define max17040_resume NULL

#endif /* CONFIG_PM */

static const struct i2c_device_id max17040_id[] = {
	{ "max17040", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, max17040_id);

static struct i2c_driver max17040_i2c_driver = {
	.driver	= {
		.name	= "max17040",
	},
	.probe		= max17040_probe,
	.remove		= __devexit_p(max17040_remove),
	.suspend	= max17040_suspend,
	.resume		= max17040_resume,
	.id_table	= max17040_id,
};

static int __init max17040_init(void)
{
	return i2c_add_driver(&max17040_i2c_driver);
}
module_init(max17040_init);

static void __exit max17040_exit(void)
{
	i2c_del_driver(&max17040_i2c_driver);
}
module_exit(max17040_exit);

MODULE_AUTHOR("Minkyu Kang <mk7.kang@samsung.com>");
MODULE_DESCRIPTION("MAX17040 Fuel Gauge");
MODULE_LICENSE("GPL");
