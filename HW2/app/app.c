#include <linux/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[]){
	int ret;
	unsigned long n = 0;

	ret = open("/dev/dev_device", O_RDWR);

	write(ret, &n, sizeof(unsigned long));
	close(ret);
	
	return 0;
}
