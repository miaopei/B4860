/*************************************************************************
	> File Name: main.cpp
	> Author: miaopei
	> Mail: miaopei@baicells.com 
	> Created Time: 2020年02月12日 星期三 11时16分50秒
 ************************************************************************/

#include "bbu.h"

using namespace uv;

std::string serverIP;

void uv_interface_get_address(const char* interface_name)
{
    char buf[512];
    uv_interface_address_t *info;
    int count, i;

    uv_interface_addresses(&info, &count);
    i = count;

    while(i--)
    {
        uv_interface_address_t interface = info[i];
        
        if(strcmp(interface.name, interface_name) == 0)
        {
            if(interface.address.address4.sin_family == AF_INET)
            {
                uv_ip4_name(&interface.address.address4, buf, sizeof(buf));
                std::cout << "IPv4 address: " << buf << std::endl;
            } else {
                continue;
            } 
        } else {
            continue;
        }
    }
    uv_free_interface_addresses(info, count);
}

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

    //const char *interface_name = "enp0s31f6";
    //uv_interface_get_address(interface_name);

    SocketAddr addr(serverIP.c_str(), 30000, SocketAddr::Ipv4);

    BBU bbu(loop);

    // 心跳超时
    //server.setTimeout(40);
    bbu.bindAndListen(addr);

    loop->run();

    return 0;
}

