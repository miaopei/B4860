/*************************************************************************
	> File Name: Utilities.cpp
	> Author: miaopei
	> Mail: miaopei@baicells.com 
	> Created Time: 2020年02月12日 星期三 11时16分50秒
 ************************************************************************/
#ifndef UV_UTILITIES_H
#define UV_UTILITIES_H

#include <iostream>
#include <ifaddrs.h>  
#include <arpa/inet.h>

#include <string.h>
#include <errno.h>
#include <netinet/in.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>


#include "uv11.h"

/*************************************************************************
 * @uv_interface_get_address
 * get device interface address
 * 
 * interface_name - interface name
 * 
 * Return: On Sucess, interface address
 *         On Failure, NULL
 ************************************************************************/

int getSubnetMask();
void DispNetInfo(const char* szDevName);

#endif
