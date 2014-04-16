#include <sys/mman.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/input.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include "./fpga_dot_font.h"

#define IO_GPL_BASE_ADDR 0x11000000
#define FND_GPL2CON 0x0100
#define FND_GPL2DAT 0x0104

#define IO_GPE_BASE_ADDR 0x11400000
#define FND_GPE3CON 0x00140
#define FND_GPE3DAT 0x00144

#define IO_BASE_ADDR 0x11400000
#define CON_OFFSET 0x40
#define DAT_OFFSET 0x44

#define BUFF_SIZE 32
#define MAX_BUTTON 9
#define LINE_BUFF 16
#define FPGA_NUMBER 18
#define RUN 8

#define KEY_RELEASE 0
#define KEY_PRESS 1

#define SW1 139
#define SW2 102
#define SW3 158
#define SW4 217
#define SW_UP 115
#define SW_DOWN 114
#define SW_QUIT 116

int mode_shmid, input_shmid, output_shmid;
key_t mode_key, input_key, output_key;
char *mode_shm, *input_shm, *output_shm;

// function to print error
static void die(char *str){
	perror(str);
	*mode_shm = '0';
	exit(1);
}

// initialize sharedmemory to default value
static void init_shared(void){
	int i;

	*input_shm = '*';
	*output_shm = '*';
	
	if(*mode_shm == '2')
		output_shm[0] = 'A';

	if(*mode_shm != '1'){
		output_shm[1] = '0';
		output_shm[2] = '0';
		output_shm[3] = '0';
		output_shm[4] = '0';
		input_shm[9] = '0';
		input_shm[10] = '*';
	}

	for(i=5;i<42;i++)
		output_shm[i] = ' ';
}

static void free_shared(void){
	// dettach the segments from memory space
	shmdt((char *)mode_shm);
	shmdt((char *)input_shm);
	shmdt((char *)output_shm);

	// deallocate segments
	shmctl(mode_shmid, IPC_RMID, (struct shmid_ds *)NULL);
	shmctl(input_shmid, IPC_RMID, (struct shmid_ds *)NULL);
	shmctl(output_shmid, IPC_RMID, (struct shmid_ds *)NULL);
}

// create and initialize shared memory to use
static int shared_memory(void){
	// naming shared memory segments
	mode_key = 1111;
	input_key = 2222;
	output_key = 3333;

	// create the segments
	if((mode_shmid = shmget(mode_key, 1, IPC_CREAT|0600)) < 0)
		die("mode shmget");
	if((input_shmid = shmget(input_key, 32, IPC_CREAT|0660)) < 0)
		die("input shmget");
	if((output_shmid = shmget(output_key, 64, IPC_CREAT|0666)) < 0)
		die("output shmget");

	// attach the segments to memory space
	if((mode_shm = shmat(mode_shmid, NULL, 0)) == (char *)-1)
		die("mode shmat");
	if((input_shm = shmat(input_shmid, NULL, 0)) == (char *)-1)
		die("input shmat");
	if((output_shm = shmat(output_shmid, NULL, 0)) == (char *)-1)
		die("output shmat");

	// initialize data to default
	*mode_shm = '1';
	init_shared();

	return 0;
}

// actual calculation takes place in main process
static int main_process(void){
	printf("DEBUG: main process entered\n");
	while(*mode_shm != '0'){
		if(*mode_shm == '1'){
			cal_stopwatch();
		}

		if(*mode_shm == '2'){
			cal_texteditor();
		}

		if(*mode_shm == '3'){
			cal_custom();
		}
	}

	printf("DEBUG: main process ended\n");
	return 0;
}

