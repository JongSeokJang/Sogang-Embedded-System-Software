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
	*input_shm = '*';
	*output_shm = '*';
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
	if((input_shmid = shmget(input_key, 4, IPC_CREAT|0660)) < 0)
		die("input shmget");
	if((output_shmid = shmget(output_key, 4, IPC_CREAT|0666)) < 0)
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
		}

		if(*mode_shm == '3'){
		}
	}

	printf("DEBUG: main process ended\n");
	return 0;
}

// function for stop watch (mode 1) calculation
static int cal_stopwatch(void){
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
					for(i=0;i<300;i++){
						output_shm[0] = 0x02;
						output_shm[1] = fnd_num[ttime/60/10];
					}

					for(i=0;i<300;i++){
						output_shm[0] = 0x04;
						output_shm[1] = fnd_num[ttime/60%10] - 0x01;
					}

					for(i=0;i<300;i++){
						output_shm[0] = 0x10;
						output_shm[1] = fnd_num[ttime%60/10];
					}

					for(i=0;i<300;i++){
						output_shm[0] = 0x80;
						output_shm[1] = fnd_num[ttime%60%10];
					}

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
			}
		}
	}

	printf("DEBUG: stop watch function ended\n");
	return 0;
}

// get all event keys and pass it to main process
static int eventkey_process(void){
	struct input_event ev[BUFF_SIZE];
	int fd, rd, value, size = sizeof(struct input_event);

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

	printf("DEBUG: event key process ended\n");
	return 0;
}

// get all input and pass it to main process
static int input_process(void){
	prinf("DEBUG: input process entered\n");
	while(*mode_shm != '0'){
	}

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
		}

		if(*mode_shm == '3'){
		}
	}

	printf("DEBUG: output process ended\n");
	return 0;
}

// print stop watch using FND driver
static int print_stopwatch(void){
	int fd;
	void *gpl_addr, *gpe_addr, baseaddr;
	unsigned long *gpe_con = 0;
	unsigned long *gpe_dat = 0;
	unsigned long *gpl_con = 0;
	unsigned long *gpl_dat = 0;
	unsigned long *led_con = 0;
	unsigned long *led_dat = 0;

	// open and initialize FND driver
	if((fd = open("/dev/mem", O_RDWR|O_SYNC)) < 0)
		die("/dev/mem open error");

	gpl_addr = (unsigned long *)mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, fd, IO_GPL_BASE_ADDR);
	if(gpl_addr != NULL){
		gpl_con = (unsigned long *)(gpl_addr + FND_GPL2CON);
		gpl_dat = (unsigned long *)(gpl_addr + FND_GPL2DAT);
	}
	if(*gpl_con == (unsigned long)-1 || *gpl_dat = (unsigned long)-1)
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

	while(*mode_shm == '1'){
		*gpe_dat = output_shm[0];
		*gpl_dat = output_shm[1];
		*led_dat = output_shm[2];
	}

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
					return main_process();
			}
	}
	return 0;
}
