/*************************************************************************
	> File Name: main.cpp
	> Author: miaopei
	> Mail: miaopei@baicells.com 
	> Created Time: 2020年02月12日 星期三 11时16分50秒
 ************************************************************************/

#include <iostream>
#include <cstdio>
#include <cstring>

#include "bbu_server.h"

using namespace uv;
using namespace std;

string& ClearHeadTailSpace(string &str)
{
    if(str.empty())
    {
        return str;
    }

    str.erase(0, str.find_first_not_of(" "));
    str.erase(str.find_last_not_of(" "));
    return str;
}

int main(int argc, char *argv[])
{
    EventLoop* loop = EventLoop::DefaultLoop();

    //uv::GlobalConfig::BufferModeStatus = uv::GlobalConfig::CycleBuffer;

    SocketAddr addr("0.0.0.0", 30000, SocketAddr::Ipv4);

    Server server(loop);

    // 心跳超时
    //server.setTimeout(40);
    server.bindAndListen(addr);

#if 1
    // server 发消息到 client
    server.setMessageCallback(
        [&server](uv::TcpConnectionPtr conn, const char* data, ssize_t size)
    {
        std::cout << std::string(data, size) << '\n' << std::endl;
        string sendmsg = "BBU Server send msg all client.";
        server.SendMsg(sendmsg.c_str(), sendmsg.length());


    });
#endif
    
    loop->run();

    return 0;
}

