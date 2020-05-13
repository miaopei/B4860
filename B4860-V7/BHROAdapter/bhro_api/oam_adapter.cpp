/*************************************************************************
	> File Name: OamAdapter.cpp
	> Author: miaopei
	> Mail: miaopei@baicells.com 
	> Created Time: 2020年02月12日 星期三 11时16分50秒
 ************************************************************************/

#include <iostream>
//#include <string.h>

#include "oam_adapter.h"

using namespace uv;
using namespace std;

OamAdapter::OamAdapter(uv::EventLoop* loop)
    :TcpClient(loop),
    sockAddr(nullptr)
{
    setConnectStatusCallback(std::bind(&OamAdapter::onConnect, this, std::placeholders::_1));
    setMessageCallback(std::bind(&OamAdapter::RecvMessage, this, std::placeholders::_1, std::placeholders::_2));
    SetROamAdapterInfo();
}

void OamAdapter::connectToServer(uv::SocketAddr& addr)
{
    sockAddr = std::make_shared<uv::SocketAddr>(addr);
    connect(addr);
}

void OamAdapter::onConnect(ConnectStatus status)
{
    if(status != ConnectStatus::OnConnectSuccess)
    {
        reConnect();
    } else {
        SendConnectMessage();
    }
}

void OamAdapter::reConnect()
{
    uv::Timer* timer = new uv::Timer(loop_, 500, 0, [this](uv::Timer* ptr)
    {
        char* pdata = NULL;
        size_t size = 32;
        pdata = (char*)malloc(size * sizeof(char));
        if(pdata == NULL)
        {		
            LOG_PRINT(LogLevel::error, "malloc gateway memory error");
        }

        GetDeviceIP(IFRNAME, pdata, size);	
        LOG_PRINT(LogLevel::debug, "ReConnect Device IP: %s", pdata);

        SocketAddr addr(pdata, PORT, SocketAddr::Ipv4);
        connect(addr);

        free(pdata);
        pdata = NULL;

        ptr->close([](uv::Timer* ptr)
        {
            delete ptr;    
        });
    });
    timer->start();
}

void OamAdapter::SendConnectMessage()
{
    std::string data = "ResultID=0";
    
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

void OamAdapter::SetROamAdapterInfo()
{
#if 0
	uv::Packet packet;
	char mac[32] = {0};
	if(!packet.GetDeviceMac(IFRNAME, mac))
    {
        std::cout << "Error: GetMac error" << std::endl;
        return ;
    }
#endif
	//m_mac = mac;
	m_mac = "F48E38DCD7C1";
    m_source = to_string(uv::Packet::OAM);
    m_hop = "0";
    m_port = "0";
    m_uport = "0";
}

void OamAdapter::RecvMessage(const char* buf, ssize_t size)
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
			LOG_PRINT(LogLevel::debug, "[ReceiveData: %d:%s]", packet.DataSize(), packet.getData());
			packet.UnPackMessage();

			/* 打印解包信息 */
			//packet.EchoUnPackMessage();

			ProcessRecvMessage(packet);
		}
	}
}

void OamAdapter::ProcessRecvMessage(uv::Packet& packet)
{
    packet_ = packet;
    RSPStatus = true;
#if 0
    switch(std::stoi(packet.GetMsgID()))
    {
        case uv::Packet::MSG_CONNECT:
            //std::cout << "[RCV:msg_connect]" << std::endl;
            ConnectResultProcess(packet);
            break;
        case uv::Packet::MSG_GET_NETWORK_TOPOLOGY:
            NetworkTopologyMessageProcess(packet);
            break;
        default:
            std::cout << "[Error: MessageID Error]" << std::endl;
    }
#endif
}

void OamAdapter::SendPackMessage(uv::Packet::Head& head, std::string& data, ssize_t size)
{
    uv::Packet packet;
    packet.SetHead(head);

    packet.PackMessage(data, size);

    /* 打印数据封装信息 */
    //packet.EchoPackMessage();
    
    std::string send_buf = packet.GetPacket();
    
    SendMessage(send_buf.c_str(), send_buf.length());
}

void OamAdapter::HeartSendPackMessage(uv::Packet::Head head, std::string data, ssize_t size)
{
    uv::Packet packet;
    packet.SetHead(head);

    packet.PackMessage(data, size);

    /* 打印数据封装信息 */
    //packet.EchoPackMessage();
    
    std::string send_buf = packet.GetPacket();
    
    SendMessage(send_buf.c_str(), send_buf.length());
}

void OamAdapter::SendMessage(const char* buf, ssize_t size)
{
    LOG_PRINT(LogLevel::debug, "[SendMessage: %s]", buf);
    if(uv::GlobalConfig::BufferModeStatus == uv::GlobalConfig::NoBuffer)
    {
        write(buf, (unsigned int)size);
    } else {
        uv::Packet packet;
        packet.pack(buf, size);
        write(packet.Buffer().c_str(), packet.PacketSize());
    }
}

void OamAdapter::ConnectResultProcess(uv::Packet& packet)
{
	std::cout << "[OamAdapter Connect BBU Success]" << std::endl;
}

