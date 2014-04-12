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

#define BUFF_SIZE 64
#define MAX_BUTTON 9

#define KEY_RELEASE 0
#define KEY_PRESS 1

#define SW1 139
#define SW2 102
#define SW3 158
#define SW4 217
#define SW_UP 115
#define SW_DOWN 114
#define SW_QUIT 116

int input_shmid, output_shmid, mode_shmid;
key_t input_key, output_key, mode_key;
char *input_shm, *output_shm, *mode_shm, *s;

int input_process(void);
int eventkey_process(void);
int output_process(void);
int main_process(void);
void die(char *);
void init_shared(void);
int shared_memory(void);
void user_signal1(int);

int main(int argc, char *argv[]){
	pid_t pid;

	// Initialize shared memory for IPCs
	shared_memory();

	pid = fork();
	switch(pid){
		case -1:
			// process creation failed
			printf("process creation failed\n");
			return -1;
		case 0:
			// input process
			return input_process();
		default:
			pid = fork();
			switch(pid){
				case -1:
					// process creation failed
					printf("process creation failed\n");
					return -1;
				case 0:
					// output process
					return output_process();
				default:
					// main process
					return main_process();
			}
	}

	return 0;
}

void die(char *str){
	perror(str);
	*mode_shm = '0';
	exit(1);
}

void init_shared(void){
	*input_shm = '*';
	*output_shm = '*';
}

int shared_memory(void){
	// naming shared memory segments
	mode_key = 1111;
	input_key = 2222;
	output_key = 3333;

	// create the segments
	if((mode_shmid = shmget(mode_key, 8, IPC_CREAT|0600)) < 0)
		die("mode shmget");
	if((input_shmid = shmget(input_key, 128, IPC_CREAT|0660)) < 0)
		die("input shmget");
	if((output_shmid = shmget(output_key, 128, IPC_CREAT|0666)) < 0)
		die("output shmget");

	// attaching the segments to memory space
	if((mode_shm = shmat(mode_shmid, NULL, 0)) == (char *)-1)
		die("mode shmat");
	if((input_shm = shmat(input_shmid, NULL, 0)) == (char *)-1)
		die("input shmat");
	if((output_shm = shmat(output_shmid, NULL, 0)) == (char *)-1)
		die("output shmat");

	// initialize data to default value
	*mode_shm = '1';
	*input_shm = '*';
	*output_shm = '*';

	return 0;
}

int eventkey_process(void){
	struct input_event ev[BUFF_SIZE];
	int fd, rd, value, size = sizeof(struct input_event);

	if((fd = open("/dev/input/event1", O_RDONLY)) < 0)
		die("/dev/input/event1 open error");

	while(*mode_shm != '0'){
		// get input key
		if((rd = read(fd, ev, size*BUFF_SIZE)) < size)
			die("read()");

		// mode changing button pressed
		if(ev[0].value == KEY_PRESS){
			// stop-watch button input
			if(*mode_shm == '1'){
				if(ev[0].code == SW2)
					*output_shm = '2';
				if(ev[0].code == SW3)
					*output_shm = '3';
				if(ev[0].code == SW4)
					*output_shm = '4';
			}

			// mode changing upward
			if(ev[0].code == SW_UP){
				if(*mode_shm == '1')
					*mode_shm = '2';
				else if(*mode_shm == '2')
					*mode_shm = '3';
				else
					*mode_shm = '1';

				init_shared();
			}

			// mode changing downward
			if(ev[0].code == SW_DOWN){
				if(*mode_shm == '1')
					*mode_shm = '3';
				else if(*mode_shm == '2')
					*mode_shm = '1';
				else
					*mode_shm = '2';

				init_shared();
			}

			// quit program
			if(ev[0].code == SW_QUIT)
				*mode_shm = '0';
		}
	}

	return 0;
}

