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
	/* 数据包校验，数据粘包问题处理 */
	if(size < HEADLENGTH)
	{
        uv::LogWriter::Instance()->error("Message length error.");
		return ;
	}

	auto packetbuf = connection->getPacketBuffer();
    if (nullptr != packetbuf)
    {
        packetbuf->append(buf, static_cast<int>(size));
		uv::Packet packet;
        while (0 == packetbuf->readPacket(packet))
        {
            //uv::LogWriter::Instance()->info(std::string("ReceiveData: " + packet.DataSize()) + std::string(":" + to_string(packet.getData())));
            //uv::LogWriter::Instance()->info(std::string("ReceiveData: " + packet.DataSize()) + std::string(":") + std::string(packet.getData()));
            
            #if 1
            std::cout << __FUNCTION__ << ":" << __LINE__ 
                      << "[ReceiveData: " << packet.DataSize() << ":" 
                      << std::string(packet.getData(), packet.DataSize())  << "]" << std::endl;
            #endif

			packet.UnPackMessage();

			/* 打印解包信息 */
			//packet.EchoUnPackMessage();

			ProcessRecvMessage(connection, packet);
        }
    }
}

void BBU::ProcessRecvMessage(uv::TcpConnectionPtr connection, uv::Packet& packet)
{
	switch(std::stoi(packet.GetDestination()))
	{
		case uv::Packet::TO_BBU:
			std::cout << "[Destiantion: BBU]" << std::endl;
			BBUMessageProcess(connection, packet);
			break;
		case uv::Packet::TO_HUB:
			std::cout << "[Destiantion: HUB]" << std::endl;
			HUBMessageProcess(connection, packet);
			break;
		case uv::Packet::TO_RRU:
			std::cout << "[Destiantion: RRB]" << std::endl;
			RRUMessageProcess(connection, packet);
			break;
		case uv::Packet::TO_OAM:
			std::cout << "[Destiantion: OAM]" << std::endl;
			OAMMessageProcess(connection, packet);
			break;
		default:
			std::cout << "[Error: Destiantion: Error]" << std::endl;
	}
}

void BBU::BBUMessageProcess(uv::TcpConnectionPtr& connection, uv::Packet& packet)
{
	/* BBU 为消息处理中心，接收到的都是 request 消息*/
	switch(std::stoi(packet.GetMsgID()))
	{
		case uv::Packet::MSG_CONNECT:
			std::cout << "[msg_connect]" << std::endl;
			SetConnectionClient(connection, packet);
            break;
		case uv::Packet::MSG_DELAY_MEASUREMENT:
			std::cout << "[msg_delay_measurement]" << std::endl;
			DelayMeasurementProcess(connection, packet);
			break;
        case uv::Packet::MSG_UPDATE_DELAY:
            std::cout << "[msg_update_delay]" << std::endl;
            UpdateHUBDelayInfo(packet);
            break;
		default:
			std::cout << "[Error: MessageID Error]" << std::endl;
	}
}

void BBU::HUBMessageProcess(uv::TcpConnectionPtr& connection, uv::Packet& packet)
{

}

void BBU::RRUMessageProcess(uv::TcpConnectionPtr& connection, uv::Packet& packet)
{

}
void BBU::OAMMessageProcess(uv::TcpConnectionPtr& connection, uv::Packet& packet)
{

}

void BBU::SetConnectionClient(uv::TcpConnectionPtr& connection, uv::Packet& packet)
{
	DeviceInfo dInfo;
	dInfo.s_ip = GetCurrentName(connection);
	dInfo.s_connection = connection;
	dInfo.s_source = packet.GetSource();
	dInfo.s_RRUID = packet.GetRRUID();
	dInfo.s_port = packet.GetPort();
	dInfo.s_uport = packet.GetUPort();

	if(!SetConnectionInfo(connection, dInfo))
	{
		std::cout << "[Error: SetConnectionInfo Error]" << std::endl;
		return;
	}

    /* 如果 source 是 RRU 需要更新上级 HUB 延时测量信息*/
    if(dInfo.s_source == to_string(uv::Packet::RRU))
    {
        SendUpdateHUBDelayMessage(packet);
    }

	/* Version Check */
	/* TODO*/

    /* 需要优化，使用同一的消息发送接口 */
    SendConnectionMessage(connection, packet);
}

