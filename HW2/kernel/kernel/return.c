#include <linux/kernel.h>
#include <asm/uaccess.h>

struct mydata{
	int time;
	int num;
	char option[4];
};

asmlinkage long sys_returncall(struct mydata *data){
	struct mydata input;
	char position, value, time, num;
	long result = 0;

	// copy user space data to kernel space	
	copy_from_user(&input, data, sizeof(input));

	// distinguish user option (position & start value)
	if(input.option[0] != '0'){
		position = '1';
		value = input.option[0];
	}
	else if(input.option[1] != '0'){
		position = '2';
		value = input.option[1];
	}
	else if(input.option[2] != '0'){
		position = '3';
		value = input.option[2];
	}
	else{
		position = '4';
		value = input.option[3];
	}

	// store data to variable from user space
	time = input.time;
	num = input.num;

	// shift characters to proper place of 4 byte stream
	result += position<<24;
	result += value<<16;
	result += time<<8;
	result += num;
	// 4byte stream (position , value , time , num)
	
	return result;
}