// function for stop watch (mode 1) calculation
int cal_stopwatch(void){
	int i, ttime, flag = 1;
	time_t start_time, end_time;
	unsigned long fnd_num[10] = {0x03, 0x9F, 0x25, 0x0D, 0x99, 0x49, 0xC1, 0x1F, 0x01, 0x09};

	printf("DEBUG: stop watch function entered\n");
	while(*mode_shm == '1'){
		if(*input_shm == '*' || *input_shm == '2'){
			ttime = 0;		// time in second
			output_shm[0] = 0x96;	// gpe_dat
			output_shm[1] = 0x03;	// gpl_dat
			output_shm[2] = 0xE0;	// led_dat
		} else{
			while((*input_shm == '3' || *input_shm == '4') && *mode_shm == '1'){
				time(&start_time);
				end_time = 0;

				// run statement approximately one second
				while(difftime(end_time, start_time) <= 0.999){
					for(i=0;i<400;i++){
						output_shm[0] = 0x02;
						output_shm[1] = fnd_num[ttime/60/10];
					}

					for(i=0;i<400;i++){
						output_shm[0] = 0x04;
						output_shm[1] = fnd_num[ttime/60%10] - 0x01;
					}

					for(i=0;i<400;i++){
						output_shm[0] = 0x10;
						output_shm[1] = fnd_num[ttime%60/10];
					}

					for(i=0;i<400;i++){
						output_shm[0] = 0x80;
						output_shm[1] = fnd_num[ttime%60%10];
					}

					// get time for calculating one second
					time(&end_time);
				}

				// managing led driver during mode 1
				if(*input_shm == '4'){
					// stop watch is ticking
					ttime++;
					output_shm[2] = 0x30;
				} else{
					// stop watch is paused
					if(flag){
						output_shm[2] = 0xB0;
						flag = 0;
					} else{
						output_shm[2] = 0x70;
						flag = 1;
					}
				}

				// set time 0, if over 1 hour
				if((ttime/60/10) == 6)
					ttime = 0;
			}
		}
	}

	printf("DEBUG: stop watch function ended\n");
	return 0;
}

// function for text editor (mode 2) calculation
int cal_texteditor(void){
	printf("DEBUG: text editor function entered\n");

	typing_count(0); // initialize counter on FND

	while(*mode_shm == '2'){
		if(input_shm[10] != '*'){
			// change mode to 3, if btn2 & btn3 are pressed
			if(input_shm[1] == 1 && input_shm[2] == 1){
				*mode_shm = '3';
				init_shared();
			}

			// clear lcd screen, if btn4 & btn5 are pressed
			else if(input_shm[3] == 1 && input_shm[4] == 1){
				init_shared();
			}

			// change typing mode, if btn5 & btn6 are pressed
			else if(input_shm[4] == 1 && input_shm[5] == 1){
				typing_mode();
				typing_count(2);
			}

			// terminate program, if btn8 & btn9 are pressed
			else if(input_shm[7] == 1 && input_shm[8] == 1)
				*mode_shm = '0';

			// calculate input to print on lcd
			else{
				if(output_shm[0] == 'A'){
					// calculation for alphabet mode
					typing_alphabet();
				}
				else{
					// calculation for numeric mode
					typing_numeric();
				}

				typing_count(1);
			}

			// flag down to wait new input
			input_shm[10] = '*';
		}
	}

	printf("DEBUG: text editor function ended\n");
	return 0;
}

// change typing mode (alphabet <-> numeric)
int typing_mode(void){
	if(output_shm[0] == 'A')
		output_shm[0] = 'N';
	else
		output_shm[0] = 'A';

	output_shm[39] = '\0';
	output_shm[40] = '\0';

	return 0;
}

// count number of typing
int typing_count(int count){
	int i, num;
	char temp[4];

	// get count data from shared memory
	for(i=0;i<4;i++)
		temp[i] = output_shm[i+1];

	num = atoi(temp) + count;
	if(num > 9999)
		num = 0;

	// copy and transform integer to string
	sprintf(temp, "%04d", num);

	// copy data to shared memory
	for(i=0;i<4;i++)
		output_shm[i+1] = temp[i];

	return 0;
}

// clear lcd display
int typing_clear(void){
	int i;

	for(i=5;i<41;i++)
		output_shm[i] = ' ';

	return 0;
}

