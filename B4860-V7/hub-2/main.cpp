/*************************************************************************
	> File Name: main.cpp
	> Author: miaopei
	> Mail: miaopei@baicells.com 
	> Created Time: 2020年02月12日 星期三 11时16分50秒
 ************************************************************************/

#include <iostream>
#include <cstdio>
#include <cstring>

#include "hub.h"

using namespace uv;
using namespace std;

std::string serverIP;

int main(int argc, char* argv[])
{
    EventLoop* loop = new EventLoop();

    uv::GlobalConfig::BufferModeStatus = uv::GlobalConfig::ListBuffer; 
    
    if(argc != 2)
    {
        fprintf(stdout, "usage: %s server_ip_address\neg.%s 192.168.1.1\n", argv[0], argv[0]);
        return 0;
    }
    serverIP = argv[1];

    SocketAddr addr(serverIP.c_str(), 30000, SocketAddr::Ipv4);
    HUB hub(loop);

    hub.connectToServer(addr);

    loop->run();

    return 0;
}

