#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>

unsigned char dot_number[10][10] = {
		{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // 0
		{0x7F,0x7F,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60}, // 1
		{0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x7F,0x7F}, // 2
		{0x7F,0x7F,0x60,0x60,0x7F,0x7F,0x60,0x60,0x60,0x60}, // 3
		{0x03,0x03,0x03,0x03,0x7F,0x7F,0x03,0x03,0x7F,0x7F}, // 4
		{0x7F,0x7F,0x60,0x60,0x7F,0x7F,0x60,0x60,0x7F,0x7F}, // 5
		{0x7F,0x7F,0x03,0x03,0x7F,0x7F,0x03,0x03,0x7F,0x7F}, // 6
		{0x7F,0x7F,0x63,0x63,0x7F,0x7F,0x60,0x60,0x7F,0x7F}, // 7
		{0x7F,0x7F,0x03,0x03,0x7F,0x7F,0x63,0x63,0x7F,0x7F}, // 8
		{0x3e,0x7f,0x63,0x63,0x7f,0x3f,0x03,0x03,0x03,0x03} // 9
};

void Java_com_example_androidex_FigureActivity_FigureSwitch (JNIEnv *env, jobject thiz, jstring option, jstring left){
	int fnd_dev, fndposition, fndvalue;
	int fpga_led, gpio_led, fpga_dot, fpga_fnd;
	int i, dot_size, dot_num, num;
	unsigned char fpga_led_dat, led_dat, num_dat[4];

	// Open gpio fnd driver
	if((fnd_dev = open("/dev/fnd_driver", O_WRONLY)) < 0)
		perror("/dev/fnd_driver open error");

	// Open fpga led driver
	if((fpga_led = open("/dev/fpga_led", O_WRONLY)) < 0)
		perror("/dev/fpga_led open error");

	// Open gpio led driver
	if((gpio_led = open("/dev/led_driver", O_WRONLY)) < 0)
		perror("/dev/led_driver open error");

	// Open fpga dot driver
	if((fpga_dot = open("/dev/fpga_dot", O_WRONLY)) < 0)
		perror("/dev/fpga_dot open error");
	dot_size = sizeof(dot_number[10]);

	// Open fpga fnd driver
	if((fpga_fnd = open("/dev/fpga_fnd", O_WRONLY)) < 0)
		perror("/dev/fpga_led open error");

	// Conver jstring to c string
	const char *str = (*env)->GetStringUTFChars(env, option, 0);
	const char *str2 = (*env)->GetStringUTFChars(env, left, 0);

	num = atoi(str2);
	sprintf(num_dat, "%04d", num);

	// Find where the value is
	for(i=0;i<4;i++)
		if(str[i] != '0')
			break;

	// Copy position of the char
	switch(i){
		case 0:
			fndposition = 0x02;
			led_dat = 0xE0;
			break;
		case 1:
			fndposition = 0x04;
			led_dat = 0xD0;
			break;
		case 2:
			fndposition = 0x10;
			led_dat = 0xB0;
			break;
		case 3:
			fndposition = 0x80;
			led_dat = 0x70;
			break;
		default:
			fndposition = 0x00;
			led_dat = 0xFF;
			break;
	}

	// Copy value of the char
	switch(str[i]){
		case '1':
			fndvalue = 0x73;
			fpga_led_dat = 128;
			dot_num = 1;
			break;
		case '2':
			fndvalue = 0x8F;
			fpga_led_dat = 64;
			dot_num = 2;
			break;
		case '3':
			fndvalue = 0x71;
			fpga_led_dat = 32;
			dot_num = 3;
			break;
		case '4':
			fndvalue = 0x8D;
			fpga_led_dat = 16;
			dot_num = 4;
			break;
		case '5':
			fndvalue = 0x61;
			fpga_led_dat = 8;
			dot_num = 5;
			break;
		case '6':
			fndvalue = 0x0D;
			fpga_led_dat = 4;
			dot_num = 6;
			break;
		case '7':
			fndvalue = 0x21;
			fpga_led_dat = 2;
			dot_num = 7;
			break;
		case '8':
			fndvalue = 0x05;
			fpga_led_dat = 1;
			dot_num = 8;
			break;
		default :
			fndvalue = 0x00;
			fpga_led_dat = 0;
			dot_num = 0;
			break;
	}

	// Combine position and value
	unsigned short temp;
	temp = fndposition;
	temp = (temp<<8)|fndvalue;

	// Write on devices
	write(fnd_dev, &temp, sizeof(short));
	write(fpga_led, &fpga_led_dat, 1);
	write(gpio_led, &led_dat, 1);
	write(fpga_dot, dot_number[dot_num], dot_size);
	write(fpga_fnd, &num_dat, 4);

	// Close devices
	close(fnd_dev);
	close(fpga_led);
	close(gpio_led);
	close(fpga_dot);
	close(fpga_fnd);
}       

void Java_com_example_androidex_FigureActivity_TextPrint (JNIEnv *env, jobject thiz, jstring id, jstring name){
	unsigned char text[32];
	int i, fpga_text;

	// Open fpga text lcd driver
	if((fpga_text = open("/dev/fpga_text_lcd", O_WRONLY)) < 0)
		perror("/dev/fpga_text_lcd open error");

	// Conver jstring to c string
	const char *student_id = (*env)->GetStringUTFChars(env, id, 0);
	const char *student_name = (*env)->GetStringUTFChars(env, name, 0);

	if(student_id[0] == 'N'){
		for(i=0;i<32;i++)
			text[i] = '\0';
	} else{
		for(i=0;i<16;i++){
			text[i] = student_id[i];
			text[16+i] = student_name[i];
		}
	}

	write(fpga_text, text, 32);

	close(fpga_text);
}
