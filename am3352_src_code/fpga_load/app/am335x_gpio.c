#include "am335x_gpio.h"

int am335x_gpio_open(const char* am335x_gpio_dev){
	int fd;
	
	fd = open(am335x_gpio_dev,O_RDWR);	
	if(fd<0){
		printf("open am335x_gpio device error\r\n");
		return -1;
	}
	return fd;	
}
void am335x_gpio_close(int fd){
	
	close(fd);
}
void am335x_gpio_Set_Output(int fd,unsigned int gpio_num,unsigned long level){
	struct am335x_gpio_data am335x_gpio_data;
	
	am335x_gpio_data.gpio_num = gpio_num;
	am335x_gpio_data.gpio_output_level = level;
	ioctl(fd,GPIO_SET_OUTPUT,(unsigned long *)&am335x_gpio_data);
}
unsigned int am335x_gpio_Get_Input(int fd,unsigned int gpio_num){
	struct am335x_gpio_data am335x_gpio_data;
	unsigned int value;
	am335x_gpio_data.gpio_num = gpio_num;
	ioctl(fd,GPIO_GET_INPUT,(unsigned long *)&am335x_gpio_data);
	value = am335x_gpio_data.gpio_input_value;
	
	return value;
}
