/*************************************************************************
	> File Name: main.cpp
	> Author: miaopei
	> Mail: miaopei@baicells.com 
	> Created Time: 2020年02月12日 星期三 11时16分50秒
 ************************************************************************/

#include "bbu.h"

using namespace uv;

std::string serverIP;

int main(int argc, char *argv[])
{
    EventLoop* loop = EventLoop::DefaultLoop();

    uv::GlobalConfig::BufferModeStatus = uv::GlobalConfig::ListBuffer;
    
    if(argc != 2)
    {
        fprintf(stdout, "usage: %s server_ip_address\neg.%s 192.168.1.1\n", argv[0], argv[0]);
        return 0;
    }
    serverIP = argv[1];

#if 0
    const char *interface_name = "enp0s31f6";
    char* pdata = NULL;
    size_t size = 32;
    pdata = (char*)malloc(size * sizeof(char));
    if(pdata == NULL)
    {
        std::cout << "Error: malloc gateway memory error" << std::endl;
        return -1;
    }

    GetDeviceIP(interface_name, pdata, size);
    std::cout << "Device IP: " << pdata << std::endl;
    memset(pdata , 0, size);

    GetDeviceMAC(interface_name, pdata, size);
    std::cout << "Device MAC: " << pdata << std::endl;
    memset(pdata , 0, size);
    
    GetDeviceGateWay(interface_name, pdata, size);
    std::cout << "Device GateWay: " << pdata << std::endl;
    memset(pdata , 0, size);
    
    free(pdata);
    pdata = NULL;
#endif

    SocketAddr addr(serverIP.c_str(), 30000, SocketAddr::Ipv4);

    BBU bbu(loop);

    // 心跳超时
    //server.setTimeout(40);
    bbu.bindAndListen(addr);

    loop->run();

    return 0;
}

