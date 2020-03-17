#ifndef _SPI_H_
#define _SPI_H_

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

#define SPI_DEV		"/dev/spidev0.0"
extern	int Spi_open(const char* spi_dev);
extern	void Spi_close(int fd);
extern	int Spi_WriteBytes_MSB(int fd,unsigned char* wdata,unsigned int length);
extern	int Spi_ReadByte_MSB(int fd,unsigned short readAddress);

#endif				/* _SPI_H */
