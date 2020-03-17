#ifndef _FPGA_LOAD_BIT_H_
#define _FPGA_LOAD_BIT_H_
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

#include <time.h>
#include <sys/time.h>
#include "am335x_gpio.h"
#include "gpmc_mpi.h"
#include "eeprom.h"
#include "uart.h"
#include "spi.h"

extern int fpga_load_bit(int spi_fd, int gpio_fd, char *Bituf, int size);

#endif