// alphabet mode typing
int typing_alphabet(void){
	int i=0, j=0, flag=0, k=0;
	char *s;
	char char_set[MAX_BUTTON][3] = {{'.', 'Q', 'Z'}, {'A', 'B', 'C'},
					{'D', 'E', 'F'}, {'G', 'H', 'I'},
					{'J', 'K', 'L'}, {'M', 'N', 'O'},
					{'P', 'R', 'S'}, {'T', 'U', 'V'},
					{'W', 'X', 'Y'}};

	// find empty space for string buffer
	s = output_shm;
	while(*s != ' '){
		if(*s == '*')
			break;
		s++;
		i++;
	}

	// check which button is pressed
	for(j=0;j<MAX_BUTTON;j++){
		if(input_shm[j] == 1)
			break;
	}

	// output_shm[39] == character pressed just before
	// output_shm[40] == character for multiple press (change alphabet)
	// if string buffer is full, push to left
	if(i == 37){
		// button pressed is different from previous one
		if(output_shm[39] != ('1'+j)){
			for(k=5;k<37;k++)
				output_shm[k] = output_shm[k+1];
			output_shm[36] = char_set[j][0];
			output_shm[39] = '1'+j;
			output_shm[40] = 0;
		}

		// button pressed is same with previous one
		else{
			// decide which character should be written on memory
			if(output_shm[40] == 0){
				output_shm[36] = char_set[j][1];
				output_shm[40] = 1;
			}
			else if(output_shm[40] == 1){
				output_shm[36] = char_set[j][2];
				output_shm[40] = 2;
			}
			else{
				output_shm[36] = char_set[j][0];
				output_shm[40] = 0;
			}
		}
	}
	// if string buffer is not full,
	// write character on proper position
	else{
		// button pressed is different from previous one
		if(output_shm[39] != ('1'+j)){
			output_shm[i] = char_set[j][0];
			output_shm[39] = '1'+j;
			output_shm[40] = 0;
		}

		// button pressed is same with previous one
		else{
			// decide which character should be written on memory
			if(output_shm[40] == 0){
				output_shm[i-1] = char_set[j][1];
				output_shm[40] = 1;
			}
			else if(output_shm[40] == 1){
				output_shm[i-1] = char_set[j][2];
				output_shm[40] = 2;
			}
			else{
				output_shm[i-1] = char_set[j][0];
				output_shm[40] = 0;
			}
		}
	}

	return 0;
}

// numeric mode typing
int typing_numeric(void){
	int i = 0, j;
	char *s;

	// check for empty space for input
	s = output_shm;
	while(*s != ' '){
		if(*s == '*')
			break;
		s++;
		i++;
	}

	// if buffer is reached to max, shift it left
	if(i == 37){
		for(j=5;j<37;j++)
			output_shm[j] = output_shm[j+1];
		i = 36;
	}

	// check which button is pressed
	for(j=0;j<MAX_BUTTON;j++){
		if(input_shm[j] == 1)
			break;
	}

	// copy number to shared memory to print
	switch(j+1){
		case 1:
			output_shm[i] = '1';
			break;
		case 2:
			output_shm[i] = '2';
			break;
		case 3:
			output_shm[i] = '3';
			break;
		case 4:
			output_shm[i] = '4';
			break;
		case 5:
			output_shm[i] = '5';
			break;
		case 6:
			output_shm[i] = '6';
			break;
		case 7:
			output_shm[i] = '7';
			break;
		case 8:
			output_shm[i] = '8';
			break;
		case 9:
			output_shm[i] = '9';
			break;
	}

	return 0;
}

// function for custom mode (mode 3) calculation
int cal_custom(void){
	int i;
	char *s;
	char temp[32] = "Sogang Univ Embedded System HW1 ";

	printf("DEBUG: custom mode function entered\n");

	// copy temp string to shared memory
	for(i=0, s=output_shm;i<32;i++, s++)
		*s = temp[i];

	while(*mode_shm == '3'){
		char ttemp = output_shm[0];

		// shift characters left
		for(i=0;i<31;i++)
			output_shm[i] = output_shm[i+1];
		output_shm[31] = ttemp;

		// pass command to output shared memory
		if(*input_shm == '1'){
			output_shm[32] = '1';
			*input_shm = '*';
		}
		else if(*input_shm == '2'){
			output_shm[32] = '2';
			*input_shm = '*';
		}
		else if(*input_shm == '3'){
			output_shm[32] = '3';
			*input_shm = '*';
		}
		else if(*input_shm == '4'){
			output_shm[32] = '4';
			*input_shm = '*';
		}

		sleep(1);
	}

	printf("DEBUG: custom mode function ended\n");
	return 0;
}

