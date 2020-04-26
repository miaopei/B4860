/*************************************************************************
	> File Name: main.cpp
	> Author: miaopei
	> Mail: miaopei@baicells.com 
	> Created Time: 2020年02月12日 星期三 11时16分50秒
 ************************************************************************/

#include <iostream>
#include <cstdio>
#include <cstring>

#include <assert.h>
#include <fcntl.h>
#include <unistd.h>


#include "hub.h"

using namespace uv;
using namespace std;

std::string serverIP;

int main(int argc, char* argv[])
{
    EventLoop* loop = new EventLoop();

    uv::GlobalConfig::BufferModeStatus = uv::GlobalConfig::ListBuffer;

	const char *interface_name = "enp1s0";
	char* pdata = NULL;
	size_t size = 32;
	pdata = (char*)malloc(size * sizeof(char));
	if(pdata == NULL)
	{		
		LOG_PRINT(LogLevel::error, "malloc gateway memory error");
	}
	
#if 0
	GetDeviceGateWay(interface_name, pdata, size);
	LOG_PRINT(LogLevel::debug, "Device GateWay: %s", pdata);
#endif

#if 1
    GetDeviceIP(interface_name, pdata, size);	
	LOG_PRINT(LogLevel::debug, "Device IP: %s", pdata);
#endif

	SocketAddr addr(pdata, 30000, SocketAddr::Ipv4);
    HUB hub(loop);

#if 0
	char data[80];
	hub.read_file("./test.txt", data, sizeof(data));
	std::cout << "data=" << data << std::endl;
	hub.write_file("./test.txt", "test");
#endif   

    hub.bbu_addr = pdata;

    hub.connectToServer(addr);

    loop->run();
	
    free(pdata);
    pdata = NULL;

    return 0;
}

