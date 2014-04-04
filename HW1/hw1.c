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
#define FND_GPE3Dat 0x00144

int shmid;
key_t key;
char *shm, *s;

int main_process(void){
	return 0;
}

int input_process(void){
	return 0;
}

int output_process(void){
	return 0;
}

int main(int argc, char *argv[]){
	pid_t pid;

	// Naming shared memory segment
	key = 1111;

	// Create the segment
	if((shmid = shmget(key, 1024, IPC_CREAT | 0666)) < 0){
		perror("shmget");
		exit(1);
	}

	// Attaching the segment to memory space
	if((shm = shmat(shmid, NULL, 0)) == (char *)-1){
		perror("shmat");
		exit(1);
	}

	s = shm;

	pid = fork();
	switch(pid){
		case -1:
			printf("Process create failed\n");
			return -1;

		case 0:
			// Input process
			input_process();

		default:
			pid = fork();
			switch(pid){
				case -1:
					printf("Process create failed\n");
					return -1;

				case 0:
					// Output process
					output_process();

				default:
					// Main process
					main_process();
			}
	}

	return 0;
}