// get all event keys and pass it to main process
static int eventkey_process(void){
	struct input_event ev[BUFF_SIZE];
	int fd, rd, value, size = sizeof(struct input_event);

	// open device driver
	if((fd = open("/dev/input/event1", O_RDONLY)) < 0)
		die("/dev/input/event1 open error");

	printf("DEBUG: event key process entered\n");
	while(*mode_shm != '0'){
		// get event key
		if((rd = read(fd, ev, size*BUFF_SIZE)) < size)
			die("read()");

		if(ev[0].value == KEY_PRESS){
			// stop watch button input
			if(*mode_shm == '1'){
				if(ev[0].code == SW2)
					*input_shm = '2';
				if(ev[0].code == SW3)
					*input_shm = '3';
				if(ev[0].code == SW4)
					*input_shm = '4';
			}

			// custom mode button input
			if(*mode_shm == '3'){
				if(ev[0].code == SW1)
					*input_shm = '1';
				if(ev[0].code == SW2)
					*input_shm = '2';
				if(ev[0].code == SW3)
					*input_shm = '3';
				if(ev[0].code == SW4)
					*input_shm = '4';
			}

			// mode change upward
			if(ev[0].code == SW_UP){
				if(*mode_shm == '1')
					*mode_shm = '2';
				else if(*mode_shm == '2')
					*mode_shm = '3';
				else
					*mode_shm = '1';

				init_shared();
			}

			// mode change downward
			if(ev[0].code == SW_DOWN){
				if(*mode_shm == '1')
					*mode_shm = '3';
				else if(*mode_shm == '2')
					*mode_shm = '1';
				else
					*mode_shm = '2';

				init_shared();
			}

			// terminate program
			if(ev[0].code == SW_QUIT)
				*mode_shm = '0';
		}
	}

	// close device driver
	close(fd);

	printf("DEBUG: event key process ended\n");
	return 0;
}

// get all input and pass it to main process
static int input_process(void){
	int i, dev, buff_size, flag;
	unsigned char push_sw_buff[MAX_BUTTON];

	// open device driver
	if((dev = open("/dev/fpga_push_switch", O_RDWR)) < 0)
		die("/dev/fpga_push_switch open error");
	buff_size = sizeof(push_sw_buff);

	printf("DEBUG: input process entered\n");
	while(*mode_shm != '0'){
		// if mode is 1,
		// sleep the process for better look (stop-watch blinking)
		if(*mode_shm == '1')
			sleep(1);

		if(*mode_shm == '2'){
			char *s;

			flag = 0;
			usleep(50000);

			// check for alphabet/numeric mode
			if(*output_shm != 'A' && *output_shm != 'N')
				*output_shm = 'A';

			// read switch input
			read(dev, &push_sw_buff, buff_size);

			// check for input existence
			for(i=0;i<MAX_BUTTON;i++){
				if(push_sw_buff[i] == 1)
					flag = 1;
			}

			// check if button released with changed
			if(flag == 0 && input_shm[9] == '1' && input_shm[10] == '*'){
				input_shm[9] = '0';
				input_shm[10] = '0';	// notify main to calculate
			}

			// check if button is still pressed
			else if(flag == 1 && input_shm[9] == '1' && input_shm[10] == '*'){
				// change for any additional input (keep previous input)
				for(i=0, s=input_shm;i<MAX_BUTTON;i++, s++){
					if(*s == 1)
						continue;
					else
						*s = push_sw_buff[i];
				}
			}

			// copy first input of a button to shared memory
			else if(flag == 1 && input_shm[9] == '0' && input_shm[10] == '*'){
				for(i=0, s=input_shm;i<MAX_BUTTON;i++, s++){
					*s = push_sw_buff[i];
					input_shm[9] = '1';
				}
			}

			// initialize input buffer
			else{
				for(i=0, s=input_shm;i<MAX_BUTTON;i++, s++)
					*s = push_sw_buff[i];
			}
		}
	}

	// close device driver
	close(dev);

	printf("DEBUG: input process ended\n");
	return 0;
}

