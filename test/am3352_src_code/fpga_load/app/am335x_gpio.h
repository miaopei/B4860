#ifndef _AM335X_GPIO_H_
#define _AM335X_GPIO_H_
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <linux/ip.h>
#include <linux/fs.h>

#include <fcntl.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <pthread.h>
#include <termios.h>
#include <semaphore.h>
#include <limits.h>
#include <linux/i2c.h>  
#include <linux/i2c-dev.h>
#include <linux/spi/spidev.h>

#define AM335X_GPIO_DEV	"/dev/am335x_gpio"

/*PS GPIO CONFIG CTRL*/
#define GPIO_SET_OUTPUT	_IOWR('w',0,int)
#define GPIO_GET_INPUT	_IOWR('w',1,int)

/*AM335X_GPIO1_8*/
#define FPGA_CONFG_DONE	0
/*AM335X_GPIO1_9*/
#define FPGA_CONFG_PRORAM_N	1
/*AM335X_GPIO2_24*/
#define AD9528_RESETB	2
/*AM335X_GPIO2_22*/
#define AD9528_REF_SEL	3
#define AM335X_GPIO3_18	4
#define AM335X_GPIO3_19	5
#define AM335X_GPIO3_20	6
#define	AM335X_GPIO3_21	7

struct am335x_gpio_data{
	unsigned int gpio_num;
	unsigned int gpio_output_level;
	unsigned int gpio_input_value;
	unsigned int rsv;
};

#define HIGH		1
#define LOW		0

extern int am335x_gpio_open(const char* am335x_gpio_dev);
extern void am335x_gpio_close(int fd);
extern void am335x_gpio_Set_Output(int fd,unsigned int gpio_num,unsigned long level);
extern unsigned int am335x_gpio_Get_Input(int fd,unsigned int gpio_num); 
#endif
