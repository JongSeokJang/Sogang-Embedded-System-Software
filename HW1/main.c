#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAXSIZE 27

void die(char *s){
	perror(s);
	exit(1);
}

int main(void){
	pid_t pid;
	int shmid;
	key_t key;
	char *shm, *s;

	// Naming shared memory segment
	key = 5678;

	// Create the segment
	if((shmid = shmget(key, MAXSIZE, IPC_CREAT | 0666)) < 0){
		die("shmget");
	}

	// Attaching the segment to memory space
	if((shm = shmat(shmid, NULL, 0)) == (char *)-1){
		die("shmat");
	}

	// Create child process
	pid = fork();

	switch(pid){
		case -1:
			printf("Create child failed\n");
			return -1;
		case 0:
			// Put data into the memory for other process to read
			s = shm;

			while(1){
				// Get input data from user
				scanf("%s", s);
			}
		default:
			while(1){
				if(*shm != '*'){
					for(s=shm;*s!='\0';s++)
						putchar(*s);
					putchar('\n');

					*shm = '*';
				}
			}
	}
	return 0;
}