// get all data from main process and print it on device
static int output_process(void){
	printf("DEBUG: output process entered\n");
	while(*mode_shm != '0'){
		if(*mode_shm == '1'){
			print_stopwatch();
		}

		if(*mode_shm == '2'){
			print_texteditor();
		}

		if(*mode_shm == '3'){
			print_custom();
		}
	}

	printf("DEBUG: output process ended\n");
	return 0;
}

// print stop watch using FND driver
int print_stopwatch(void){
	int fd, i;
	void *gpl_addr, *gpe_addr, *baseaddr;
	unsigned long *gpe_con = 0;
	unsigned long *gpe_dat = 0;
	unsigned long *gpl_con = 0;
	unsigned long *gpl_dat = 0;
	unsigned long *led_con = 0;
	unsigned long *led_dat = 0;

	printf("DEBUG: print stop watch entered\n");

	// open and initialize FND driver
	if((fd = open("/dev/mem", O_RDWR|O_SYNC)) < 0)
		die("/dev/mem open error");

	gpl_addr = (unsigned long *)mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, fd, IO_GPL_BASE_ADDR);
	if(gpl_addr != NULL){
		gpl_con = (unsigned long *)(gpl_addr + FND_GPL2CON);
		gpl_dat = (unsigned long *)(gpl_addr + FND_GPL2DAT);
	}
	if(*gpl_con == (unsigned long)-1 || *gpl_dat == (unsigned long)-1)
		die("mmap error");

	gpe_addr = (unsigned long *)mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, fd, IO_GPE_BASE_ADDR);
	if(gpe_addr != NULL){
		gpe_con = (unsigned long *)(gpe_addr + FND_GPE3CON);
		gpe_dat = (unsigned long *)(gpe_addr + FND_GPE3DAT);
	}
	if(*gpe_con == (unsigned long)-1 || *gpe_dat == (unsigned long)-1)
		die("mmap error");

	// open and initialize LED driver
	baseaddr = (unsigned long *)mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, fd, IO_BASE_ADDR);
	if(baseaddr != NULL){
		led_con = (unsigned long *)(baseaddr + CON_OFFSET);
		led_dat = (unsigned long *)(baseaddr + DAT_OFFSET);
	}
	if(*led_con == (unsigned long)-1 || *led_dat == (unsigned long)-1)
		die("mmap error");
	*led_con |= 0x11110000;

	// update value of FND
	while(*mode_shm == '1'){
		*gpe_dat = output_shm[0];
		*gpl_dat = output_shm[1];
		*led_dat = output_shm[2];
		*led_dat = output_shm[2];
	}

	// set to default value
	*gpe_dat = 0x96;
	*gpl_dat = 0x03;
	*led_dat = 0xE0;

	// free device
	munmap(gpe_addr, 4096);
	munmap(gpl_addr, 4096);

	// close device driver
	close(fd);

	printf("DEBUG: print stop watch ended\n");

	return 0;
}

// print text editor using fpga drivers
int print_texteditor(void){
	int fpga_dot, str_size;
	int fnd_dev, digit_size, i;
	int text_dev, text_size, chk_size;
	unsigned char data[4];
	unsigned char string[32];

	printf("DEBUG: print text editor entered\n");

	// open and initialize fpga dot driver
	if((fpga_dot = open("/dev/fpga_dot", O_WRONLY)) < 0)
		die("/dev/fpga_dot open error");
	str_size = sizeof(fpga_number[FPGA_NUMBER]);

	// open and initialize fpga fnd driver
	if((fnd_dev = open("/dev/fpga_fnd", O_RDWR)) < 0)
		die("/dev/fpga_fnd open error");
	memset(data, 0, sizeof(data));

	// open and initialize fpga text driver
	if((text_dev = open("/dev/fpga_text_lcd", O_WRONLY)) < 0)
		die("/dev/fpga_text_lcd open error");
	memset(string, 0, sizeof(string));

	// update values for mode 2
	while(*mode_shm == '2'){
		usleep(50000);

		// typing mode (alphabet / numeric)
		if(output_shm[0] == 'N')
			write(fpga_dot, fpga_number[1], str_size);
		else
			write(fpga_dot, fpga_number[10], str_size);

		// print number of count
		for(i=0;i<4;i++)
			data[i] = output_shm[i+1];
		write(fnd_dev, &data, 4);

		// print text on lcd
		if(output_shm[5] != '*'){
			for(i=0;i<32;i++)
				string[i] = output_shm[i+5];

			write(text_dev, string, BUFF_SIZE);
		}
	}

	// set to default value for each device (just for clean look)
	write(fpga_dot, fpga_number[10], str_size);
	for(i=0;i<4;i++)
		data[i] = '0';
	write(fnd_dev, &data, 4);
	for(i=0;i<32;i++)
		string[i] = ' ';
	write(text_dev, string, BUFF_SIZE);

	// close all device driver
	close(fpga_dot);
	close(fnd_dev);
	close(text_dev);

	printf("DEBUG: print text editor ended\n");
	return 0;
}

