#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#define DEV_NAME "/dev/stopwatch"

int main(int argc, char *argv[]){
	int dev;

	dev = open(DEV_NAME, O_WRONLY);
	if(dev < 0){
		printf("Device open error : %s\n", DEV_NAME);
		exit(1);
	}

	return 0;
}
