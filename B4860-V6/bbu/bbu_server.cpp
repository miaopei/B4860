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
    setMessageCallback(std::bind(&Server::OnMessage, this, placeholders::_1, placeholders::_2, placeholders::_3));
}

void Server::OnMessage(shared_ptr<TcpConnection> connection, const char* buf, ssize_t size)
{
    std::cout << "BBU Recv Msg: " << std::string(buf, size) << std::endl;
    std::cout << "Msg size=" << size << std::endl;

	std::string revb_buf = std::string(buf, size);
	uv::PacketIR packetir;
	packetir.UnPackMessage(revb_buf);
	std::cout << "解析 packet:" << std::endl;
    std::cout << "\tGetPacket: " << packetir.GetPacket() << std::endl;
    std::cout << "\tGetHead: " << packetir.GetHead() << std::endl;
    std::cout << "\tGetType: " << packetir.GetType() << std::endl;
    std::cout << "\tGetMsgID: " << packetir.GetMsgID() << std::endl;
    std::cout << "\tGetState: " << packetir.GetState() << std::endl;
    std::cout << "\tGetRRUID: " << packetir.GetRRUID() << std::endl;
    std::cout << "\tGetPort: " << packetir.GetPort() << std::endl;
    std::cout << "\tGetData: " << packetir.GetData() << std::endl;

    WriteMessage(connection, buf, size);
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

void Server::WriteMessage(shared_ptr<TcpConnection> connection, const char* buf, ssize_t size)
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

void Server::SendMessage(const char* msg, ssize_t size)
{
    std::map<std::string ,TcpConnectionPtr>  allconnnections;
    getAllConnection(allconnnections);

    for(auto &conn : allconnnections)
    {
        std::cout << "client ip=" << conn.first << std::endl; 
        //conn.second->write(msg, size, nullptr);
        WriteMessage(conn.second, msg, size);
    }
}