int input_process(void){
	pid_t pid = fork();

	switch(pid){
		case -1:
			printf("process creation failed!\n");
			return -1;
		case 0:
			// mode switching and mode 1
			return eventkey_process();
		default:
			// other mode below
			break;
	}

	int switch_dev, buff_size;
	unsigned char push_sw_buff[MAX_BUTTON];

	// open push switch device
	if((switch_dev = open("/dev/fpga_push_switch", O_RDWR)) < 0)
		die("/dev/fpga_push_switch open error");
	buff_size = sizeof(push_sw_buff);

	while(*mode_shm != '0'){
		// text editor switch button input
		if(*mode_shm == '2' && *input_shm == '*'){
			char temp[10];
			int i, flag = 0;

			read(switch_dev, &push_sw_buff, buff_size);
			// detect which button is pressed
			for(i=0;i<MAX_BUTTON;i++){
				if(push_sw_buff[i] == 1){
					temp[i] = '1';
					flag = 1;
				}
				else
					temp[i] = '0';
			}

			// copy input to shared memory only when data is in
			if(flag == 1){
				s = input_shm;
				strcpy(s, temp);
			}
		}
	}

	/*
	struct input_event ev[BUFF_SIZE];
	int fd, rd, value, size = sizeof(struct input_event);

	int switch_dev, buff_size, i, flag;
	unsigned char push_sw_buff[MAX_BUTTON];

	if((fd = open("/dev/input/event1", O_RDONLY)) < 0)
		die("/dev/input/event1 open error");

	// open push switch device
	if((switch_dev = open("/dev/fpga_push_switch", O_RDWR)) < 0)
		die("/dev/fpga_push_switch open error");
	buff_size = sizeof(push_sw_buff);

	while(*mode_shm != '0'){
		// TODO one more fork for mode switching???
		// TODO test if this affect while statement
		if((rd = read(fd, ev, size*BUFF_SIZE)) < size)
			die("read()");

		// key is pressed
		if(ev[0].value == KEY_PRESS){
			// stop-watch button input
			if(*mode_shm == '1'){
				if(ev[0].code == SW2)
					*output_shm = '2';
				if(ev[0].code == SW3)
					*output_shm = '3';
				if(ev[0].code == SW4)
					*output_shm = '4';
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

			if(ev[0].code == SW_QUIT)
				*mode_shm = '0';

			// custom mode button input
			if(*mode_shm == '3'){
			}
		}

		// text editor switch button input
		if(*mode_shm == '2'){
			while(*input_shm == '*'){
				char temp[10];

				read(switch_dev, &push_sw_buff, buff_size);
				flag = 0;
				for(i=0;i<MAX_BUTTON;i++){
					if(push_sw_buff[i] == 1){
						temp[i] = '1';
						flag = 1;
					}
					else
						temp[i] = '0';
				}

				// copy input to shared memory only when data is in
				if(flag == 1){
					s = input_shm;
					strcpy(s, temp);
				}
			}
		}
	}*/
	return 0;
}

int main_process(void){
	int Alpha_Num_mode = 0;
	while(*mode_shm != '0'){
		if(*mode_shm == '2'){
			if(*input_shm != '*' && *output_shm == '*'){
				int i;
				char temp[10];
				s = input_shm;
				for(i=0;i<10;i++, s++)
					temp[i] = *s;
				*input_shm = '*';

				if(temp[4] == '1' && temp[5] == '1'){
					if(Alpha_Num_mode == 0){
						temp[0] = 'A';
						temp[1] = '\0';
						Alpha_Num_mode = 1;
					}
					else{
						temp[0] = 'N';
						temp[1] = '\0';
						Alpha_Num_mode = 0;
					}
				}
				strcpy(output_shm, temp);
				printf("input:%s\n", input_shm);
				printf("temp:%s\n", temp);
			}
		}
		sleep(0.1);
	}
	return 0;
}