void BBU::DelayMeasurementProcess(uv::TcpConnectionPtr& connection, uv::Packet& packet)
{
    switch(std::stoi(packet.GetSource()))
    {
        case uv::Packet::HUB:
            std::cout << "[hub_delay_measurement]" << std::endl;
			HubDelayInfo(packet);
            break;
        case uv::Packet::RRU:
            std::cout << "[rru_delay_measurement]" << std::endl;
            RruDelayProcess(packet);
            break;
        default:
            std::cout << "[Error: delay measurement source error]" << std::endl;
    }
}


void BBU::UnPackData(uv::Packet& packet, std::map<std::string, std::string>& map)
{
	/* Json 存储数据 jsoncpp，考虑同时支持 xml 存储数据 */
	//packet.SplitData2Map(map);
}

void BBU::SendConnectionMessage(uv::TcpConnectionPtr& connection, uv::Packet& packet)
{
    uv::Packet::Head head;
    head.s_source       = packet.GetDestination();
    head.s_destination  = packet.GetSource();
    head.s_state        = to_string(uv::Packet::RESPONSE);
    head.s_msgID        = packet.GetMsgID();
    head.s_rruid        = packet.GetRRUID();
    head.s_port         = packet.GetPort();
    head.s_uport        = packet.GetUPort();

	std::string data = "CheckResult=0";

    SendPackMessage(connection, head, data, data.length());
}

void BBU::SendPackMessage(uv::TcpConnectionPtr& connection, uv::Packet::Head head, std::string& data, ssize_t size)
{
    uv::Packet Packet;
    Packet.SetHead(head.s_source, 
                     head.s_destination,
                     head.s_state,
                     head.s_msgID, 
                     head.s_rruid,
                     head.s_port,
                     head.s_uport);

    Packet.PackMessage(data, size);

	/* 打印数据封装信息 */
	//Packet.EchoPackMessage();

	std::string send_buf = Packet.GetPacket();

	SendMessage(connection, send_buf.c_str(), send_buf.length());
}

void BBU::SendPackMessageToAllDevice(DeviceType device, uv::Packet::Head head, std::string& data, ssize_t size)
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

void BBU::SendUpdateHUBDelayMessage(uv::Packet& packet)
{
    shared_ptr<TcpConnection> connection;
    if(!QueryUhubConnection(packet.GetRRUID(), connection))
    {
        std::cout << "[Error: SendUpdateHUBDelayMessage not find hub rruid]" << std::endl;
        return;
    }
    
    /* 封装消息，指定 HUB 更新时延测量 */
    uv::Packet::Head head;
    head.s_source = packet.GetDestination();
    head.s_destination = to_string(uv::Packet::TO_HUB);
    head.s_state = to_string(uv::Packet::REQUEST);
    head.s_msgID = to_string(uv::Packet::MSG_UPDATE_DELAY);
    head.s_rruid = packet.GetRRUID();
    head.s_port = packet.GetPort();
    head.s_uport = packet.GetUPort();

    std::string data = "updataDelayInfo";

    SendPackMessage(connection, head, data, data.length());
}

void BBU::UpdateHUBDelayInfo(uv::Packet& packet)
{
    UpdateDelayInfo(packet.GetData(), packet.GetRRUID(), delay_map);
	
#if 1
    for(auto &it : delay_map)
    {
        std::cout << "key=" << it.first 
                  << " key.key=" << it.second.key
                  << " key.value=" << it.second.value << std::endl;
    }
#endif
}

void BBU::HubDelayInfo(uv::Packet& packet)
{
	std::string data = packet.GetData();
    SplitStrings2Map(data, packet.GetRRUID(), delay_map);
    
#if 1
    for(auto &it : delay_map)
    {
        std::cout << "key=" << it.first 
                  << " key.key=" << it.second.key
                  << " key.value=" << it.second.value << std::endl;
    }
#endif
}

void BBU::RruDelayProcess(uv::Packet& packet)
{
    std::cout << "RruDelayProcess: packet.data=" << packet.GetData() << std::endl;

	double delayCompensation;

	CalculationDelayCompensation(packet, delayCompensation);

}

bool BBU::QueryUhubConnection(std::string rruid, uv::TcpConnectionPtr& connection)
{
    std::map<std::string, DeviceInfo> netTopology;
    GetNetworkTopology(netTopology);

	int ruhub_rruid = std::stoi(rruid) - 1;

    for(auto &it : netTopology)
    {
        if(it.second.s_RRUID == to_string(ruhub_rruid))
        {
            connection = it.second.s_connection;
            return true;
        }
    }
    return false;
}

