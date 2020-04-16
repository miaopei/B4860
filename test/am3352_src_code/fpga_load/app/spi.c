#include "spi.h"

int Spi_open(const char* spi_dev){
	int fd;
	int ret;
	unsigned char bits = 8;
	unsigned int  speed = 24000000;
	
	fd=open(spi_dev,O_RDWR);
	if(fd<0){
		printf("open spi device error\r\n");
		return -1;
	}
	/*bits per word*/
	ret=ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if(ret<0){
		printf("can't set bits per word\r\n");
	}
	ret=ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if(ret<0){
		printf("can't set bits per word\r\n");
	}
	/*max speed hz*/
	ret=ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if(ret<0){
		printf("can't set spi max speed hz\r\n");
	}
	ret=ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if(ret<0){
		printf("can't get spi max speed\r\n");
		return -1;
	}
	return fd;
}
void Spi_close(int fd){
	
	close(fd);
}
int Spi_WriteBytes_MSB(int fd,unsigned char* wdata,unsigned int length){
	int ret;
	unsigned char data[4];
	struct spi_ioc_transfer tr = {
		    .tx_buf = (unsigned long)data,
		    .len = 1,
		    .delay_usecs = 0,
		    .cs_change = 0,	
		    .bits_per_word = 8,
	};
	tr.tx_buf = (unsigned long) wdata;
	tr.len = length;
	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	if(ret < 1){
		printf("can't xfer spi message\r\n");
		return -1;
	}
	return 0;
}
int Spi_ReadByte_MSB(int fd,unsigned short readAddress){
	int ret;
	unsigned char data[4];
	unsigned char address[4];
	struct spi_ioc_transfer tr[2] = {
		{
		    .tx_buf = (unsigned long)address,
		    .len = 2,
		    .delay_usecs = 0,
		    .cs_change = 0,	
		    .bits_per_word = 8,
		},
		{
		    .rx_buf = (unsigned long)data,
		    .len = 1,
		    .delay_usecs = 0,
		    .cs_change = 0,
		    .bits_per_word = 8,
		},
	};	
	address[0] = ((readAddress >> 8) | 0x80) & 0xff;
	address[1] = readAddress & 0xff;
	ret = ioctl(fd, SPI_IOC_MESSAGE(2), tr);
	if(ret < 1){
		printf("can't xfer spi message\r\n");
		return -1;
	}
	ret = data[0];
	return ret;
}











