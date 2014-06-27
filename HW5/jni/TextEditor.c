#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include "fpga_dot_font.h"
#include "android/log.h"

#define LOG_TAG "MyTag"
#define LOGV(...)   __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

void Java_com_example_androidex_TextActivity_TextEditor (JNIEnv *env, jobject thiz, jstring string){
	int text_dev, fpga_fnd, fpga_dot, fpga_led;
	int i, length, str_size, text_size;
	unsigned char led;
	unsigned char data[4];
	unsigned char text[40];
	unsigned char temp[40];

	// Open fpga text lcd driver
	if((text_dev = open("/dev/fpga_text_lcd", O_WRONLY)) < 0)
		perror("/dev/fpga_text_lcd open error");
	memset(text, 0, sizeof(text));

	// Open fpga fnd driver
	if((fpga_fnd = open("/dev/fpga_fnd", O_WRONLY)) < 0)
		perror("/dev/fpga_fnd open error");

	// Open fpga dot driver
	if((fpga_dot = open("/dev/fpga_dot", O_WRONLY)) < 0)
		perror("/dev/fpga_dot open error");
	str_size = sizeof(fpga_number[18]);

	// Open fpga led driver
	if((fpga_led = open("/dev/fpga_led", O_RDWR)) < 0)
		perror("/dev/fpga_led open error");

	// Conver jstring to c string
	const char *str = (*env)->GetStringUTFChars(env, string, 0);
	length = (*env)->GetStringLength(env, string);

	LOGV("log test %d", length);

	if(length == 0){
		for(i=0;i<32;i++)
			temp[i] = '\0';
	} else {
		text_size = strlen(str);
		if (text_size > 0) {
			strncat(temp, str, text_size);
			memset(temp + text_size, ' ', 32 - text_size);
		}
		for (i = 0; i < text_size; i++)
			temp[i] = str[i];
	}

	// Convert integer to string format
	sprintf(data, "%04d", length);
	led = atoi(data);

	// Get last number of length
	length = length % 10;

	// Print on devices
	write(text_dev, temp, 32);	// fpga text lcd
	write(fpga_fnd, &data, 4);	// fpga fnd
	if(str[0] == '\0')
		write(fpga_dot, fpga_set_blank, str_size);
	else
		write(fpga_dot, fpga_number[length], str_size);
	write(fpga_led, &led, 1);

	// Free memory allocated for the string
	(*env)->ReleaseStringUTFChars(env, string, str);

	// Close device driver
	close(text_dev);
	close(fpga_fnd);
	close(fpga_dot);
	close(fpga_led);
}       

jstring Java_com_example_androidex_TextActivity_PushSwitch (JNIEnv *env, jobject thiz){
	int switch_dev, i;
	unsigned char push_sw[9];
	unsigned char temp[10];
	char *str;

	if((switch_dev = open("/dev/fpga_push_switch", O_RDWR)) < 0)
		perror("/dev/fpga_push_switch open error");

	// Read switch input
	read(switch_dev, &push_sw, 9);

	// Copy to char array
	for(i=0;i<9;i++){
		if(push_sw[i] == 1)
			temp[i] = '1';
		else
			temp[i] = '0';
	}
	temp[9] = '\0';

	// Close fpga switch driver
	close(switch_dev);

	return (*env)->NewStringUTF(env, temp);
}
