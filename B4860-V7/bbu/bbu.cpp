/*************************************************************************
	> File Name: bbu.cpp
	> Author: miaopei
	> Mail: miaopei@baicells.com 
	> Created Time: 2020年02月12日 星期三 11时15分05秒
 ************************************************************************/

#include <cstring>
#include <iostream>

#include "bbu.h"

using namespace uv;
using namespace std;

BBU::BBU(EventLoop* loop)
    :TcpServer(loop)
{
    setMessageCallback(std::bind(&BBU::OnMessage, this, placeholders::_1, placeholders::_2, placeholders::_3));
}

void BBU::OnMessage(shared_ptr<TcpConnection> connection, const char* buf, ssize_t size)
{
	if(size < HEADLENGTH)
	{
		std::cout << "Message length error." << std::endl;
		return ;
	}

	std::string revb_buf = std::string(buf, size);
    uv::PacketIR packet;
    packet.UnPackMessage(revb_buf);

	/* 打印解包信息 */
	packet.EchoUnPackMessage();

	switch(std::stoi(packet.GetDestination()))
	{
		case uv::PacketIR::TO_BBU:
			std::cout << "[Destiantion: BBU]" << std::endl;
			BBUMessageProcess(connection, packet);
			break;
		case uv::PacketIR::TO_HUB:
			std::cout << "[Destiantion: HUB]" << std::endl;
			HUBMessageProcess(connection, packet);
			break;
		case uv::PacketIR::TO_RRU:
			std::cout << "[Destiantion: RRB]" << std::endl;
			RRUMessageProcess(connection, packet);
			break;
		case uv::PacketIR::TO_OAM:
			std::cout << "[Destiantion: OAM]" << std::endl;
			OAMMessageProcess(connection, packet);
			break;
		default:
			std::cout << "[Error: Destiantion: Error]" << std::endl;
	}
}

void BBU::BBUMessageProcess(uv::TcpConnectionPtr& connection, uv::PacketIR& packet)
{
	/* BBU 为消息处理中心，接收到的都是 request 消息*/
	switch(std::stoi(packet.GetMsgID()))
	{
		case uv::PacketIR::MSG_CONNECT:
			std::cout << "[msg_connect]" << std::endl;
			SetConnectionClient(connection, packet);
            break;
		case uv::PacketIR::MSG_DELAY_MEASUREMENT:
			std::cout << "[msg_delay_measurement]" << std::endl;
			DelayMeasurementProcess(connection, packet);
			break;
        case uv::PacketIR::MSG_UPDATE_DELAY:
            std::cout << "[msg_update_delay]" << std::endl;
            UpdateHUBDelayInfo(packet);
            break;
		default:
			std::cout << "[Error: MessageID Error]" << std::endl;
	}
}

void BBU::HUBMessageProcess(uv::TcpConnectionPtr& connection, uv::PacketIR& packet)
{

}

void BBU::RRUMessageProcess(uv::TcpConnectionPtr& connection, uv::PacketIR& packet)
{

}
void BBU::OAMMessageProcess(uv::TcpConnectionPtr& connection, uv::PacketIR& packet)
{

}

void BBU::SetConnectionClient(uv::TcpConnectionPtr& connection, uv::PacketIR& packet)
{
	ClientInfo cInfo;
	cInfo.s_ip = GetCurrentName(connection);
	cInfo.s_connection = connection;
	cInfo.s_source = packet.GetSource();
	cInfo.s_RRUID = packet.GetRRUID();
	cInfo.s_port = packet.GetPort();
	cInfo.s_uport = packet.GetUPort();

	if(!SetConnectionInfo(connection, cInfo))
	{
		std::cout << "[Error: SetConnectionInfo Error]" << std::endl;
		return;
	}

    /* 如果 source 是 RRU 需要更新上级 HUB 延时测量信息*/
    if(cInfo.s_source == to_string(uv::PacketIR::RRU))
    {
        SendUpdateHUBDelayMessage(packet);
    }
	
	std::cout << "[SetConnectionInfo Success, Echo Message to " << packet.GetSource() << "]"<< std::endl;

	/* Version Check */
	/* TODO*/

    /* 需要优化，使用同一的消息发送接口 */
    SendConnectionMessage(connection, packet);
}

