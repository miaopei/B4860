#ifndef _GPMC_MPI_H_
#define _GPMC_MPI_H_
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

#define GPMC_MPI_DEV	"/dev/gpmc_mpi"

/*gpmc_MPI CONFIG IP REG*/
#define  GPMC_MPI_REG0	0X00
#define  GPMC_MPI_REG1	0X02
#define  GPMC_MPI_REG2	0X04
#define  GPMC_MPI_REG3	0X08
/*gpmc_MPI CONFIG VALUES*/
#define  GPMC_MPI_EN         0x00000001
/*gpmc_MPI CONFIG CTRL*/
#define GPMC_MPI_INIT	_IOW('g',0,int)
#define GPMC_MPI_WR	_IOW('g',1,int)
#define GPMC_MPI_RD 	_IOWR('g',2,int)

struct gpmc_mpi_data{
	unsigned int regaddress;
	unsigned int regvalue;
};

extern int gpmc_mpi_open(const char* gpmc_mpi_dev);
extern void gpmc_mpi_close(int fd);
extern unsigned int gpmc_mpi_write(int fd,unsigned int addr,unsigned int wdata);
extern unsigned int gpmc_mpi_read(int fd,unsigned int addr);
extern unsigned int gpmc_mpi_write_device(int fd,unsigned int module_addr, unsigned int reg_addr, unsigned int reg_wdata);
extern unsigned int gpmc_mpi_read_device(int fd,unsigned int module_addr, unsigned int reg_addr);
 
#endif


