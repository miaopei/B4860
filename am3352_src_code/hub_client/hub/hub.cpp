/*************************************************************************
	> File Name: hub.cpp
	> Author: miaopei
	> Mail: miaopei@baicells.com 
	> Created Time: 2020年02月12日 星期三 11时16分50秒
 ************************************************************************/

#include <iostream>
#include <string.h>

#include "hub.h"

using namespace uv;
using namespace std;

HUB::HUB(uv::EventLoop* loop)
    :TcpClient(loop),
    sockAddr(nullptr)
{
    setConnectStatusCallback(std::bind(&HUB::onConnect, this, std::placeholders::_1));
    setMessageCallback(std::bind(&HUB::RecvMessage, this, std::placeholders::_1, std::placeholders::_2));

    SetRHUBInfo();
}

void HUB::connectToServer(uv::SocketAddr& addr)
{
    sockAddr = std::make_shared<uv::SocketAddr>(addr);
    connect(addr);
}

void HUB::onConnect(ConnectStatus status)
{
    if(status != ConnectStatus::OnConnectSuccess)
    {
        reConnect();
    } else {
        SendConnectMessage();
    }
}

void HUB::reConnect()
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

void HUB::SendConnectMessage()
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

void HUB::SetRHUBInfo()
{
	uv::Packet packet;
	char mac[32] = {0};
	if(!packet.GetDeviceMac(IFRNAME, mac))
    {
        std::cout << "Error: GetMac error" << std::endl;
        return ;
    }
	m_mac = mac;
	
    int mpi_fd = gpmc_mpi_open(GPMC_MPI_DEV);
    /* 获取 rhub 的 port id 信息 */
    uint16_t rhub_port = get_rhup_port_id(mpi_fd, UP);

    std::cout << "rhub_port = " << rhub_port << std::endl;
    std::cout << "port = " << ((rhub_port >> 8) & 0xf) << std::endl;
    std::cout << "rruid = " << ((rhub_port >> 4) & 0xf) << std::endl;
    std::cout << "uport = " << (rhub_port & 0xf) << std::endl;

    m_source = to_string(uv::Packet::HUB);
    m_port = to_string(((rhub_port >> 8) & 0xf));
    m_hop = to_string(((rhub_port >> 4) & 0xf));
    m_uport = to_string((rhub_port & 0xf));

    gpmc_mpi_close(mpi_fd);
}

void HUB::SendRHUBDelayInfo()
{
    int mpi_fd = gpmc_mpi_open(GPMC_MPI_DEV);
    
    /* 获取 rhub 的处理时延  */
    struct rhup_data_delay* rhub_up = (struct rhup_data_delay*)malloc(sizeof(struct rhup_data_delay));
    get_rhup_delay(mpi_fd, UP, rhub_up);
    std::string data = std::string("delay1_up=" + to_string(rhub_up->delay1) + 
								   "&delay2_up=" + to_string(rhub_up->delay2) +
								   "&delay3_up=" + to_string(rhub_up->delay3) +
								   "&delay4_up=" + to_string(rhub_up->delay4) +
								   "&delay5_up=" + to_string(rhub_up->delay5) +
								   "&delay6_up=" + to_string(rhub_up->delay6) +
								   "&delay7_up=" + to_string(rhub_up->delay7) +
								   "&delay8_up=" + to_string(rhub_up->delay8) +
								   "&delay9_up=" + to_string(rhub_up->delay9));

    struct rhup_data_delay* rhub_down = (struct rhup_data_delay*)malloc(sizeof(struct rhup_data_delay));
    get_rhup_delay(mpi_fd, DOWN, rhub_down);
	data += std::string("&delay1_down=" + to_string(rhub_down->delay1) + 
						"&delay2_down=" + to_string(rhub_down->delay2) +
						"&delay3_down=" + to_string(rhub_down->delay3) +
						"&delay4_down=" + to_string(rhub_down->delay4) +
						"&delay5_down=" + to_string(rhub_down->delay5) +
						"&delay6_down=" + to_string(rhub_down->delay6) +
						"&delay7_down=" + to_string(rhub_down->delay7) +
						"&delay8_down=" + to_string(rhub_down->delay8) +
						"&delay9_down=" + to_string(rhub_down->delay9));
    
    /* 获取 rhub T14 测量时延信息 */
    struct rhup_t14_delay* t14_delay = (struct rhup_t14_delay*)malloc(sizeof(struct rhup_t14_delay));
    get_rhup_t14_delay(mpi_fd, t14_delay);
	data += std::string("&t14_delay1=" + to_string(t14_delay->delay1) + 
						"&t14_delay2=" + to_string(t14_delay->delay2) +
						"&t14_delay3=" + to_string(t14_delay->delay3) +
						"&t14_delay4=" + to_string(t14_delay->delay4) +
						"&t14_delay5=" + to_string(t14_delay->delay5) +
						"&t14_delay6=" + to_string(t14_delay->delay6) +
						"&t14_delay7=" + to_string(t14_delay->delay7) +
						"&t14_delay8=" + to_string(t14_delay->delay8) +
						"&t14_delay9=" + to_string(t14_delay->delay9));

    data += std::string("&toffset=" + to_string(TOFFSET));

    uv::Packet::Head head;
    head.s_source = m_source;
    head.s_destination = to_string(uv::Packet::TO_BBU);
	head.s_mac = m_mac;
    head.s_state = to_string(uv::Packet::RESPONSE);
    head.s_msgID = to_string(uv::Packet::MSG_DELAY_MEASUREMENT);
    head.s_hop = m_hop;
    head.s_port = m_port;
    head.s_uport = m_uport;

    SendPackMessage(head, data, data.length());

    free(rhub_up);
    free(rhub_down);
    free(t14_delay);
    gpmc_mpi_close(mpi_fd);
}

