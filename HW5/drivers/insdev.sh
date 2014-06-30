insmod fnd_driver.ko
insmod led_driver.ko
insmod fpga_fnd_driver.ko
insmod fpga_text_lcd_driver.ko
insmod fpga_led_driver.ko
insmod fpga_dot_driver.ko
insmod fpga_push_switch_driver.ko

mknod /dev/led_driver c 240 0
mknod /dev/fpga_led c 260 0
mknod /dev/fpga_dot c 262 0
mknod /dev/fnd_driver c 241 0
mknod /dev/fpga_fnd c 261 0
mknod /dev/fpga_text_lcd c 263 0
mknod /dev/fpga_push_switch c 265 0

chmod 666 /dev/alarm
