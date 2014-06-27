#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>

unsigned char count_number[11][10] = {
	{0x3e,0x7f,0x63,0x73,0x73,0x6f,0x67,0x63,0x7f,0x3e}, // 0
	{0x0c,0x1c,0x1c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x1e}, // 1
	{0x7e,0x7f,0x03,0x03,0x3f,0x7e,0x60,0x60,0x7f,0x7f}, // 2
	{0xfe,0x7f,0x03,0x03,0x7f,0x7f,0x03,0x03,0x7f,0x7e}, // 3
	{0x66,0x66,0x66,0x66,0x66,0x66,0x7f,0x7f,0x06,0x06}, // 4
	{0x7f,0x7f,0x60,0x60,0x7e,0x7f,0x03,0x03,0x7f,0x7e}, // 5
	{0x60,0x60,0x60,0x60,0x7e,0x7f,0x63,0x63,0x7f,0x3e}, // 6
	{0x7f,0x7f,0x63,0x63,0x03,0x03,0x03,0x03,0x03,0x03}, // 7
	{0x3e,0x7f,0x63,0x63,0x7f,0x7f,0x63,0x63,0x7f,0x3e}, // 8
	{0x3e,0x7f,0x63,0x63,0x7f,0x3f,0x03,0x03,0x03,0x03}, // 9
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}	// empty
};

void Java_com_example_androidex_PuzzleActivity_PuzzleCount (JNIEnv *env, jobject obj, jstring time_left){
	int fpga_dot, dot_size, dot_num;

	// Open fpga dot driver
	if((fpga_dot = open("/dev/fpga_dot", O_WRONLY)) < 0)
		perror("/dev/fpga_dot open error");
	dot_size = sizeof(count_number[11]);

	// Conver jstring to c string
	const char *str = (*env)->GetStringUTFChars(env, time_left, 0);

	switch(str[0]){
		case '0':
			dot_num = 0;
			break;
		case '1':
			dot_num = 1;
			break;
		case '2':
			dot_num = 2;
			break;
		case '3':
			dot_num = 3;
			break;
		case '4':
			dot_num = 4;
			break;
		case '5':
			dot_num = 5;
			break;
		case '6':
			dot_num = 6;
			break;
		case '7':
			dot_num = 7;
			break;
		case '8':
			dot_num = 8;
			break;
		case '9':
			dot_num = 9;
			break;
		default:
			dot_num = 10;
			break;
	}

	write(fpga_dot, count_number[dot_num], dot_size);

	close(fpga_dot);
}

void Java_com_example_androidex_PuzzleActivity_PuzzleScoring (JNIEnv *env, jobject obj, jstring score){
	int fpga_fnd, i;
	char data[4];

	// Open fpga fnd driver
	if((fpga_fnd = open("/dev/fpga_fnd", O_WRONLY)) < 0)
			perror("/dev/fpga_fnd open error");

	// Convert jstring to c string
	const char *str = (*env)->GetStringUTFChars(env, score, 0);

	// Convert string to char array
	for(i=0;i<4;i++)
		data[i] = str[i];

	write(fpga_fnd, &data, 4);	// fpga fnd

	close(fpga_fnd);
}
