/*************************************************************************
	> File Name: hub_client.cpp
	> Author: miaopei
	> Mail: miaopei@baicells.com 
	> Created Time: 2020年02月12日 星期三 11时16分50秒
 ************************************************************************/

#include <iostream>
#include <string.h>

#include "hub_client.h"

using namespace uv;
using namespace std;

Client::Client(uv::EventLoop* loop)
    :TcpClient(loop),
    sockAddr(nullptr)
{
    setConnectStatusCallback(std::bind(&Client::onConnect, this, std::placeholders::_1));
    setMessageCallback(std::bind(&Client::RecvMesg, this, std::placeholders::_1, std::placeholders::_2));
}

void Client::connectToServer(uv::SocketAddr& addr)
{
    sockAddr = std::make_shared<uv::SocketAddr>(addr);
    connect(addr);
}

void Client::reConnect()
{
    uv::Timer* timer = new uv::Timer(loop_, 500, 0, [this](uv::Timer* ptr)
    {
        connect(*(sockAddr.get()));
        ptr->close([](uv::Timer* ptr)
        {
            delete ptr;    
        });
    });
    timer->start();
}

void Client::sendTestMessage()
{
    // packet 封装
    // void packet(uv::PacketIR::PackHead head, std::string& data, size_t size);
    // return data(消息头+消息体)

    // C 风格 packet
#if 0
    const char *data = "timeDelay=100&upDelay=20&downDelay=66";

    std::cout << "data_size=" << strlen(data) << std::endl;

    uv::PacketIR::PackHead packet;
    memset(packet.data, 0, sizeof(packet.data));
    strcpy(packet.t_type, "1");
    strcpy(packet.t_msgID, "1001");
    strcpy(packet.t_state, "1");
    strcpy(packet.t_RRUID, "4");
    strcpy(packet.t_PORT, "2");

    char *send_buf = new char[sizeof(packet) + strlen(data) + 1];
    memcpy(send_buf, &packet, sizeof(packet));
    strcat(send_buf, data);

    std::cout << "send_buf=" << send_buf << std::endl;
    std::cout << "send_buf_size=" << strlen(send_buf) << std::endl;

    write(send_buf, (int)strlen(send_buf));

    delete[] send_buf;
#endif

    // C++ 风格 Packet
#if 1
    std::string data = "timeDelay=100&upDelay=20&downDelay=66";
    std::cout << "data_size=" << data.length() << std::endl;
    
    uv::PacketIR::PackHead packet;
    strcpy(packet.t_type, "1");
    strcpy(packet.t_msgID, "1001");
    strcpy(packet.t_state, "1");
    strcpy(packet.t_RRUID, "4");
    strcpy(packet.t_PORT, "2");

    char s[sizeof(packet)];
    memset(s, 0, sizeof(s));
    memcpy(s, &packet, sizeof(packet));
    std::string send_buf(s);
    send_buf += data;

    std::cout << "send_data=" << send_buf << std::endl;
    std::cout << "send_buf_size=" << send_buf.length() << std::endl;

    write(send_buf.c_str(), (int)send_buf.length());
#endif 

#if 0
    if(uv::GlobalConfig::BufferModeStatus == uv::GlobalConfig::NoBuffer)
    {
        //write(data, (int)sizeof(data));
        write(send_buf, (int)sizeof(send_buf));
    } else {
        uv::Packet packetxx;
        //packetxx.pack(data.c_str(), data.length());
        //write(packetxx.Buffer().c_str(), packetxx.PacketSize());
    }
#endif
}

void Client::onConnect(ConnectStatus status)
{
    if(status != ConnectStatus::OnConnectSuccess)
    {
        reConnect();
    } else {
        sendTestMessage();
    }
}

void Client::newMessage(const char* buf, ssize_t size)
{
    std::cout << "HUB newMessage: " << std::string(buf, size) << std::endl;

#if 0
    if(uv::GlobalConfig::BufferModeStatus == uv::GlobalConfig::NoBuffer)
    {
        write(buf, (unsigned int)size);
    } else {
        auto packetbuf = getCurrentBuf();
        if(nullptr != packetbuf)
        {
            packetbuf->append(buf, static_cast<int>(size));
            uv::Packet packet;
            while(0 == packetbuf->readPacket(packet))
            {
                write(packet.Buffer().c_str(), (unsigned)packet.PacketSize(), nullptr);
            }
        }
    }
#endif
}

void Client::RecvMesg(const char* buf, ssize_t size)
{
    std::cout << "HUB Recv Msg: " << std::string(buf, size) << std::endl;
    //string msg = "ttt";
    //write(msg.c_str(), msg.length());
}

void Client::SendMesg(const char* buf, ssize_t size)
{
    std::cout << "Client::SendMesg" << std::endl;
    if(uv::GlobalConfig::BufferModeStatus == uv::GlobalConfig::NoBuffer)
    {
        writeInLoop(buf, (unsigned int)size, nullptr);
    } else {
        auto packetbuf = getCurrentBuf();
        if(nullptr != packetbuf)
        {
            packetbuf->append(buf, static_cast<int>(size));
            uv::Packet packet;
            while(0 == packetbuf->readPacket(packet))
            {
                write(packet.Buffer().c_str(), (unsigned)packet.PacketSize(), nullptr);
            }
        }
    }

}

