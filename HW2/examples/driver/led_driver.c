/* LED Ioremap Control
FILE : led_driver.c */

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <mach/gpio.h>
#include <mach/regs-gpio.h>
#include <plat/gpio-cfg.h>
#include <linux/platform_device.h>
#include <linux/delay.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/kernel.h>
#include <linux/ioport.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/version.h>

#define LED_MAJOR 240          // 디바이스 드라이버의 주번호
#define LED_MINOR 0             // 디바이스 드라이버의 부번호
#define LED_NAME "led_driver"       // 디바이스 드라이버의 이름
#define LED_GPBCON 0x11400040   // GPBCON 레지스터의 물리주소
#define LED_GPBDAT 0x11400044   // GPBDAT 레지스터의 물리주소

static char *buffer = NULL;

int led_open(struct inode *, struct file *);
int led_release(struct inode *, struct file *);
ssize_t led_write(struct file *, const char *, size_t, loff_t *);

static int led_usage = 0;
static unsigned char *led_data;
static unsigned int *led_ctrl;

static struct file_operations led_fops =   // 파일 오퍼레이션 구조체
{
    .open       = led_open,
    .write      = led_write,
    .release    = led_release,
};

// 드라이버가 열릴 때 사수행되는 함수
int led_open(struct inode *minode, struct file *mfile)
{
    if(led_usage != 0)
        return -EBUSY;
    led_usage = 1;
    return 0;
}

// 드라이버가 클로즈 될 때 수행되는 함수
int led_release(struct inode *minode, struct file *mfile)
{
    led_usage = 0;
    return 0;
}

// 드라이버에 값을 쓰기 위한 함수
ssize_t led_write(struct file *inode, const char *gdata, size_t length, loff_t *off_what)  
{
    const char *tmp = gdata;
    unsigned short led_buff=0;

    // 사용자 공간에서 데이터를 커널공간으로 복사
    if (copy_from_user(&led_buff, tmp, length)) // 2byte receive from application
        return -EFAULT;

    printk("DATA : %d\n",led_buff);
    outb (led_buff, (unsigned int)led_data);  // 넘어온 데이터를 기록 -> LED 동작

    return length;
}

int __init led_init(void)  // 드라이버가 초기화 될 때 동작되는 함수
{
    int result;
    unsigned int get_ctrl_io=0;
    struct class *led_dev_class=NULL;
    struct device *led_dev=NULL;

    // 디바이스 드라이버 등록
    result = register_chrdev(LED_MAJOR, LED_NAME, &led_fops);
    if(result <0) {  // 실패하면 오류처리
        printk(KERN_WARNING"Can't get any major!\n");
        return result;
    }

    led_data = ioremap(LED_GPBDAT, 0x01);  // LED 제어 데이터 메모리 매핑
    if(led_data==NULL) // 매핑에 실패하면 에러처리
    {   
        printk("ioremap failed!\n");
        return -1;
    }

    led_ctrl = ioremap(LED_GPBCON, 0x04);  // 물리주소를 매핑
    if(led_ctrl==NULL)  // 물리주소 매핑에서 오류가 발생하면 에러 처리
    {
        printk("ioremap failed!\n");
        return -1;
    } else {   // 물리주소 매핑을 성공하면 해당 레지스터 값을 변경
        get_ctrl_io=inl((unsigned int)led_ctrl);  // GPBCON 데이터를 받음
        led_dev = device_create(led_dev_class,NULL,MKDEV(LED_MAJOR,0),NULL,LED_NAME);
        buffer = (char*)kmalloc(1024, GFP_KERNEL);
 
        if(buffer != NULL)
            memset(buffer, 0, 1024);

        get_ctrl_io|=(0x11110000);  // GPB Pin 설정 레지스트 상위 4바이트를 출력으로 설정
        outl(get_ctrl_io,(unsigned int)led_ctrl);  // 설정된 값을 레지스터에 적용  
    }

    printk("init module, /dev/led_driver major : %d\n", LED_MAJOR);
    outb(0xF0, (unsigned int)led_data);

    return 0;
}

void __exit led_exit(void)
{
    outb(0xF0, (unsigned int)led_data);
    iounmap(led_data);
    iounmap(led_ctrl);
    unregister_chrdev(LED_MAJOR, LED_NAME);
    printk("Removed LED module\n");

}

module_init(led_init);
module_exit(led_exit);

MODULE_LICENSE ("GPL");
MODULE_AUTHOR ("Huins HSH");
