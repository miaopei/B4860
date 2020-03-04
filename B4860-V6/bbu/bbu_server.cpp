/*************************************************************************
	> File Name: bbu_server.cpp
	> Author: miaopei
	> Mail: miaopei@baicells.com 
	> Created Time: 2020年02月12日 星期三 11时15分05秒
 ************************************************************************/

#include <cstring>
#include <iostream>

#include "bbu_server.h"

using namespace uv;
using namespace std;

Server::Server(EventLoop* loop)
    :TcpServer(loop)
{
    //setMessageCallback(std::bind(&Server::WriteMesg, this, placeholders::_1, placeholders::_2, placeholders::_3));
    //setMessageCallback(std::bind(&Server::ReadMesg, this, placeholders::_1, placeholders::_2, placeholders::_3));
    setMessageCallback(std::bind(&Server::newMessage, this, placeholders::_1, placeholders::_2, placeholders::_3));
}

void Server::writeCallback(uv::WriteInfo& info)
{
    uv::LogWriter::Instance()->debug("Server::writeCallback");
    if(0 != info.status)
    {
        std::cout << "Write error ：" << EventLoop::GetErrorMessage(info.status) << std::endl;
    }
    delete[] info.buf;
}

void Server::newMessage(shared_ptr<TcpConnection> connection, const char* buf, ssize_t size)
{
    std::cout << "BBU newMessage:Recv Msg: " << std::string(buf, size) << std::endl;
    std::cout << "Msg size=" << size << std::endl;

    uv::PacketIR::PackHead head;
    std::string recv_buf(buf, size);
    std::string phead = recv_buf.substr(0, sizeof(head) - 1);
    std::cout << "phead=" << phead << std::endl;

    std::string pdata = recv_buf.substr(sizeof(head));
    std::cout << "pdata=" << pdata << std::endl;

    //strcpy(head.t_type, phead[0]);                                                                                
    //strcpy(head.t_msgID, "1001");
    //strcpy(head.t_state, "1");
    //strcpy(head.t_RRUID, "4");
    //strcpy(head.t_PORT, "2");
    //head.t_type = buf[0];

    //std::cout << "head.t_type=" << head.t_type << std::endl;
    //std::cout << "head.t_type=" << head.t_type << std::endl;
#if 1
    if(uv::GlobalConfig::BufferModeStatus == uv::GlobalConfig::NoBuffer)
    {
        //char* data = new char[3] {'6', '6', '6'};
        //connection->write(data, sizeof(data), writeCallback);

        connection->write(buf, size, nullptr);
    } else {
        Packet packet;
        auto packetbuf = connection->getPacketBuffer();
        if (nullptr != packetbuf)
        {
            packetbuf->append(buf, static_cast<int>(size));

            while (0 == packetbuf->readPacket(packet))
            {
                std::cout << "reserver data " << packet.DataSize() << ":" << packet.getData() << std::endl;
                connection->write(packet.Buffer().c_str(), packet.PacketSize(), nullptr);
            }
        }
    }
#endif
}

void Server::ReadMesg(shared_ptr<TcpConnection> connection, const char* buf, ssize_t size)
{
    std::cout << "BBU Recv Msg: " << std::string(buf, size) << std::endl;
}

void Server::WriteMesg(shared_ptr<TcpConnection> connection, const char* buf, ssize_t size)
{
    if(uv::GlobalConfig::BufferModeStatus == uv::GlobalConfig::NoBuffer)
    {
        connection->write(buf, size, nullptr);
    } else {
        Packet packet;
        auto packetbuf = connection->getPacketBuffer();
        if (nullptr != packetbuf)
        {
            packetbuf->append(buf, static_cast<int>(size));

            while (0 == packetbuf->readPacket(packet))
            {
                std::cout << "reserver data " << packet.DataSize() << ":" << packet.getData() << std::endl;
                connection->write(packet.Buffer().c_str(), packet.PacketSize(), nullptr);
            }
        }
    }
}

void Server::SendMsg(const char* msg, ssize_t size)
{
    std::map<std::string ,TcpConnectionPtr>  allconnnections;
    getAllConnection(allconnnections);

    for(auto &conn : allconnnections)
    {
        std::cout << "client ip=" << conn.first << std::endl; 
        //conn.second->write(msg, size, nullptr);
        WriteMesg(conn.second, msg, size);
    }
#if 0
    auto conn = allconnnections.begin();
    for(conn; conn != allconnnections.end(); conn++)
    {
        std::cout << "client ip=" << conn->first << std::endl; 
        conn->second->write(msg, size, nullptr);
    }
#endif
}
