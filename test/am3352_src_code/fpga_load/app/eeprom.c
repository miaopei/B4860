#include "eeprom.h"

int Eeprom_open(const char *eeprom_dev){
	int fd;

	fd = open(eeprom_dev, O_RDWR);
	if (fd < 0)
	{
		printf("open eeprom dev error\r\n");
		return -1;
	}
	return fd;
}
void Eeprom_close(int fd){
	
	close(fd);
}
int Eeprom_ReadBytes(int fd,long addr,char *buf,int len){
	int size;

	lseek(fd,addr,SEEK_SET);/*set eeprom address*/
	if((size=read(fd,buf,len))<0){
		printf("read error\r\n");
		return -1;
	}
	return size;
}
int Eeprom_WriteBytes(int fd,long addr,char *buf,int len){
	int size;

	lseek(fd,addr,SEEK_SET);/*set eeprom address*/
	if((size=write(fd,buf,len))<0){
		printf("write error\r\n");
		return -1;
	}
	return size;
}
