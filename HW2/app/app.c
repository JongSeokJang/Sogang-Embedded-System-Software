#include <unistd.h>
#include <syscall.h>
#include <stdio.h>
#include <stdlib.h>

struct input_data{
	int time;
	int num;
	char option[5];
};

int main(int argc, char *argv[]){
	struct input_data data;
	char *tmp;
	int i;

	if(argc != 4){
		printf("Wrong parameter!\n");
		return -1;
	}

	data.time = atoi(argv[1]);
	data.num = atoi(argv[2]);
	tmp = argv[3];

	for(i=0;i<4;i++)
		data.option[i] = tmp[i];
	data.option[i] = '\0';

	long ret = syscall(366, &data);

	long temp = ret>>24;
	printf("Position is %c\n", temp);

	temp = ret<<8;
	temp = temp>>24;
	printf("Value is %c\n", temp);

	temp = ret<<16;
	temp = temp>>24;
	printf("Time is %d\n", temp);

	temp = ret<<24;
	temp = temp>>24;
	printf("Number is %d\n", temp);

	return 0;
}
