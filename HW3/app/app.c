#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#define DEV_NAME "/dev/stopwatch"

int main(int argc, char *argv[]){
	int dev;
	unsigned int gdata = 0;

	dev = open(DEV_NAME, O_WRONLY);
	if(dev < 0){
		printf("Device open error : %s\n", DEV_NAME);
		exit(1);
	}

	write(dev, &gdata, 0);
	while(1){}

	close(dev);

	return 0;
}
