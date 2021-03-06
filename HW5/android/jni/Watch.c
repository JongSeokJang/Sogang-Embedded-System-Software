#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>

void JNICALL Java_com_example_androidex_WatchActivity_Watch (JNIEnv *env, jobject thiz, jstring jdate, jstring jtime){
	unsigned char text[32];
	int text_dev, i;

	// Open fpga text lcd driver
	if((text_dev = open("/dev/fpga_text_lcd", O_WRONLY)) < 0)
		perror("/dev/fpga_text_lcd open error");

	// Convert jstring to c string
	const char *date = (*env)->GetStringUTFChars(env, jdate, 0);
	const char *time = (*env)->GetStringUTFChars(env, jtime, 0);

	if(date[0] == 'N'){
		for(i=0;i<32;i++)
			text[i] = '\0';
	} else{
		for(i=0;i<16;i++){
			text[i] = date[i];
			text[16+i] = time[i];
		}
	}

	write(text_dev, text, 32);

	close(text_dev);
}       

void JNICALL Java_com_example_androidex_WatchActivity_WatchFND (JNIEnv *env, jobject thiz, jstring jtime){
	int fpga_fnd, i;
	unsigned char data[4];

	// Open fpga fnd driver
	if((fpga_fnd = open("/dev/fpga_fnd", O_WRONLY)) < 0)
		perror("/dev/fpga_fnd open error");

	const char *date = (*env)->GetStringUTFChars(env, jtime, 0);
	for(i=0;i<4;i++)
		data[i] = date[i];

	write(fpga_fnd, &data, 4);

	close(fpga_fnd);
}

jstring JNICALL Java_com_example_androidex_WatchActivity_WatchControl (JNIEnv *env, jobject thiz){
	int switch_dev, i;
	unsigned char push_sw[9];
	unsigned char temp[10];

	// Open fpga push switch driver
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

	close(switch_dev);

	return (*env)->NewStringUTF(env, temp);
}
