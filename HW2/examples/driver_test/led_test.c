 /* GPIO LED Driver test application
   FILE : test_led.c
   AUTH : Huins */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define LED0_ON     0x00
#define LED0_OFF    0x01
#define LED1_ON     0x02
#define LED1_OFF    0x03
#define LED2_ON     0x04
#define LED2_OFF    0x05
#define LED3_ON     0x06
#define LED3_OFF    0x07
#define LED_ALL_ON  0x08
#define LED_ALL_OFF 0x09

int main(int argc, char **argv)
{
    int led_fd;
    int get_number;
    unsigned char val[] = {0x70, 0xB0, 0xD0, 0xE0, 0x00, 0xF0};
    if(argc != 2) {  // 실행 어규먼트를 받았는지 체크한다.
        printf("Usage : %s [Number]\n",argv[0]);
        return -1;
    }

    led_fd = open("/dev/led_driver", O_RDWR); // 디바이스를 오픈한다.
    if (led_fd<0){  // 만약 디바이스가 정상적으로 오픈되지 않으면 오류 처리후 종료한다.
        printf("LED Driver Open Failured!\n");
        return -1;
    }

    get_number=atoi(argv[1]); // 받은 인자를 숫자로 바꾼다.


    if(get_number>0||get_number<9)  // 숫자가 0~9 까지에 포함되는지 확인한다.
	write(led_fd, &val[get_number], sizeof(unsigned char));
    else
        printf("Invalid Value : 0 thru 9");  // 포함되지 않으면, 메시지를 출력한다.

    close(led_fd);  // 장치를 닫아준다.

    return 0; // 프로그램을 종료한다.
}