void HUB::RecvMessage(const char* buf, ssize_t size)
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

void HUB::ProcessRecvMessage(uv::Packet& packet)
{
	switch(std::stoi(packet.GetMsgID()))
    {
        case uv::Packet::MSG_CONNECT:
            std::cout << "[RCV:msg_connect]" << std::endl;
            ConnectResultProcess(packet);
            break;
        case uv::Packet::MSG_UPDATE_DELAY:
            std::cout << "[RCV:msg_updata_delay]" << std::endl;
            UpdataDelay(packet);
            break;
        default:
            std::cout << "[Error: MessageID Error]" << std::endl;
    }
}

void HUB::SendPackMessage(uv::Packet::Head& head, std::string& data, ssize_t size)
{
    uv::Packet packet;
    packet.SetHead(head);

    packet.PackMessage(data, size);

    /* 打印数据封装信息 */
    //packet.EchoPackMessage();
    
    std::string send_buf = packet.GetPacket();
    
    SendMessage(send_buf.c_str(), send_buf.length());
}

void HUB::SendMessage(const char* buf, ssize_t size)
{
    std::cout << "[SendMessage: " << buf << "]" << std::endl;
    if(uv::GlobalConfig::BufferModeStatus == uv::GlobalConfig::NoBuffer)
    {
        //writeInLoop(buf, (unsigned int)size, nullptr);
        write(buf, (unsigned int)size);
    } else {
        uv::Packet packet;
        packet.pack(buf, size);
        write(packet.Buffer().c_str(), packet.PacketSize());
    }
}

void HUB::ConnectResultProcess(uv::Packet& packet)
{
    SendRHUBDelayInfo();
    //TestProcess(packet);
}

void HUB::UpdataDelay(uv::Packet& packet)
{	
    uv::Packet::Head head;
    head.s_source = to_string(uv::Packet::HUB);
    head.s_destination = to_string(uv::Packet::TO_BBU);
	head.s_mac = m_mac;
    head.s_state = to_string(uv::Packet::RESPONSE);
    head.s_msgID = to_string(uv::Packet::MSG_UPDATE_DELAY);
    head.s_hop = m_hop;
    head.s_port = m_port;
    head.s_uport = m_uport;

    //TestProcess(packet);
    std::string data = "delay1_up=34&delay2_up=35&delay3_up=36&delay4_up=37&delay5_up=38&delay1_down=36&delay2_down=37&delay3_down=38&delay4_down=39&delay5_down=37&t14_delay1=11488&t14_delay2=11488&t14_delay3=11488&t14_delay4=11488&t14_delay5=11488";
    
    SendPackMessage(head, data, data.length());
}