void BBU::DelayMeasurementProcess(uv::TcpConnectionPtr& connection, uv::PacketIR& packet)
{
	std::cout << "packet.data=" << packet.GetData() << std::endl;
    switch(std::stoi(packet.GetSource()))
    {
        case uv::PacketIR::HUB:
            std::cout << "[hub_delay_measurement]" << std::endl;
			HubDelayInfo(packet);
            break;
        case uv::PacketIR::RRU:
            std::cout << "[rru_delay_measurement]" << std::endl;
            break;
        default:
            std::cout << "[Error: delay measurement source error]" << std::endl;
    }
}


void BBU::UnPackData(uv::PacketIR& packet)
{
	/* Json 存储数据 jsoncpp，考虑同时支持 xml 存储数据 */
	
}

void BBU::SendConnectionMessage(uv::TcpConnectionPtr& connection, uv::PacketIR& packet)
{
	std::string data = "CheckResult=0";
    uv::PacketIR packetir;
    
    packetir.SetHead(packet.GetDestination(), 
                     packet.GetSource(),
                     to_string(uv::PacketIR::RESPONSE),
                     packet.GetMsgID(), 
                     packet.GetRRUID(),
                     packet.GetPort(),
                     packet.GetUPort());

    packetir.PackMessage(data, data.length());

	/* 打印数据封装信息 */
	packetir.EchoPackMessage();

	std::string send_buf = packetir.GetPacket();

	SendMessage(connection, send_buf.c_str(), send_buf.length());
}

void BBU::SendPackMessage(uv::TcpConnectionPtr& connection, Head head, std::string& data, ssize_t size)
{
    uv::PacketIR packetir;
    packetir.SetHead(head.s_source, 
                     head.s_destination,
                     head.s_state,
                     head.s_msgID, 
                     head.s_rruid,
                     head.s_port,
                     head.s_uport);

    packetir.PackMessage(data, size);

	/* 打印数据封装信息 */
	packetir.EchoPackMessage();

	std::string send_buf = packetir.GetPacket();

	SendMessage(connection, send_buf.c_str(), send_buf.length());
}

void BBU::SendPackMessageToAllDevice(DeviceType device, Head head, std::string& data, ssize_t size)
{
    if(device == ALL_HUB_DEVICE)
    switch(device)
    {
        case ALL_HUB_DEVICE:
            std::cout << "[SendPackMessageToAllDevice: ALL_HUB_DEVICE]" << std::endl;
            break;
        case ALL_RRU_DEVICE:
            std::cout << "[SendPackMessageToAllDevice: ALL_RRU_DEVICE]" << std::endl;
            break;
        default:
            std::cout << "[Error: SendPackMessageToAllDevice device error]" << std::endl;
    }
}

void BBU::CalculationDelayCompensation()
{

}

void BBU::SendUpdateHUBDelayMessage(uv::PacketIR& packet)
{
    shared_ptr<TcpConnection> connection;
    if(!QueryUhubConnection(packet.GetRRUID(), connection))
    {
        std::cout << "[Error: SendUpdateHUBDelayMessage not find hub rruid]" << std::endl;
        return;
    }
    
    /* 封装消息，指定 HUB 更新时延测量 */
    Head head;
    head.s_source = packet.GetDestination();
    head.s_destination = to_string(uv::PacketIR::TO_HUB);
    head.s_state = to_string(uv::PacketIR::REQUEST);
    head.s_msgID = to_string(uv::PacketIR::MSG_UPDATE_DELAY);
    head.s_rruid = packet.GetRRUID();
    head.s_port = packet.GetPort();
    head.s_uport = packet.GetUPort();

    std::string data = "updataDelayInfo=0";

    SendPackMessage(connection, head, data, data.length());
}

