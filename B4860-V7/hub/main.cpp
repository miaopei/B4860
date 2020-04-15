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
    
    if(argc != 2)
    {
        fprintf(stdout, "usage: %s server_ip_address\neg.%s 192.168.1.1\n", argv[0], argv[0]);
        return 0;
    }
    serverIP = argv[1];

	SocketAddr addr(serverIP.c_str(), 30000, SocketAddr::Ipv4);
    HUB hub(loop);

#if 1
	ftplib *ftp = new ftplib();
	ftp->Connect("ftp.gwdg.de:21");
	ftp->Login("anonymous", "");
	ftp->Dir(NULL, "/pub/linux/apache");
	ftp->Quit();
#endif
#if 0
	char data[80];
	hub.read_file("./test.txt", data, sizeof(data));
	std::cout << "data=" << data << std::endl;

	hub.write_file("./test.txt", "test");
#endif   

    hub.bbu_addr = serverIP;

    hub.connectToServer(addr);

    loop->run();

    return 0;
}

