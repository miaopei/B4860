/*************************************************************************
	> File Name: main.cpp
	> Author: miaopei
	> Mail: miaopei@baicells.com 
	> Created Time: 2020年02月12日 星期三 11时16分50秒
 ************************************************************************/

#include <iostream>
#include <thread>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <cstdio>
#include <cstring>
#include <vector>

#include "oam.h"

using namespace uv;
using namespace std;

int main(int argc, char* argv[])
{
    EventLoop* loop = new EventLoop();

    //uv::GlobalConfig::BufferModeStatus = uv::GlobalConfig::CycleBuffer;

    SocketAddr addr("127.0.0.1", 30000, SocketAddr::Ipv4);
    Client client(loop);

    client.connectToServer(addr);

    loop->run();

    /* 实现数据接收线程和数据发送线程 */

    return 0;
}

