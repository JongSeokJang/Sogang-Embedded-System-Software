#include <linux/kernel.h>
#include <asm/uaccess.h>

struct my_data{
	int time;
	int num;
	char option[4];
}

asmlinkage int sys_returncall(struct my_data *block){
	struct my_data data;
	int time, num;
	char option[4];

	// copy user space data to kernel space
	copy_from_user(&data, block, sizeof(struct my_data));

	// store data to variable from user space
	time = data->time;
	num = data->num;
	option = data->option;

	printk("%d\n%d\n%s\n", time, num, option);

	return 21;
}
