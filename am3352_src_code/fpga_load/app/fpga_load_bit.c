/***********************************************************************/

#include "fpga_load_bit.h"

int fpga_load_bit(int spi_fd, int gpio_fd, char *Bitbuf, int size){
	int i = 0,j=0,k=0;
	unsigned char chk=0xff;
	unsigned int tmp;
	unsigned int level=0;

	am335x_gpio_Set_Output(gpio_fd,FPGA_CONFG_PRORAM_N,LOW);
	usleep(500000);
	am335x_gpio_Set_Output(gpio_fd,FPGA_CONFG_PRORAM_N,HIGH);
	usleep(500000);
	j = size / 1024;
	k = size % 1024;
	for(i=0;i<j;i++){
		tmp = Spi_WriteBytes_MSB(spi_fd,(Bitbuf+i*1024),1024);
	}
	if(k > 0){
		tmp = Spi_WriteBytes_MSB(spi_fd,(Bitbuf+j*1024),k);
	}
	if(tmp < 0){
		printf("write fpga bit fail\r\n");
	}
	usleep(500000);
	while((level = am335x_gpio_Get_Input(gpio_fd,FPGA_CONFG_DONE))==0){
		//sleep(1);
		printf("config fpga bit stream fail\r\n");
		return -1;
	}
	tmp = Spi_WriteBytes_MSB(spi_fd,&chk,1);
	if(level == 0){
		printf("config fpga bit stream fail\r\n");
	}
	else{
		printf("config fpga bit stream success\r\n");
	}
	return 0;
}
