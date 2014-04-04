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

#define BUFF_SIZE 64
#define VU 115
#define VD 114

void die(char *s);
void input_process(void);
void output_process(void);
void main_process(void);

int mode_index;
char *mode[3] = {"stop watch", "text editor", "extra"};

int main(int argc, char *argv[]){
	pid_t pid;
	mode_index = 0;

	pid = fork();

	switch(pid){
		case -1:
			printf("Create child process failed\n");
			return -1;
		case 0:
			input_process();
		default:
			// Main process
			pid = fork();
			switch(pid){
				case 0:
					output_process();
				default:
					main_process();
			}
	}
	return 0;
}

void die(char *s){
	perror(s);
	exit(1);
}

void main_process(void){
	int shmid;
	key_t key;
	int *shm, *s;

	// Naming shared memory segment
	key = 1111;

	// Create the segment
	if((shmid = shmget(key, 4, IPC_CREAT | 0666)) < 0)
		die("shmget");

	// Attaching the segment to memory space
	if((shm = shmat(shmid, NULL, 0)) == (int *)-1)
		die("shmat");

	while(1){
		if(*shm != '*'){
			mode_index = *s;
			printf("%d\n", mode_index);
			*shm = '*';
		}
	}
}

void input_process(void){
	int shmid;
	key_t key;
	int *shm, *s;

	struct input_event ev[BUFF_SIZE];
	int fd, rd, value, size = sizeof(struct input_event);
	char *device = "/dev/input/event1";

	// Naming shared memory segment
	key = 1111;

	// Create the segment
	if((shmid = shmget(key, 4, IPC_CREAT | 0666)) < 0)
		die("shmget");

	// Attaching the segment to memory space
	if((shm = shmat(shmid, NULL, 0)) == (int *)-1)
		die("shmat");

	s = shm;

	if((fd = open(device, O_RDONLY)) == -1){
		printf("%s is not a valid device\n", device);
		exit(1);
	}

	while(1){
		rd = read(fd, ev, size*BUFF_SIZE);

		value = ev[0].value;

		if(value == KEY_PRESS){
			if(ev[0].code == VU){
				mode_index++;
				if(mode_index == 3)
					mode_index = 0;
				*s = mode_index;
			} else if(ev[0].code == VD){
				mode_index--;
				if(mode_index == -1)
					mode_index = 2;
				*s = mode_index;
			}
		}
	}
}

void output_process(void){
	while(1){
		exit(1);
	}
}