int output_process(void){
	int mmap_fd, i, ttime;
	void *gpl_addr, *gpe_addr;
	time_t start_time, end_time;
	unsigned long *gpe_con = 0;
	unsigned long *gpe_dat = 0;
	unsigned long *gpl_con = 0;
	unsigned long *gpl_dat = 0;
	unsigned long fnd_number[10] = {0x03, 0x9F, 0x25, 0x0D, 0x99, 0x49, 0xC1, 0x1F, 0x01, 0x09};

	void *baseaddr;
	unsigned long *led_con = 0;
	unsigned long *led_dat = 0;
	int flag = 1;

	int fpga_dot, str_size;

	// initialize fnd device and mmaping it
	mmap_fd = open("/dev/mem", O_RDWR|O_SYNC);
	if(mmap_fd < 0)
		die("/dev/mem open error");

	gpl_addr = (unsigned long *)mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, mmap_fd, IO_GPL_BASE_ADDR);
	if(gpl_addr != NULL){
		gpl_con = (unsigned long *)(gpl_addr + FND_GPL2CON);
		gpl_dat = (unsigned long *)(gpl_addr + FND_GPL2DAT);
	}
	if(*gpl_con == (unsigned long)-1 || *gpl_dat == (unsigned long)-1)
		die("mmap error!");

	gpe_addr = (unsigned long *)mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, mmap_fd, IO_GPE_BASE_ADDR);
	if(gpe_addr != NULL){
		gpe_con = (unsigned long *)(gpe_addr + FND_GPE3CON);
		gpe_dat = (unsigned long *)(gpe_addr + FND_GPE3DAT);
	}
	if(*gpe_con == (unsigned long)-1 || *gpe_dat == (unsigned long)-1)
		die("mmap error!");

	// initialize led driver
	baseaddr = (unsigned long *)mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, mmap_fd, IO_BASE_ADDR);
	if(baseaddr != NULL){
		led_con = (unsigned long *)(baseaddr + CON_OFFSET);
		led_dat = (unsigned long *)(baseaddr + DAT_OFFSET);
	}
	if(*led_con == (unsigned long)-1 || *led_dat == (unsigned long)-1)
		die("mmap error!");
	*led_con |= 0x11110000;

	// initialize fpga dot driver
	if((fpga_dot = open("/dev/fpga_dot", O_WRONLY)) < 0)
		die("/dev/fpga_dot open failed");
	str_size = sizeof(fpga_number[10]);

	while(*mode_shm != '0'){
		// stop-watch mode
		if(*mode_shm == '1'){
			// initialize timer
			if(*output_shm == '*' || *output_shm == '2'){
				ttime = 0;
				*gpe_dat = 0x96;
				*gpl_dat = 0x03;
				*led_dat = 0xE0;
			} else{
				// start timer
				while(*output_shm == '4' || *output_shm == '3'){
					time(&start_time);
					end_time = 0;

					// this will take approximately one second
					while(difftime(end_time, start_time) <= 0.99999){
						for(i=0;i<500;i++){
							*gpe_dat = 0x02;
							*gpl_dat = fnd_number[ttime/60/10];
						}

						for(i=0;i<500;i++){
							*gpe_dat = 0x04;
							*gpl_dat = fnd_number[ttime/60%10]-0x01;
						}

						for(i=0;i<500;i++){
							*gpe_dat = 0x10;
							*gpl_dat = fnd_number[ttime%60/10];
						}

						for(i=0;i<500;i++){
							*gpe_dat = 0x80;
							*gpl_dat = fnd_number[ttime%60%10];
						}

						time(&end_time);
					}

					if(*output_shm == '4'){
						ttime++;
						*led_dat = 0x30;
					} else{
						if(flag){
							*led_dat = 0xB0;
							flag = 0;
						}
						else{
							*led_dat = 0x70;
							flag = 1;
						}
					}
				}
			}
		}

		// text editor mode
		if(*mode_shm == '2'){
			// changing to alphabet mode
			if(*output_shm == 'A'){
				write(fpga_dot, fpga_number[10], str_size);
				*output_shm = '*';
			}

			// changing to numeric mode
			if(*output_shm == 'N'){
				write(fpga_dot, fpga_number[1], str_size);
				*output_shm = '*';
			}

			if(*output_shm != '*'){
				printf("out_bef:%s\n", output_shm);
				*output_shm = '*';
				printf("out_aft:%s\n", output_shm);
			}

			// fpga-fnd print count for number of key pressed
			// display text on lcd display
			// dot matrix for alphabet mode and numeric mode
		}

		// custom mode
		if(*mode_shm == '3'){
			//printf("Now is mode 3\n");
			//sleep(3);
		}

		sleep(0.1);
	}

	return 0;
}