void OamAdapter::NetworkTopologyMessageProcess(uv::Packet& packet)
{
    std::vector<std::string> topo;
    std::vector<std::string> topos = packet.DataSplit(packet.GetData(), "#");  
    std::vector<std::string> value;

    std::cout << "\n" 
              << " \tIP" << "\t\t\tMAC" << "\t\tSource" << "\tHOP" << "\tUpgradeState" << "\tRouteIndex" 
              << std::endl;
    for(auto it : topos)
    {
        int i = 0;
        topo = packet.DataSplit(it, "&");
        for(auto res : topo)
        {
            value = packet.DataSplit(res, "="); 
            if(i == 5){
                std::cout << " \t\t" << value[1];
            }else{
                std::cout << " \t" << value[1];
            }
            i++;
        }
        std::cout << std::endl;
        //std::cout << "\t " << topo[0] << " \t" << topo[1] << " \t" << topo[2] << " \t" << topo[3] << std::endl;
    }
}

void OamAdapter::SendUpgradeMessage(std::string destination, std::string fileName, std::string md5)
{
    std::string data = "fileName=" + fileName + "&md5=" + md5;
    
    uv::Packet::Head head;
    head.s_source = m_source;
    head.s_destination = destination;
	head.s_mac = m_mac;
    head.s_state = to_string(uv::Packet::REQUEST);
    head.s_msgID = to_string(uv::Packet::MSG_UPGRADE);
    head.s_hop = m_hop;
    head.s_port = m_port;
    head.s_uport = m_uport;

    SendPackMessage(head, data, data.length());
}

void OamAdapter::SendUpgradeMessage(std::string destination, std::string routeIndex, std::string fileName, std::string md5)
{
    std::string data = "routeIndex=" + routeIndex + "&fileName=" + fileName + "&md5=" + md5;
    
    uv::Packet::Head head;
    head.s_source = m_source;
    head.s_destination = destination;
	head.s_mac = m_mac;
    head.s_state = to_string(uv::Packet::REQUEST);
    head.s_msgID = to_string(uv::Packet::MSG_UPGRADE);
    head.s_hop = m_hop;
    head.s_port = m_port;
    head.s_uport = m_uport;

    SendPackMessage(head, data, data.length());
}

void OamAdapter::SendRFTxMessage(std::string RFTxStatus)
{
    std::string data = "RFTxStatus=" + RFTxStatus;
    
    uv::Packet::Head head;
    head.s_source = m_source;
    head.s_destination = to_string(uv::Packet::TO_RRU);
	head.s_mac = m_mac;
    head.s_state = to_string(uv::Packet::REQUEST);
    head.s_msgID = to_string(uv::Packet::MSG_RFTxStatus_SET);
    head.s_hop = m_hop;
    head.s_port = m_port;
    head.s_uport = m_uport;

    SendPackMessage(head, data, data.length());
}

void OamAdapter::SendRFTxMessage(std::string routeIndex, std::string RFTxStatus)
{
    std::string data = "routeIndex=" + routeIndex + "&RFTxStatus=" + RFTxStatus;
    
    uv::Packet::Head head;
    head.s_source = m_source;
    head.s_destination = to_string(uv::Packet::TO_RRU);
	head.s_mac = m_mac;
    head.s_state = to_string(uv::Packet::REQUEST);
    head.s_msgID = to_string(uv::Packet::MSG_RFTxStatus_SET);
    head.s_hop = m_hop;
    head.s_port = m_port;
    head.s_uport = m_uport;

    SendPackMessage(head, data, data.length());
}

void OamAdapter::GetNetworkTopology()
{
    RSPStatus = false;
	std::string data = "";
    
    uv::Packet::Head head;
    head.s_source = m_source;
    head.s_destination = to_string(uv::Packet::TO_BBU);
	head.s_mac = m_mac;
    head.s_state = to_string(uv::Packet::REQUEST);
    head.s_msgID = to_string(uv::Packet::MSG_GET_NETWORK_TOPOLOGY);
    head.s_hop = m_hop;
    head.s_port = m_port;
    head.s_uport = m_uport;

    SendPackMessage(head, data, data.length());
}

bool OamAdapter::GetRSPPacket(uv::Packet& packet)
{
    LOG_PRINT(LogLevel::debug, "GetRSPPacket ...");
    if(RSPStatus)
    {
        packet = packet_;
        return true;
    }
    return false;
}

void OamAdapter::Heart()
{
    LOG_PRINT(LogLevel::debug, "Start Heart ...");
    std::thread t1(std::bind(&OamAdapter::HandleHeart, this, (void*)this));
    t1.detach();
}

void OamAdapter::HandleHeart(void* arg)
{
    EventLoop loop;
    OamAdapter* adapter = (OamAdapter*)arg;

    std::string data = "heartbeat";
    uv::Packet::Head head;
    head.s_source = m_source;
    head.s_destination = to_string(uv::Packet::TO_BBU);
	head.s_mac = m_mac;
    head.s_state = to_string(uv::Packet::REQUEST);
    head.s_msgID = to_string(uv::Packet::MSG_HEART_BEAT);
    head.s_hop = m_hop;
    head.s_port = m_port;
    head.s_uport = m_uport;

    uv::Timer timer(&loop, 30000, 30000,                                                                  
        [&adapter, head, data](Timer*)
    {
        adapter->HeartSendPackMessage(head, data, data.length());
    });
    timer.start();
    loop.run();
}

