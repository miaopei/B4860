/*************************************************************************
	> File Name: Utilities.cpp
	> Author: miaopei
	> Mail: miaopei@baicells.com 
	> Created Time: 2020年02月12日 星期三 11时16分50秒
 ************************************************************************/
#ifndef UV_UTILITIES_H
#define UV_UTILITIES_H

#include <iostream>
#include <string.h>
#include <unistd.h>
#include <net/route.h>
#include <errno.h>
#include <vector>

#include "uv11.h"

using namespace std;

#define PATH_ROUTE      "/proc/net/route"

/*************************************************************************
 * @GetDeviceIP
 * get device interface address
 * 
 * interface_name - interface name
 * ip - store ip memory
 * size - memory size
 * 
 * Return: On Sucess, true
 *         On Failure, false
 ************************************************************************/
bool GetDeviceIP(const char* interface_name, char* ip, size_t size);

/*************************************************************************
 * @GetDeviceMAC
 * get device interface mac
 * 
 * interface_name - interface name
 * mac - store mac memory
 * size - memory size
 * 
 * Return: On Sucess, true
 *         On Failure, false
 ************************************************************************/
bool GetDeviceMAC(const char* interface_name, char* mac, size_t size);

/*************************************************************************
 * @GetDeviceGateWay
 * get device interface gateway
 * 
 * interface_name - interface name
 * gateway - store gateway memory
 * size - memory size
 * 
 * Return: On Sucess, true
 *         On Failure, false
 ************************************************************************/
bool GetDeviceGateWay(const char* interface_name, char* gateway, size_t size);


std::vector<std::string> DataSplit(const std::string& in, const std::string& delim);



#endif