void BBU::UpdateHUBDelayInfo(uv::PacketIR& packet)
{
    UpdateDelayInfo(packet.GetData(), packet.GetRRUID(), delay_map);

    for(auto &it : delay_map)
    {
        std::cout << "key=" << it.first 
                  << " key.key=" << it.second.key
                  << " key.value=" << it.second.value << std::endl;
    }
}

void BBU::HubDelayInfo(uv::PacketIR& packet)
{
	std::string data = packet.GetData();
    SplitStrings2Map(data, packet.GetRRUID(), delay_map);
    
    for(auto &it : delay_map)
    {
        std::cout << "key=" << it.first 
                  << " key.key=" << it.second.key
                  << " key.value=" << it.second.value << std::endl;
    }

#if 0
	/* 测试hubdelayinfo */ 
	std::string data = packet.GetData();
	std::string rruid = "1";
    //std::map<std::string, atom> delay_map;
    SplitStrings2Map(data, rruid, delay_map);

    //rruid = "3";
    SplitStrings2Map(data, packet.GetRRUID(), delay_map);

	rruid = "4";
    SplitStrings2Map(data, rruid, delay_map);

    for(auto &it : delay_map)
    {
        std::cout << "key=" << it.first 
                  << " key.key=" << it.second.key
                  << " key.value=" << it.second.value << std::endl;
    }

    rruid = "1";
    DeleteHubDelay(rruid, delay_map);
    std::cout << "/t----------------------" << std::endl;
    for(auto &it : delay_map)
    {
        std::cout << "key=" << it.first 
                  << " key.key=" << it.second.key
                  << " key.value=" << it.second.value << std::endl;
    }
#endif
}

bool BBU::QueryUhubConnection(std::string rruid, uv::TcpConnectionPtr& connection)
{
    std::map<std::string, ClientInfo> netTopology;
    GetNetworkTopology(netTopology);

    for(auto &it : netTopology)
    { 
        std::cout << "netTopology: " 
            << it.first << " - > " 
            << it.second.s_ip << " "
            << it.second.s_connection << " "
            << it.second.s_source << " "
            << it.second.s_RRUID << " " 
            << it.second.s_port << " "
            << it.second.s_uport << std::endl;

        if(std::stoi(it.second.s_RRUID) == (std::stoi(rruid) - 1))
        {
            connection = it.second.s_connection;
            return true;
        }
    }
    return false;
}






#if 0
void BBU::writeCallback(uv::WriteInfo& info)
{
    uv::LogWriter::Instance()->debug("Server::writeCallback");
    if(0 != info.status)
    {
        std::cout << "Write error ：" << EventLoop::GetErrorMessage(info.status) << std::endl;
    }
    delete[] info.buf;
}
#endif

void BBU::SendMessage(shared_ptr<TcpConnection> connection, const char* buf, ssize_t size)
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

#if 0
void BBU::SendAllClientMessage(const char* msg, ssize_t size)
{
    std::map<std::string ,TcpConnectionPtr>  allconnnections;
    getAllConnection(allconnnections);

    for(auto &conn : allconnnections)
    {
        std::cout << "client ip=" << conn.first << std::endl; 
        //conn.second->write(msg, size, nullptr);
        SendMessage(conn.second, msg, size);
    }
}
#endif

void BBU::NetworkTopology()
{
    std::map<std::string, ClientInfo> netTopology;
    GetNetworkTopology(netTopology);

    for(auto &it : netTopology)
    { 
        std::cout << "netTopology: " 
            << it.first << " - > " 
            << it.second.s_ip << " "
            << it.second.s_connection << " "
            << it.second.s_source << " "
            << it.second.s_RRUID << " " 
            << it.second.s_port << " "
            << it.second.s_uport << std::endl;
    }
}

void BBU::TestGet()
{
    std::vector<TcpConnectionPtr> hubsConnection;
    GetHUBsConnection(hubsConnection);
    for(auto &it : hubsConnection)
    {
        std::cout << "hubsConnection: " << it << std::endl;
    }

    std::vector<TcpConnectionPtr> rrusConnection;
    GetRRUsConnection(rrusConnection);
    for(auto &it : rrusConnection)
    {
        std::cout << "rrusConnection: " << it << std::endl;
    }
}


