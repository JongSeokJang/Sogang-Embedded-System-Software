#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <linux/input.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <termios.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>

#include "./fpga_dot_font.h"

#define KEY_RELEASE 0
#define KEY_PRESS 1

#define SW1 139
#define SW2 102
#define SW3 158
#define SW4 217
#define VOLUME_UP 115
#define VOLUME_DOWN 114
#define SW_QUIT 116

#define IO_BASE_ADDR 0x11400000
#define CON_OFFSET 0x40
#define DAT_OFFSET 0x44

#define FND0	0x00
#define FND1	0x01
#define FND2	0x02
#define FND3	0x03
#define FND4	0x04
#define FND5	0x05
#define FND6	0x06
#define FND7	0x07
#define FND8	0x08
#define FND9	0x09
#define FND_OFF	0x0A
#define FND_S	0x0B
#define FND_T	0x0C
#define FND_O	0x0D
#define FND_P	0x0E

#define IO_GPL_BASE_ADDR 0x11000000
#define FND_GPL2CON 0x0100
#define FND_GPL2DAT 0x0104

#define IO_GPE_BASE_ADDR 0x11400000
#define FND_GPE3CON 0x00140
#define FND_GPE3DAT 0x00144

#define BUFF_SIZE 64

int input_shmid, output_shmid, mode_shmid;
key_t input_key, output_key, mode_key;
char *input_shm, *output_shm, *mode_shm, *s;

int input_process(void){
	struct input_event ev[BUFF_SIZE];
	int fd, rd, value, size = sizeof (struct input_event);
	int mode_num;
	char temp[BUFF_SIZE];

	// Mode selection device open
	char *device = "/dev/input/event1";
	if((fd = open(device, O_RDONLY)) == -1)
		printf("%s is not a valid device\n", device);

	s = input_shm;
	while(1){
		mode_num = atoi(mode_shm);

		if(mode_num == 0)
			return 0;

		if((rd = read(fd, ev, size*BUFF_SIZE)) < size){
			printf("read()");
			return 0;
		}

		// Key pressed
		if(ev[0].value == KEY_PRESS){
			// Check for mode
			switch(mode_num){
				case 1:
					if(ev[0].code == SW2)
						strcpy(s, "2");
					if(ev[0].code == SW3)
						strcpy(s, "3");
					if(ev[0].code == SW4)
						strcpy(s, "4");
					break;
				case 2:
					break;
				case 3:
					break;
			}

			// Mode switching upward
			if(ev[0].code == VOLUME_UP){
				switch(mode_num){
					case 1:
						*mode_shm = '2';
						break;
					case 2:
						*mode_shm = '3';
						break;
					case 3:
						*mode_shm = '1';
						break;
				}
			}

			// Mode switching downward
			if(ev[0].code == VOLUME_DOWN){
				switch(mode_num){
					case 1:
						*mode_shm = '3';
						break;
					case 2:
						*mode_shm = '1';
						break;
					case 3:
						*mode_shm = '2';
						break;
				}
			}

			// Power off button
			if(ev[0].code == SW_QUIT){
				*mode_shm = '0';
			}
		}
	}

	return 0;
}

int func_stopwatch(int code){
	int i;
	if(code == 2){
		output_shm[0] = '1';
		output_shm[1] = '2';
	}
	return 0;
}

int main_process(void){
	int mode_num;

	while(1){
		mode_num = atoi(mode_shm);

		if(mode_num == 0)
			return 0;

		// Functions for each selected mode
		switch(mode_num){
			case 1:
				if(*input_shm != '*'){
					int code = atoi(input_shm);
					func_stopwatch(code);
					*input_shm = '*';
				}
				break;
			case 2:
				break;
			case 3:
				break;
		}
		/*
		if(*input_shm != '*'){
			s = output_shm;
			strcpy(s, input_shm);

			*input_shm = '*';
		}
		*/
	}
	return 0;
}