std::string BBU::CreateRouteIndex(uv::Packet& packet)
{
    int level = 0;
    std::string RouteIndex;
    DeviceInfo dInfo;

    level = std::stoi(packet.GetRRUID()) - 1;
    if(level <= 0)
	{
		std::cout << __FILE__ << ":" << __LINE__ << ":" <<__FUNCTION__ 
                  << "#Error: level error" << std::endl;
		return "";
	} 

    RouteIndex = std::string(packet.GetPort() + "_" + packet.GetUPort());

    if(level >= 1)
    {
        for(level = level; level > 0; level--)
        {
            if(!FindDeviceInfo(level, dInfo))
            {
                std::cout << __FILE__ << ":" << __LINE__ << ":" <<__FUNCTION__ 
                    << "#Error: level error" << std::endl;
                return "";
            }
            RouteIndex += "_" + dInfo.s_port + "_" + dInfo.s_uport;
        }
    }
    return RouteIndex;
}

bool BBU::FindDeviceInfo(int level, DeviceInfo& dInfo)
{
    std::map<std::string, DeviceInfo> netTopology;
    GetNetworkTopology(netTopology);

    for(auto &it : netTopology)
    {
        if(it.second.s_RRUID == to_string(level))
        {
            dInfo = it.second;
            return true;
        }
    }
    return false;
}

bool BBU::CalculationDelayCompensation(uv::Packet& packet, double& delayCompensation)
{
	int level = 0;
	std::string key;
	std::string tbdelayDL;
	std::string tbdelayUL;
	std::string t14;
	std::string T2a;
	std::string Ta3;
	double totalDL;
	double totalUL;
	double totalDLHUBDelay;
	double totalULHUBDelay;
	double t12;

    mDelayDL.clear();
    mDelayUL.clear();
    
    std::string RouteIndex = CreateRouteIndex(packet);
    if(RouteIndex.empty())
    {
        std::cout << "Error: CreateRouteIndex failure" << std::endl;
        return false;
    }
    std::cout << "RouteIndex=" << RouteIndex << std::endl;

	std::map<std::string, std::string> map;
	packet.SplitData2Map(map);

	if(!FindDataMapValue(map, "T2a", T2a))
	{
		std::cout << __FUNCTION__ << "#Error: FindDataMapValue T2a error" << std::endl;
		return false;
	}

	if(!FindDataMapValue(map, "Ta3", Ta3))
	{
		std::cout << "Error: FindDataMapValue Ta3 error" << std::endl;
		return false;
	}
	
	std::cout << "T2a=" << T2a << " Ta3=" << Ta3 << std::endl;

	level = std::stoi(packet.GetRRUID()) - 1;
	if(level <= 0)
	{
		std::cout << "Error: level error" << std::endl;
		return false;
	} 

	/* 计算 rru 上一级 hub delay */
	/* TBdelay DL */
	key = std::string(to_string(level) + "1" + packet.GetUPort());
	std::cout << "key = " << key << std::endl;
	if(!FindDelayMapValue(key, tbdelayDL))
	{
		std::cout << "Error: FindDelayMapValue tbdelayDL error" << std::endl;
		return false;
	}
	std::cout << "tbdelayDL = " << tbdelayDL << std::endl;

	/* TBdelay UL */
	key = std::string(to_string(level) + "2" + packet.GetUPort());
	std::cout << "key = " << key << std::endl;
	if(!FindDelayMapValue(key, tbdelayUL))
	{
		std::cout << "Error: FindDelayMapValue tbdelayUL error" << std::endl;
		return false;
	}
	std::cout << "tbdelayUL = " << tbdelayUL << std::endl;

	/* T14 */
	key = std::string(to_string(level) + "3" + packet.GetUPort());
	std::cout << "key = " << key << std::endl;
	if(!FindDelayMapValue(key, t14))
	{
		std::cout << "Error: FindDelayMapValue t14 error" << std::endl;
		return false;
	}
	std::cout << "t14 = " << t14 << std::endl;

	totalDLHUBDelay = atof(tbdelayDL.c_str()) + ((atof(t14.c_str()) - (RRUToffset * TOFFSETCYCLE) / 2));
	totalULHUBDelay = atof(tbdelayUL.c_str()) + ((atof(t14.c_str()) - (RRUToffset * TOFFSETCYCLE) / 2));

	/* 级联 hub delay 计算，级联的情况 hub port=0 uport=1 */
	level = level - 1;
	if(level >= 1)
	{
		for(level = level; level > 0; level--)
		{
			std::cout << __FUNCTION__ << ":" << __LINE__ << ":" << "level=" << level << std::endl;

			/* TBdelay DL */
			key = std::string(to_string(level) + "1" + "1");  // 最后一位为级联口
			std::cout << "key = " << key << std::endl;
			if(!FindDelayMapValue(key, tbdelayDL))
			{
				std::cout << "Error: FindDelayMapValue tbdelayDL error" << std::endl;
				return false;
			}
			std::cout << "tbdelayDL = " << tbdelayDL << std::endl;
			totalDLHUBDelay += atof(tbdelayDL.c_str());

			/* TBdelay UL */
			key = std::string(to_string(level) + "2" + "1");
			std::cout << "key = " << key << std::endl;
			if(!FindDelayMapValue(key, tbdelayUL))
			{
				std::cout << "Error: FindDelayMapValue tbdelayUL error" << std::endl;
				return false;
			}
			std::cout << "tbdelayUL = " << tbdelayUL << std::endl;
			totalULHUBDelay += atof(tbdelayUL.c_str());

			/* T14 */
			key = std::string(to_string(level) + "3" + "1");
			std::cout << "key = " << key << std::endl;
			if(!FindDelayMapValue(key, t14))
			{
				std::cout << "Error: FindDelayMapValue t14 error" << std::endl;
				return false;
			}
			std::cout << "t14 = " << t14 << std::endl;
			totalDLHUBDelay += ((atof(t14.c_str()) - (RRUToffset * TOFFSETCYCLE) / 2));
			totalULHUBDelay += ((atof(t14.c_str()) - (RRUToffset * TOFFSETCYCLE) / 2));
		}
	}

	t12 = (BBUT14 - (HUBToffset * TOFFSETCYCLE)) / 2;
	totalDL = t12 + totalDLHUBDelay + atof(T2a.c_str());
	totalUL = t12 + totalULHUBDelay + atof(Ta3.c_str());
	std::cout << "totalDL=" << to_string(totalDL) << std::endl;
	std::cout << "totalUL=" << to_string(totalUL) << std::endl;

    mDelayDL.insert(make_pair(RouteIndex, to_string(totalDL)));
    mDelayUL.insert(make_pair(RouteIndex, to_string(totalUL)));

    sortMapByValue(mDelayDL, tVectorDL);
    sortMapByValue(mDelayUL, tVectorUL);

    std::cout << "Echo maxDelayDL Result: " << std::endl;
    EchoSortResult(tVectorDL);

    std::cout << "Echo maxDelayUL Result: " << std::endl;
    EchoSortResult(tVectorUL);

	return true;
}

