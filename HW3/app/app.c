#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#define DEV_NAME "/dev/stopwatch"

int main(int argc, char *argv[]){
	int dev, ret;
	unsigned int gdata = 0;

	dev = open(DEV_NAME, O_WRONLY);
	if(dev < 0){
		printf("Device open error : %s\n", DEV_NAME);
		exit(1);
	}

	ret = write(dev, &gdata, 0);
	while(ret){
		printf("%d\n", ret);
	}

	printf("finished?!\n");

	close(dev);

	return 0;
}