int output_process(void){
	int fpga_dot, mode_num, str_size;

	// Test
	int fnd_fd;
	void *gpladdr, *gpe_addr;
	unsigned long *gpe_con=0;
	unsigned long *gpe_dat=0;
	unsigned long *gpl_con=0;
	unsigned long *gpl_dat=0;
	int ttime, i;
	float start_time, end_time;
	// Test

	fpga_dot = open("/dev/fpga_dot", O_WRONLY);
	if(fpga_dot < 0){
		perror("FPGA dot driver open failed\n");
		*mode_shm = '0';
	}

	fnd_fd = open("/dev/mem", O_RDWR|O_SYNC);
	if(fnd_fd < 0){
		perror("/dev/mem open error");
		*mode_shm = '0';
	}
	gpladdr = (unsigned long *)mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, fnd_fd, IO_GPL_BASE_ADDR);
	if(gpladdr != NULL){
		gpl_con = (unsigned long *)(gpladdr + FND_GPL2CON);
		gpl_dat = (unsigned long *)(gpladdr + FND_GPL2DAT);
	}
	if(*gpl_con == (unsigned long)-1 || *gpl_dat == (unsigned long)-1){
		perror("mmap error!");
		*mode_shm = '0';
	}
	gpe_addr = (unsigned long *)mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, fnd_fd, IO_GPE_BASE_ADDR);
	if(gpe_addr != NULL){
		gpe_con = (unsigned long *)(gpe_addr + FND_GPE3CON);
		gpe_dat = (unsigned long *)(gpe_addr + FND_GPE3DAT);
	}
	if(*gpe_con == (unsigned long)-1 || *gpe_dat == (unsigned long)-1){
		perror("mmap error!\n");
		*mode_shm = '0';
	}

	*gpl_con = 0x11111111;
	*gpe_con = 0x10010110;

	ttime = 0;
	*gpl_dat = 0x02;
	*gpl_dat = 0x02;
	*gpe_dat = 0x96;
	sleep(1);

	while(1){
		mode_num = atoi(mode_shm);

		if(mode_num == 0)
			return 0;
		else{
			str_size = sizeof(fpga_number[mode_num]);
			write(fpga_dot, fpga_number[mode_num], str_size);
		}

		ttime++;
		time(&start_time);
		end_time = 0;

		while(difftime(end_time, start_time) <= 0.99999){
			for(i=0;i<500;i++){
				*gpe_dat = 0x02;
				*gpl_dat = 0x9F;
			}
			for(i=0;i<500;i++){
				*gpe_dat = 0x04;
				*gpl_dat = 0x25;
			}
			for(i=0;i<500;i++){
				*gpe_dat = 0x10;
				*gpl_dat = 0x0D;
			}
			for(i=0;i<500;i++){
				*gpe_dat = 0x80;
				*gpl_dat = 0x99;
			}

			time(&end_time);
		}

		/*
		// Output for stop watch mode
		if(mode_num == 1 && *output_shm != '*'){
			char number, number2;
			int position, value;
			unsigned short temp;
			unsigned char temp1, temp2;

			s = output_shm;

			number = (char)atoi(&output_shm[0]);
			printf("%c!!\n", number);
			switch(number){
				case 0:
					position = 0x96;
					break;
				case 1:
					position = 0x02;
					break;
				case 2:
					position = 0x04;
					break;
				case 3:
					position = 0x10;
					break;
				case 4:
					position = 0x80;
					break;
				case 5:
					position = 0x12;
					break;
			}

			*s++;
			number2 = (char)atoi(s);
			printf("%c!!!!\n", number2);
			switch(number2){
				case 0:
					value = FND0;
					break;
				case 1:
					value = FND1;
					break;
				case 2:
					value = FND2;
					break;
				case 3:
					value = FND3;
					break;
				case 4:
					value = FND4;
					break;
				case 5:
					value = FND5;
					break;
				case 6:
					value = FND6;
					break;
				case 7:
					value = FND7;
					break;
				case 8:
					value = FND8;
					break;
				case 9:
					value = FND9;
					break;
			}

			temp1 = position;
			temp2 = value;
			temp = temp + temp1;
			temp = (temp<<8)|temp2;
			write(fnd_fd, &temp, sizeof(short));

			*output_shm = '*';
		}*/

		/*if(*output_shm != '*'){
			for(s=output_shm;*s!='\0';s++)
				putchar(*s);
			putchar('\n');

			*output_shm = '*';
		}*/
	}
	return 0;
}

int init_shared_memory(void){
	// Naming shared memory segments
	mode_key = 1111;
	input_key = 2222;
	output_key = 3333;

	// Create the segments
	if((mode_shmid = shmget(mode_key, 8, IPC_CREAT | 0600)) < 0){
		perror("mode shmget");
		exit(1);
	}
	if((input_shmid = shmget(input_key, 1024, IPC_CREAT | 0660)) < 0){
		perror("input shmget");
		exit(1);
	}
	if((output_shmid = shmget(output_key, 1024, IPC_CREAT | 0666)) < 0){
		perror("output shmget");
		exit(1);
	}

	// Attaching the segments to memory space
	if((mode_shm = shmat(mode_shmid, NULL, 0)) == (char *)-1){
		perror("mode shmat");
		exit(1);
	}
	if((input_shm = shmat(input_shmid, NULL, 0)) == (char *)-1){
		perror("input shmat");
		exit(1);
	}
	if((output_shm = shmat(output_shmid, NULL, 0)) == (char *)-1){
		perror("output shmat");
		exit(1);
	}

	// Initialize mode to default(1)
	*mode_shm = '1';
}

int main(int argc, char *argv[]){
	pid_t pid;

	init_shared_memory();

	pid = fork();
	switch(pid){
		case -1:
			// Process creation failed
			printf("Process create failed\n");
			return -1;
		case 0:
			// Input process
			return input_process();
		default:
			pid = fork();

			switch(pid){
				case -1:
					// Process creation failed
					printf("Process create failed\n");
					return -1;
				case 0:
					// Output process
					return output_process();
				default:
					// Main process
					return main_process();
			}
	}

	return 0;
}
