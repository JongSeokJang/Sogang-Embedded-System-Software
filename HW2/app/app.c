#include <unistd.h>
#include <syscall.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define DEV_DEVICE "/dev/dev_driver"

struct input_data{
	int time;
	int num;
	char option[5];
};

int main(int argc, char *argv[]){
	struct input_data data;
	char *tmp;
	int i, dev, retval;

	// check correctness of parameter
	if(argc != 4){
		printf("Please input the correct parameter!\n");
		printf("Ex) ./app 10 20 0040\n");
		return -1;
	}

	// check correctness of time interval
	data.time = atoi(argv[1]);
	if(data.time < 0 || data.time > 100){
		printf("Warning! number between 0 ~ 100 only!\n");
		return -1;
	}

	// check correctness of number of chaning
	data.num = atoi(argv[2]);
	if(data.num < 0 || data.num > 100){
		printf("Warning! number between 0 ~ 100 only!\n");
		return -1;
	}

	// copy option data to data structure
	tmp = argv[3];
	for(i=0;i<4;i++)
		data.option[i] = tmp[i];
	data.option[i] = '\0';

	// get return value of given parameter
	long data_stream = syscall(366, &data);

	// device driver open
	dev = open(DEV_DEVICE, O_WRONLY);
	if(dev < 0){
		printf("Device open error : %s\n", DEV_DEVICE);
		exit(1);
	}

	// write input data to module
	retval = write(dev, &data_stream, 4);
	if(retval < 0){
		printf("Write Error!\n");
		return -1;
	}

	// close device driver
	close(dev);

	return 0;
}
