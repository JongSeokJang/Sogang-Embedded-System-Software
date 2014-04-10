#include <sys/mman.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#define IO_GPL_BASE_ADDR 0x11000000
#define FND_GPL2CON 0x0100
#define FND_GPL2DAT 0x0104

#define IO_GPE_BASE_ADDR 0x11400000
#define FND_GPE3CON 0x00140
#define FND_GPE3DAT 0x00144

#define BUFF_SIZE 64

#define KEY_RELEASE 0
#define KEY_PRESS 1

#define SW1 139
#define SW2 102
#define SW3 158
#define SW4 217

int input_shmid, output_shmid, mode_shmid;
key_t input_key, output_key, mode_key;
char *input_shm, *output_shm, *mode_shm;

int input_process(void);
int output_process(void);
int main_process(void);
void die(char *);
int shared_memory(void);

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

int input_process(void){
	struct input_event ev[BUFF_SIZE];
	int fd, rd, value, size = sizeof(struct input_event);

	if((fd = open("/dev/input/event1", O_RDONLY)) < 0)
		die("/dev/input/event1 open error");

	while(*mode_shm != '0'){
		if((rd = read(fd, ev, size*BUFF_SIZE)) < size)
			die("read()");

		// key is pressed
		if(ev[0].value == KEY_PRESS){
			if(ev[0].code == SW2)
				*output_shm = '2';
			if(ev[0].code == SW3)
				*output_shm = '3';
			if(ev[0].code == SW4)
				*output_shm = '4';
		}
	}
	return 0;
}

int main_process(void){
	while(*mode_shm != '0'){
		printf("main\n");
		sleep(3);
	}
	return 0;
}

int output_process(void){
	int fnd_fd, i;
	void *gpl_addr, *gpe_addr;
	time_t start_time, end_time;
	unsigned long *gpe_con = 0;
	unsigned long *gpe_dat = 0;
	unsigned long *gpl_con = 0;
	unsigned long *gpl_dat = 0;
	unsigned long fnd_number[10] = {0x03, 0x9F, 0x25, 0x0D, 0x99, 0x49, 0xC1, 0x1F, 0x01, 0x09};

	// initialize fnd device and mmaping it
	fnd_fd = open("/dev/mem", O_RDWR|O_SYNC);
	if(fnd_fd < 0)
		die("/dev/mem open error");

	gpl_addr = (unsigned long *)mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, fnd_fd, IO_GPL_BASE_ADDR);
	if(gpl_addr != NULL){
		gpl_con = (unsigned long *)(gpl_addr + FND_GPL2CON);
		gpl_dat = (unsigned long *)(gpl_addr + FND_GPL2DAT);
	}
	if(*gpl_con == (unsigned long)-1 || *gpl_dat == (unsigned long)-1)
		die("mmap error!");

	gpe_addr = (unsigned long *)mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, fnd_fd, IO_GPE_BASE_ADDR);
	if(gpe_addr != NULL){
		gpe_con = (unsigned long *)(gpe_addr + FND_GPE3CON);
		gpe_dat = (unsigned long *)(gpe_addr + FND_GPE3DAT);
	}
	if(*gpe_con == (unsigned long)-1 || *gpe_dat == (unsigned long)-1)
		die("mmap error!");

	while(*mode_shm != '0'){
		// stop-watch mode
		if(*mode_shm == '1'){
			// initialize timer
			if(*output_shm == '*'){
				ttime = 0;
				*gpe_dat = 0x96;
				*gpl_dat = 0x02;
			} else{
				while(*output_shm == '4'){
					time(&start_time);
					end_time = 0;

					while(difftime(end_time, start_time) <= 0.99999){
						for(i=0;i<500;i++){
							*gpe_dat = 0x02;
							*gpl_dat = fnd_number[ttime/60/10];
						}

						for(i=0;i<500;i++){
							*gpe_dat = 0x04;
							*gpl_dat = fnd_number[ttime/60%10];
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

					ttime++;
				}
			}
		}
	}

	return 0;
}
