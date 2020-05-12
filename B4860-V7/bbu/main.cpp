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

	const char *interface_name = "enp0s31f6";
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
    //bbu.setTimeout(40);
#if 0
    uv::Timer* pTimer = new uv::Timer(loop, 500, 0, [](uv::Timer* handle)
    {
        LOG_PRINT(LogLevel::debug, "timer callback run onice.")
        handle->close([](uv::Timer* ptr)
        {
            delete ptr;    
        });
    });
    pTimer->start();
#endif
#if 0
    uv::Timer timer(loop, 1000, 1000,                                                                  
        [](Timer*)
    {
        LOG_PRINT(LogLevel::debug, "timer callback test...");
    });
    timer.start();
#endif

    bbu.bindAndListen(addr);

    loop->run();
	
	free(pdata);
    pdata = NULL;
    return 0;
}

