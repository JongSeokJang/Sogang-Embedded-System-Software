obj-m := fnd_driver.o fpga_buzzer_driver.o fpga_dip_switch_driver.o fpga_dot_driver.o fpga_fnd_driver.o fpga_led_driver.o fpga_push_switch_driver.o fpga_step_motor_driver.o fpga_text_lcd_driver.o led_driver.o 

KDIR := /home/lee/Desktop/kernel
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean :
	rm -rf *.o
	rm -rf *.ko
	rm -rf *.mod.c
	rm -rf *.order
	rm -rf *.symvers
