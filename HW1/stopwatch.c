#include <sys/mman.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define IO_GPL_BASE_ADDR 0x11000000
#define FND_GPL2CON 0x0100
#define FND_GPL2DAT 0x0104

#define IO_GPE_BASE_ADDR 0x11400000
#define FND_GPE3CON 0x00140
#define FND_GPE3DAT 0x00144

int main(void){
	int fd, ttime, i;
	void *gpl_addr, *gpe_addr;
	unsigned long *gpe_con = 0;
	unsigned long *gpe_dat = 0;
	unsigned long *gpl_con = 0;
	unsigned long *gpl_dat = 0;
	time_t start_time, end_time;
	unsigned long fnd_number[10] = {0x03, 0x9F, 0x25, 0x0D, 0x99, 0x49, 0xC1, 0x1F, 0x01, 0x09};

	fd = open("/dev/mem", O_RDWR|O_SYNC);
	gpl_addr = (unsigned long *)mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, fd, IO_GPL_BASE_ADDR);
	gpl_con = (unsigned long *)(gpl_addr + FND_GPL2CON);
	gpl_dat = (unsigned long *)(gpl_addr + FND_GPL2DAT);
	gpe_addr = (unsigned long *)mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, fd, IO_GPE_BASE_ADDR);
	gpe_con = (unsigned long *)(gpe_addr + FND_GPE3CON);
	gpe_dat = (unsigned long *)(gpe_addr + FND_GPE3DAT);

	ttime = 0;
	*gpe_dat = 0x96;
	*gpl_dat = 0x02;

	while(1){
		ttime++;
		time(&start_time);
		end_time = 0;

		while(difftime(end_time, start_time) <= 0.99999){
			for(i=0;i<500;i++){
				*gpe_dat = 0x02;
				*gpl_dat = fnd_number[ttime/60/10];
			}

			for(i=0;i<500;i++){
				*gpe_dat = 0x04;
				*gpl_dat = fnd_number[ttime/60%10]-0x01;
			}

			for(i=0;i<500;i++){
				*gpe_dat = 0x10;
				*gpl_dat = fnd_number[ttime%60/10];
			}

			for(i=0;i<500;i++){
				*gpe_dat = 0x80;
				*gpl_dat = fnd_number[ttime%60%10];
			}

			time(&end_time);
		}
	}

	return 0; 
}