// print custom mode
int print_custom(void){
	int text_dev, i;
	unsigned char string[32], j = 0;

	int dot_dev, dot_size;
	int buzzer_dev;
	int motor_dev, motor_size;
	unsigned char motor_state[3] = {0, 0, 10};
	unsigned char data;

	printf("DEBUG: print custom mode entered\n");

	// open and initialize fpga text driver
	if((text_dev = open("/dev/fpga_text_lcd", O_WRONLY)) < 0)
		die("/dev/fpga_text_lcd open error");
	memset(string, 0, sizeof(string));

	// open and initialize fpga dot driver
	if((dot_dev = open("/dev/fpga_dot", O_WRONLY)) < 0)
		die("/dev/fpga_dot open error");
	dot_size = sizeof(fpga_number[FPGA_NUMBER]);

	// open and initialize fpga motor
	motor_dev = open("/dev/fpga_step_motor", O_WRONLY);

	//open and initialize buzzer driver
	buzzer_dev = open("/dev/fpga_buzzer", O_RDWR);
	data = 0;

	while(*mode_shm == '3'){
		// print text on lcd display
		for(i=0;i<32;i++)
			string[i] = output_shm[i];
		write(text_dev, string, BUFF_SIZE);

		// print text on dot driver
		write(dot_dev, fpga_number[11+j], dot_size);
		if(++j == RUN)
			j = 0;

		// check for motor state
		if(output_shm[32] == '1'){
			if(motor_state[0] == '1')
				motor_state[0] = '0';
			else
				motor_state[0] = '1';

			output_shm[32] = '*';
		}
		else if(output_shm[32] == '2'){
			if(motor_state[1] == '1')
				motor_state[1] = '0';
			else
				motor_state[1] = '1';

			output_shm[32] = '*';
		}

		// check for buzzer state
		if(output_shm[32] == '3'){
			data = 1;
			output_shm[32] = '*';
		}
		else if(output_shm[32] == '4'){
			data = 0;
			output_shm[32] = '*';
		}

		write(motor_dev, motor_state, 3);
		write(buzzer_dev, &data, 1);

		sleep(1);
	}

	// set display for default value (just for better look)
	for(i=0;i<32;i++)
		string[i] = ' ';
	write(text_dev, string, BUFF_SIZE);
	write(dot_dev, fpga_number[10], dot_size);
	motor_state[0] = '0';
	motor_state[1] = '0';
	write(motor_dev, motor_state, 3);
	data = 0;
	write(buzzer_dev, &data, 1);


	printf("DEBUG: print custom mode ended\n");

	// close devices
	close(text_dev);
	close(dot_dev);
	close(motor_dev);
	close(buzzer_dev);

	return 0;
}

int main(int argc, char *argv[]){
	pid_t pid;

	// initialize shared memory for IPCs
	shared_memory();

	pid = fork();
	switch(pid){
		case -1:
			// process creation failed
			die("process creation failed");
		case 0:
			// input process
			pid = fork();
			switch(pid){
				case -1:
					// process creation failed
					die("process creation failed");
				case 0:
					// event key process (input)
					return eventkey_process();
				default:
					// input process
					return input_process();
			}
		default:
			pid = fork();
			switch(pid){
				case -1:
					// process creation failed
					die("process creation failed");
				case 0:
					// output process
					return output_process();
				default:
					// main process
					main_process();
			}
	}

	free_shared();

	return 0;
}
