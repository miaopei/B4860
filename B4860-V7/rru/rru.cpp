/*************************************************************************
	> File Name: RRU.cpp
	> Author: miaopei
	> Mail: miaopei@baicells.com 
	> Created Time: 2020年02月12日 星期三 11时16分50秒
 ************************************************************************/

#include <iostream>
#include <string.h>

#include "rru.h"

using namespace uv;
using namespace std;

RRU::RRU(uv::EventLoop* loop)
    :TcpClient(loop),
    sockAddr(nullptr)
{
    setConnectStatusCallback(std::bind(&RRU::onConnect, this, std::placeholders::_1));
    setMessageCallback(std::bind(&RRU::RecvMessage, this, std::placeholders::_1, std::placeholders::_2));
    SetRRRUInfo();
}

void RRU::connectToServer(uv::SocketAddr& addr)
{
    sockAddr = std::make_shared<uv::SocketAddr>(addr);
    connect(addr);
}

void RRU::onConnect(ConnectStatus status)
{
    if(status != ConnectStatus::OnConnectSuccess)
    {
        reConnect();
    } else {
        SendConnectMessage();
    }
}

void RRU::reConnect()
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

void RRU::SendConnectMessage()
{
    std::string data = "Version=1.0";
    
    uv::Packet::Head head;
    head.s_source = m_source;
    head.s_destination = to_string(uv::Packet::TO_BBU);
	head.s_mac = m_mac;
    head.s_state = to_string(uv::Packet::REQUEST);
    head.s_msgID = to_string(uv::Packet::MSG_CONNECT);
    head.s_hop = m_hop;
    head.s_port = m_port;
    head.s_uport = m_uport;

    SendPackMessage(head, data, data.length());
}

void RRU::SetRRRUInfo()
{
#if 0
    int mpi_fd = gpmc_mpi_open(GPMC_MPI_DEV);
    /* 获取 rRRU 的 port id 信息 */
    uint16_t rRRU_port = get_rhup_port_id(mpi_fd, UP);

    std::cout << "rRRU_port = " << rRRU_port << std::endl;
    std::cout << "port = " << ((rRRU_port >> 8) & 0xf) << std::endl;
    std::cout << "rruid = " << ((rRRU_port >> 4) & 0xf) << std::endl;
    std::cout << "uport = " << (rRRU_port & 0xf) << std::endl;

    m_source = to_string(uv::Packet::RRU);
    m_port = to_string(((rRRU_port >> 8) & 0xf));
    m_rruid = to_string(((rRRU_port >> 4) & 0xf));
    m_uport = to_string((rRRU_port & 0xf));

    gpmc_mpi_close(mpi_fd);
#endif
	uv::Packet packet;
	char mac[32] = {0};
	if(!packet.GetDeviceMac(IFRNAME, mac))
    {
        std::cout << "Error: GetMac error" << std::endl;
        return ;
    }
	
	//m_mac = mac;
	m_mac = "F48E38DCD7CD";
    m_source = to_string(uv::Packet::RRU);
    m_port = "0";
    m_hop = "4";
    m_uport = "4";
}

void RRU::RecvMessage(const char* buf, ssize_t size)
{
    if(size < HEADLENGTH)
    {
        std::cout << "Message Length error." << std::endl;
        return;
    }

    /* 接收到的数据解析 */
	auto packetbuf = getCurrentBuf();
	if (nullptr != packetbuf)
	{
		packetbuf->append(buf, static_cast<int>(size));
		uv::Packet packet;
		while (0 == packetbuf->readPacket(packet))
		{
			std::cout << "[ReceiveData: " << packet.DataSize() << ":" << packet.getData() << "]" << std::endl;
			packet.UnPackMessage();

			/* 打印解包信息 */
			packet.EchoUnPackMessage();

			ProcessRecvMessage(packet);
		}
	}
}

void RRU::ProcessRecvMessage(uv::Packet& packet)
{
    switch(std::stoi(packet.GetMsgID()))
    {
        case uv::Packet::MSG_CONNECT:
            std::cout << "[RCV:msg_connect]" << std::endl;
            ConnectResultProcess(packet);
            break;
        default:
            std::cout << "[Error: MessageID Error]" << std::endl;
    }
}

void RRU::SendPackMessage(uv::Packet::Head& head, std::string& data, ssize_t size)
{
    uv::Packet packet;
    packet.SetHead(head);

    packet.PackMessage(data, size);

    /* 打印数据封装信息 */
    //packet.EchoPackMessage();
    
    std::string send_buf = packet.GetPacket();
    
    SendMessage(send_buf.c_str(), send_buf.length());
}
void RRU::SendMessage(const char* buf, ssize_t size)
{
    std::cout << "[SendMessage: " << buf << "]" << std::endl;
    if(uv::GlobalConfig::BufferModeStatus == uv::GlobalConfig::NoBuffer)
    {
        write(buf, (unsigned int)size);
    } else {
        uv::Packet packet;
        packet.pack(buf, size);
        write(packet.Buffer().c_str(), packet.PacketSize());
    }
}

void RRU::ConnectResultProcess(uv::Packet& packet)
{
    /* connect bbu success, process delay info and send to bbu */
    /* delay 500ms for update hub delay info */
    std::this_thread::sleep_for(chrono::milliseconds(500)); // 延时 500ms
    SendRRRUDelayInfo();
    //TestProcess(packet);
}

void RRU::SendRRRUDelayInfo()
{
	uv::Packet::Head head;
    head.s_source = m_source;
    head.s_destination = to_string(uv::Packet::TO_BBU);
	head.s_mac = m_mac;
    head.s_state = to_string(uv::Packet::REQUEST);
    head.s_msgID = to_string(uv::Packet::MSG_DELAY_MEASUREMENT);
    head.s_hop = m_hop;
    head.s_port = m_port;
    head.s_uport = m_uport;

    //TestProcess(packet);
    std::string data = "T2a=123&Ta3=456";
    
    SendPackMessage(head, data, data.length());
}