bool BBU::FindDelayMapValue(std::string key, std::string& value)
{
	auto rst = delay_map.find(key);
	if(rst == delay_map.end())
	{
		std::cout << "Error: not find delay_map key" << std::endl;
		return false;
	}
	value = rst->second.value;
	return true;
}

bool BBU::FindDataMapValue(std::map<std::string, std::string>& map, std::string key, std::string& value)
{
	auto rst = map.find(key);
	if(rst == map.end())
	{
		std::cout << "Error: not find dataMap key" << std::endl;
		return false;
	}
	value = rst->second;
	return true;
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
    std::cout << "[SendMessage: " << buf << "]" << std::endl;
    if(uv::GlobalConfig::BufferModeStatus == uv::GlobalConfig::NoBuffer)
    {
        connection->write(buf, size, nullptr);
    } else {
        Packet packet;
		packet.pack(buf, size);
		connection->write(packet.Buffer().c_str(), packet.PacketSize(), nullptr);
    }
}

double BBU::cmp(const PAIR& x, const PAIR& y)
{
    return atof(x.second.c_str()) > atof(y.second.c_str());
}

void BBU::sortMapByValue(std::map<std::string, std::string>& map, vector<PAIR>& tVector)
{
    for(auto &it : map)
        tVector.push_back(make_pair(it.first, it.second));

    sort(tVector.begin(), tVector.end(), cmp);
}

void BBU::EchoSortResult(vector<PAIR>& tVector)
{
    for(int i = 0; i < static_cast<int>(tVector.size()); i++)
    {
        std::cout << tVector[i].first << ":" << tVector[i].second << std::endl;
    }

    std::cout << "Max Delay: " << tVector.begin()->second << std::endl;
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
    std::map<std::string, DeviceInfo> netTopology;
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


