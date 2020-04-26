/*************************************************************************
	> File Name: main.cpp
	> Author: miaopei
	> Mail: miaopei@baicells.com 
	> Created Time: 2020年02月12日 星期三 11时16分50秒
 ************************************************************************/

#include "bbu.h"

using namespace uv;

int main(int argc, char *argv[])
{
    EventLoop* loop = EventLoop::DefaultLoop();

    uv::GlobalConfig::BufferModeStatus = uv::GlobalConfig::ListBuffer;

	const char *interface_name = "enp1s0";
    char* pdata = NULL;
    size_t size = 32;
    pdata = (char*)malloc(size * sizeof(char));
    if(pdata == NULL)
    {
		LOG_PRINT(LogLevel::error, "malloc gateway memory error");
    }

    GetDeviceIP(interface_name, pdata, size);	
	LOG_PRINT(LogLevel::debug, "Device IP: %s", pdata);
	
    SocketAddr addr(pdata, 30000, SocketAddr::Ipv4);

    BBU bbu(loop);

    // 心跳超时
    //server.setTimeout(40);
    bbu.bindAndListen(addr);

    loop->run();
	
	free(pdata);
    pdata = NULL;
    return 0;
}

