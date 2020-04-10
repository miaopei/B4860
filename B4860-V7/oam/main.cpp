/*************************************************************************
	> File Name: main.cpp
	> Author: miaopei
	> Mail: miaopei@baicells.com 
	> Created Time: 2020年02月12日 星期三 11时16分50秒
 ************************************************************************/

#include <iostream>
//#include <cstdio>
//#include <cstring>

#include "oam_adapter.h"

using namespace uv;
using namespace std;

std::string serverIP;

void InitializeOamAdapterConnect(void *arg)
{
    EventLoop* loop = (EventLoop*)arg;

    uv::GlobalConfig::BufferModeStatus = uv::GlobalConfig::ListBuffer;
    
    SocketAddr addr(serverIP.c_str(), 30000, SocketAddr::Ipv4);
    OamAdapter OamAdapter(loop);

    OamAdapter.connectToServer(addr);

    loop->run();
}

void testThread(void *arg)
{
    EventLoop* loop = (EventLoop*)arg;

    std::this_thread::sleep_for(chrono::milliseconds(500)); // 延时 500ms
    std::string args;
    std::cout << "This is Test Thread" << std::endl;
    while(1)
    {
        std::cout << "OamAdapter > ";
        getline(cin, args);
        std::cout << args << std::endl;
        loop->runInThisLoop(TestFunc);
    }
}

int main(int argc, char* argv[])
{
    EventLoop* loop = new EventLoop();

    if(argc != 2)
    {
        fprintf(stdout, "usage: %s server_ip_address\neg.%s 192.168.1.1\n", argv[0], argv[0]);
        return 0;
    }
    serverIP = argv[1];

    uv_thread_t t1;
    uv_thread_t t2;
    
    uv_thread_create(&t1, InitializeOamAdapterConnect, loop);
    uv_thread_create(&t2, testThread, loop);

    uv_thread_join(&t1);
    uv_thread_join(&t2);

    return 0;
}

