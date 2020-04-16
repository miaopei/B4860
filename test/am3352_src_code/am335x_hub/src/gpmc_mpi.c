#include "gpmc_mpi.h"

/*
function name	: gpmc_mpi_open
parameters	: gpmc_mpi_dev
return		: fd
function	: open device file
*/
int gpmc_mpi_open(const char* gpmc_mpi_dev){
	int fd;

	fd = open(gpmc_mpi_dev, O_RDWR);
	if (fd < 0)
	{
		printf("open gpmc_mpi dev error\r\n");
		return -1;
	}
	return fd;
}
/*
function name	: gpmc_mpi_close
parameters	: fd
return		: void
function	: close device file
*/
void gpmc_mpi_close(int fd){
	
	close(fd);
}
/*
function name	: gpmc_mpi_write
parameters	: fd,addr,wdata
return		: 0
function	: write config reg data
*/
unsigned int gpmc_mpi_write(int fd,unsigned int addr,unsigned int wdata){
	struct gpmc_mpi_data gpmc_mpi_data;
	
	gpmc_mpi_data.regaddress = addr;
	gpmc_mpi_data.regvalue = wdata;
	ioctl(fd,GPMC_MPI_WR,(unsigned long *)&gpmc_mpi_data);

	return 0;
}
/*
function name	: gpmc_mpi_read
parameters	: fd,addr
return		: rdata
function	: read config reg data
*/
unsigned int gpmc_mpi_read(int fd,unsigned int addr){
	struct gpmc_mpi_data gpmc_mpi_data;
	unsigned int rdata;

	gpmc_mpi_data.regaddress = addr;
	gpmc_mpi_data.regvalue = rdata;
	ioctl(fd,GPMC_MPI_RD,(unsigned long *)&gpmc_mpi_data);
	rdata = gpmc_mpi_data.regvalue;

	return rdata;	
}
/*
function name	: gpmc_mpi_write_device
parameters	: fd,module_addr,reg_addr,reg_wdata
return		: 0
function	: write config reg data
*/
unsigned int gpmc_mpi_write_device(int fd,unsigned int module_addr, unsigned int reg_addr, unsigned int reg_wdata)
{
	struct gpmc_mpi_data gpmc_mpi_data;
	unsigned int mpi_write_addr;

	mpi_write_addr = reg_addr;
	gpmc_mpi_data.regaddress = GPMC_MPI_REG1;
	gpmc_mpi_data.regvalue = mpi_write_addr;
	ioctl(fd,GPMC_MPI_WR,(unsigned long *)&gpmc_mpi_data);
	usleep(200);
	mpi_write_addr = module_addr;
	gpmc_mpi_data.regaddress = GPMC_MPI_REG2;
	gpmc_mpi_data.regvalue = mpi_write_addr;
	ioctl(fd,GPMC_MPI_WR,(unsigned long *)&gpmc_mpi_data);
	usleep(200);
	gpmc_mpi_data.regaddress = GPMC_MPI_REG3;
	gpmc_mpi_data.regvalue = reg_wdata;
	ioctl(fd,GPMC_MPI_WR,(unsigned long *)&gpmc_mpi_data);
	usleep(200);
	gpmc_mpi_data.regaddress = GPMC_MPI_REG0;
	gpmc_mpi_data.regvalue = 0x00000000;
	ioctl(fd,GPMC_MPI_WR,(unsigned long *)&gpmc_mpi_data);
	usleep(200);
	gpmc_mpi_data.regaddress = GPMC_MPI_REG0;
	gpmc_mpi_data.regvalue = 0x00000003;
	ioctl(fd,GPMC_MPI_WR,(unsigned long *)&gpmc_mpi_data);
	usleep(200);
	return 0;
}
/*
function name	: gpmc_mpi_read_device
parameters	: fd,module_addr,reg_addr
return		: mpi_read_data
function	: read config reg data
*/
unsigned int gpmc_mpi_read_device(int fd,unsigned int module_addr, unsigned int reg_addr)
{
	struct gpmc_mpi_data gpmc_mpi_data;
	unsigned int mpi_read_addr;
	unsigned int mpi_read_data;

	mpi_read_addr = reg_addr;
	gpmc_mpi_data.regaddress = GPMC_MPI_REG1;
	gpmc_mpi_data.regvalue = mpi_read_addr;
	ioctl(fd,GPMC_MPI_WR,(unsigned long *)&gpmc_mpi_data);
	usleep(200);
	mpi_read_addr = module_addr;
	gpmc_mpi_data.regaddress = GPMC_MPI_REG2;
	gpmc_mpi_data.regvalue = mpi_read_addr;
	ioctl(fd,GPMC_MPI_WR,(unsigned long *)&gpmc_mpi_data);
	usleep(200);
	gpmc_mpi_data.regaddress = GPMC_MPI_REG0;
	gpmc_mpi_data.regvalue = 0x00000000;
	ioctl(fd,GPMC_MPI_WR,(unsigned long *)&gpmc_mpi_data);
	usleep(200);
	gpmc_mpi_data.regaddress = GPMC_MPI_REG0;
	gpmc_mpi_data.regvalue = 0x00000002;
	ioctl(fd,GPMC_MPI_WR,(unsigned long *)&gpmc_mpi_data);
	usleep(500);
	gpmc_mpi_data.regaddress = GPMC_MPI_REG4;
	gpmc_mpi_data.regvalue = mpi_read_data;
	ioctl(fd,GPMC_MPI_RD,(unsigned long *)&gpmc_mpi_data);
	mpi_read_data = gpmc_mpi_data.regvalue;

    	/*printf("The mpi_read_addr 0x%04x is 0x%04x\r\n",mpi_read_addr,mpi_read_data);*/
    	return mpi_read_data;
}


