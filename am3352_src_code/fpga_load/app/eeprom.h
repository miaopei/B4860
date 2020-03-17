#ifndef _EEPROM_H_
#define _EEPROM_H_

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include "stdint.h"
#include "errno.h"
#include "fcntl.h"
#include "sys/types.h"
#include "sys/stat.h"


#define EEPROM_DEV	"/sys/bus/i2c/devices/2-0050/eeprom"

extern int Eeprom_open(const char *eeprom_dev);
extern void Eeprom_close(int fd);
extern int Eeprom_ReadBytes(int fd,long addr,char *buf,int len);
extern int Eeprom_WriteBytes(int fd,long addr,char *buf,int len);

#endif	
