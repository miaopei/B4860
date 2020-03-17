#include <time.h>
#include <sys/time.h>
#include "am335x_gpio.h"
#include "gpmc_mpi.h"
#include "eeprom.h"
#include "uart.h"
#include "spi.h"
#include "fpga_load_bit.h"
int main(int argc,char *argv[]){
	int gpio_fd,spi_fd;
	FILE *pFile=NULL;
	char *pBUF=NULL;
	int file_size=0;

	gpio_fd = am335x_gpio_open(AM335X_GPIO_DEV);
	if(gpio_fd < 0){
		return -1;
	}
	spi_fd = Spi_open(SPI_DEV);
	if(spi_fd < 0){
		return -1;
	}
	pFile = fopen("/mnt/jinsha_fpga_top.bit","r");
	if(pFile == NULL){
		printf("Open file %s fail\r\n",pFile);
	}
	fseek(pFile,0,SEEK_END);
	file_size = ftell(pFile);
	pBUF = (char *)malloc(file_size);
	rewind(pFile);
	fread(pBUF,1,file_size,pFile);
	fclose(pFile);
	printf("read file fpga.bit size %d bytes\r\n",file_size);

	fpga_load_bit(spi_fd, gpio_fd, pBUF, file_size);	

	am335x_gpio_close(gpio_fd);
	Spi_close(spi_fd);

	return 0;
}
