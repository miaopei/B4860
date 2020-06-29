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
    setConnectCloseCallback(std::bind(&BBU::OnConnectClose, this, placeholders::_1));
#if 0
	FUN_T_PARA *para = new FUN_T_PARA;

	para->carrierIdx = 0;
	ipc_init_nxp(1, para);
    LOG_PRINT(LogLevel::debug, "main: carrierIdx[0] T14=%d", para->t14);
	m_carrierIdx_0_T14 = para->t14;
	
	para->carrierIdx = 1;
	ipc_init_nxp(1, para);
    LOG_PRINT(LogLevel::debug, "main: carrierIdx[1] T14=%d", para->t14);
	m_carrierIdx_1_T14 = para->t14;

	delete para;
#endif
	
    char* pdata = NULL;
    size_t size = 32;
    pdata = (char*)malloc(size * sizeof(char));
    if(pdata == NULL)
    {
        LOG_PRINT(LogLevel::error, "malloc memory error");
    }
    GetDeviceMAC(IFRNAME, pdata, size);
	LOG_PRINT(LogLevel::debug, "Device Mac: %s", pdata);

    m_mac       = pdata;
    m_source    = to_string(uv::Packet::BBU);
    m_hop       = "0";
    m_port      = "X";
    m_uport     = "X";
    m_uuport    = "X";
    
    free(pdata);
    pdata = NULL;
	CloseDeviceConnectFlag = false;
}

void BBU::OnConnectClose(shared_ptr<TcpConnection> connection)
{
	if(CloseDeviceConnectFlag)
	{
		CloseDeviceConnectFlag = false;
		return;
	}

    std::vector<TcpConnectionPtr> oamsConnection;
    GetOAMConnection(oamsConnection);
    for(auto it : oamsConnection)
    {
        if(it == connection)
            continue;

        uv::Packet::Head head;
        CreateHead(HeadType::B2A_HEAD, head);
        head.s_msgID        = to_string(uv::Packet::MSG_CONNECT_CLOSED);

        DeviceInfo dInfo;
        if(!GetDeviceInfo(connection, dInfo))
        {
            LOG_PRINT(LogLevel::error, "Get Device Info error");
            return;
        }

        std::string data = "Type=" + dInfo.s_source;
        data += "&Mac=" + dInfo.s_mac;
        data += "&Status=2";

	    LOG_PRINT(LogLevel::debug, "[SendMessage2Adapter: %s]", data.c_str());
        SendMessage2Adapter(head, data, data.length());
    }
}

void BBU::OnMessage(shared_ptr<TcpConnection> connection, const char* buf, ssize_t size)
{
	/* 数据包校验，数据粘包问题处理 */
	if(size < HEADLENGTH)
	{
		LOG_PRINT(LogLevel::error, "Message length error.");
		return ;
	}

	auto packetbuf = connection->getPacketBuffer();
    if (nullptr != packetbuf)
    {
        packetbuf->append(buf, static_cast<int>(size));
		uv::Packet packet;
        while (0 == packetbuf->readPacket(packet))
        {
			//LOG_PRINT(LogLevel::debug, "[ReceiveData: %d:%s]", packet.DataSize(), packet.getData());
			packet.UnPackMessage();

			/* 打印解包信息 */
			packet.EchoUnPackMessage();

			ProcessRecvMessage(connection, packet);
        }
    }
}

void BBU::ProcessRecvMessage(uv::TcpConnectionPtr connection, uv::Packet& packet)
{
	if(packet.GetDestination().empty())
	{
		LOG_PRINT(LogLevel::error, "Receive packet is broke. GetDestination is empty.");
		return ;
	}

	switch(std::stoi(packet.GetDestination()))
	{
		case uv::Packet::TO_BBU:
			BBUMessageProcess(connection, packet);
			break;
		case uv::Packet::TO_HUB:
            LOG_PRINT(LogLevel::debug, "Message Destination TO_HUB");
			HUBMessageProcess(connection, packet);
			break;
		case uv::Packet::TO_RRU:
            LOG_PRINT(LogLevel::debug, "Message Destination TO_RRU");
			RRUMessageProcess(connection, packet);
			break;
		case uv::Packet::TO_OAM:
            LOG_PRINT(LogLevel::debug, "Message Destination TO_OAM");
			OAMMessageProcess(connection, packet);
			break;
		default:
			LOG_PRINT(LogLevel::error, "[Destiantion: Error]");
            break;
	}
}

void BBU::BBUMessageProcess(uv::TcpConnectionPtr& connection, uv::Packet& packet)
{
	/* BBU 为消息处理中心，接收到的都是 request 消息*/
	switch(std::stoi(packet.GetMsgID()))
	{
		case uv::Packet::MSG_CONNECT:
			SetConnectionClient(connection, packet);
            break;
		case uv::Packet::MSG_DELAY_MEASUREMENT:
            LOG_PRINT(LogLevel::debug, "[MSG_DELAY_MEASUREMENT]");
			DelayMeasurementProcess(connection, packet);
			break;
        case uv::Packet::MSG_UPDATE_DELAY:
            LOG_PRINT(LogLevel::debug, "[MSG_UPDATE_DELAY]");
            UpdateHUBDelayInfo(packet);
            break;
        case uv::Packet::MSG_HEART_BEAT:
            //LOG_PRINT(LogLevel::debug, "[msg_heart_beat device:%s [HUB,RRU,BBU,OAM]]", packet.GetSource().c_str());
            break;
		default:
			LOG_PRINT(LogLevel::error, "[MessageID Error]");
            break;
	}
}

void BBU::HUBMessageProcess(uv::TcpConnectionPtr& connection, uv::Packet& packet)
{
    switch(std::stoi(packet.GetMsgID()))
	{
        case uv::Packet::MSG_SET:
            DevicesDataSetProcess(packet);
            break;
        case uv::Packet::MSG_UPGRADE:
            HUBUpgradeProcess(connection, packet);
            break;
		default:
			LOG_PRINT(LogLevel::error, "[MessageID Error]");
            break;
	}
}

void BBU::RRUMessageProcess(uv::TcpConnectionPtr& connection, uv::Packet& packet)
{
	switch(std::stoi(packet.GetMsgID()))
	{
        case uv::Packet::MSG_SET:
            DevicesDataSetProcess(packet);
            break;
        case uv::Packet::MSG_UPGRADE:
            RRUUpgradeProcess(connection, packet);
            break;
        case uv::Packet::MSG_RFTxStatus_SET:
            RRURFTxStatusProcess(connection, packet);
            break;
		default:
			LOG_PRINT(LogLevel::error, "[MessageID Error]");
            break;
	}
}

void BBU::OAMMessageProcess(uv::TcpConnectionPtr& connection, uv::Packet& packet)
{
    std::string send_buf;
    uv::Packet::Head head;
    CreateHead(HeadType::DEFAULT, head, packet);

	send_buf = "Type=" + packet.GetSource();
	send_buf += "&Mac=" + packet.GetMac();
    send_buf += "&" + packet.GetData();

	LOG_PRINT(LogLevel::debug, "[SendMessage2Adapter: %s]", send_buf.c_str());
    SendMessage2Adapter(head, send_buf, send_buf.length());
}

void BBU::HUBUpgradeProcess(uv::TcpConnectionPtr& connection, uv::Packet& packet)
{
    std::string routeIndex;
    uv::TcpConnectionPtr to_connect;
    std::string send_buf;
    std::string data;

    std::map<std::string, std::string> map;
	packet.SplitData2Map(map);
	if(!FindDataMapValue(map, "routeIndex", routeIndex))
    {
        std::vector<TcpConnectionPtr> hubsConnection;
        GetHUBsConnection(hubsConnection);
        for(auto it : hubsConnection)
        {	
            send_buf = packet.GetPacket();
	        LOG_PRINT(LogLevel::debug, "[SendMessage: %s]", send_buf.c_str());
            SendMessage(it, send_buf.c_str(), send_buf.length());
        }
        return ;
    }

    if(!GetConnectByRouteIndex(routeIndex, to_connect))
    {
		LOG_PRINT(LogLevel::error, "Get Connect by routeIndex error");
        return ;
    }
    std::string fileName;
    std::string md5;
    if(!FindDataMapValue(map, "fileName", fileName))
	{
		LOG_PRINT(LogLevel::error, "not find dataMapValue fileName");
		return ;
	}

    if(!FindDataMapValue(map, "md5", md5))
	{
		LOG_PRINT(LogLevel::error, "not find dataMapValue fileName");
		return ;
	}
    
    data = "fileName=" + fileName + "&md5=" + md5;
    packet.PackMessage(data, data.length());
    send_buf = packet.GetPacket();

	LOG_PRINT(LogLevel::debug, "[SendMessage: %s]", send_buf.c_str());
    SendMessage(to_connect, send_buf.c_str(), send_buf.length());
}

void BBU::RRUUpgradeProcess(uv::TcpConnectionPtr& connection, uv::Packet& packet)
{
    std::string routeIndex;
    uv::TcpConnectionPtr to_connect;
    std::string send_buf;
    std::string data;
    std::string fileName;
    std::map<std::string, std::string> map;
    
	packet.SplitData2Map(map);
    if(!FindDataMapValue(map, "fileName", fileName))
	{
		LOG_PRINT(LogLevel::error, "not find dataMapValue fileName");
		return ;
	}

    std::vector<std::string> files = GetFiles(std::string("/tmp/ftp/" + fileName));
    if(static_cast<int>(files.size()) != 3)
    {
		LOG_PRINT(LogLevel::error, "RRU upgrade Director file num error");
        return ;
    }
    data = "fileName=" + fileName + "/" + files[0] + "&md5=" + md5file(std::string("/tmp/ftp/" + fileName + "/" + files[0]).c_str()) + "&fileName=" + fileName + "/" + files[1] + "&md5=" + md5file(std::string("/tmp/ftp/" + fileName + "/" + files[1]).c_str()) + "&fileName=" + fileName + "/" + files[2] + "&md5=" + md5file(std::string("/tmp/ftp/" + fileName + "/" + files[2]).c_str());

	if(!FindDataMapValue(map, "routeIndex", routeIndex))
    {
        std::vector<TcpConnectionPtr> rrusConnection;
        GetRRUsConnection(rrusConnection);
        for(auto it : rrusConnection)
        {	
            packet.PackMessage(data, data.length());
            send_buf = packet.GetPacket();

	        LOG_PRINT(LogLevel::debug, "[SendMessage: %s]", send_buf.c_str());
            SendMessage(it, send_buf.c_str(), send_buf.length());
        }
        return ;
    }

    if(!GetConnectByRouteIndex(routeIndex, to_connect))
    {
		LOG_PRINT(LogLevel::error, "Get Connect by routeIndex error");
        return ;
    }
    
    packet.PackMessage(data, data.length());
    send_buf = packet.GetPacket();

	LOG_PRINT(LogLevel::debug, "[SendMessage: %s]", send_buf.c_str());
    SendMessage(to_connect, send_buf.c_str(), send_buf.length());
}

void BBU::RRURFTxStatusProcess(uv::TcpConnectionPtr& connection, uv::Packet& packet)
{
    std::string routeIndex;
    uv::TcpConnectionPtr to_connect;
    std::string send_buf;
    std::string data;

    std::map<std::string, std::string> map;
	packet.SplitData2Map(map);
	if(!FindDataMapValue(map, "routeIndex", routeIndex))
    {
		LOG_PRINT(LogLevel::debug, "Set All RRU Devices!");

        std::vector<TcpConnectionPtr> rrusConnection;
        GetRRUsConnection(rrusConnection);
        for(auto it : rrusConnection)
        {	
            send_buf = packet.GetPacket();
	        
            LOG_PRINT(LogLevel::debug, "[SendMessage: %s]", send_buf.c_str());
            SendMessage(it, send_buf.c_str(), send_buf.length());
        }
        return ;
    }

    if(!GetConnectByRouteIndex(routeIndex, to_connect))
    {
		LOG_PRINT(LogLevel::error, "Get Connect by routeIndex error");
        return ;
    }

    std::string RFTxStatus;
    if(!FindDataMapValue(map, "RFTxStatus", RFTxStatus))
	{
		LOG_PRINT(LogLevel::error, "not find dataMapValue RFTxStatus");
		return ;
	}

    data = "RFTxStatus=" + RFTxStatus;
    packet.PackMessage(data, data.length());
    send_buf = packet.GetPacket();

	LOG_PRINT(LogLevel::debug, "[SendMessage: %s]", send_buf.c_str());
    SendMessage(to_connect, send_buf.c_str(), send_buf.length());
}

void BBU::DevicesDataSetProcess(uv::Packet& packet)
{
    std::string routeIndex;
    std::vector<TcpConnectionPtr> DevicesConnection;
    std::string send_buf;
    uv::TcpConnectionPtr to_connect;
    std::string data;

    std::map<std::string, std::string> map;
	packet.SplitData2Map(map);
	if(!FindDataMapValue(map, "routeIndex", routeIndex))
    {
		LOG_PRINT(LogLevel::debug, "Set All Devices!");

        if(packet.GetDestination() == to_string(uv::Packet::TO_HUB))
        {
            DevicesConnection.clear();
            GetHUBsConnection(DevicesConnection);
            for(auto it : DevicesConnection)
            {	
                send_buf = packet.GetPacket();
	
                LOG_PRINT(LogLevel::debug, "[SendMessage: %s]", send_buf.c_str());
                SendMessage(it, send_buf.c_str(), send_buf.length());
            }

        } else if(packet.GetDestination() == to_string(uv::Packet::TO_RRU))
        {
            DevicesConnection.clear();
            GetRRUsConnection(DevicesConnection);
            for(auto it : DevicesConnection)
            {	
                send_buf = packet.GetPacket();
                
                LOG_PRINT(LogLevel::debug, "[SendMessage: %s]", send_buf.c_str());
                SendMessage(it, send_buf.c_str(), send_buf.length());
            }

        } else {
            LOG_PRINT(LogLevel::error, "Set All Devices!");
        }
        return ;
    }

    if(!GetConnectByRouteIndex(routeIndex, to_connect))
    {
		LOG_PRINT(LogLevel::error, "Get Connect by routeIndex error");
        return ;
    }
   
    data = packet.GetData();
    data = data.substr(data.find('&') + 1);
    packet.PackMessage(data, data.length());
    send_buf = packet.GetPacket();
    
    LOG_PRINT(LogLevel::debug, "[SendMessage: %s]", send_buf.c_str());
    SendMessage(to_connect, send_buf.c_str(), send_buf.length());
}

void BBU::SetConnectionClient(uv::TcpConnectionPtr& connection, uv::Packet& packet)
{
	DeviceInfo dInfo;
	dInfo.s_ip = GetCurrentName(connection);
	dInfo.s_connection = connection;
	dInfo.s_source = packet.GetSource();
	dInfo.s_mac = packet.GetMac();
	dInfo.s_hop = packet.GetHop();
	dInfo.s_port = packet.GetPort();
	dInfo.s_uport = packet.GetUPort();

	if(!SetConnectionInfo(connection, dInfo))
	{
		LOG_PRINT(LogLevel::error, "SetConnectionInfo error");
		return;
	}
    
    if(packet.GetSource() != to_string(uv::Packet::OAM))
    {
        if(!SetDeviceRouteIndex(connection))
        {
            LOG_PRINT(LogLevel::error, "Set Device RouteIndex error");
			/* close the device connect */
			std::string name = GetCurrentName(connection);
			CloseDeviceConnectFlag = true;
			closeConnection(name);
            return ;
        }
    }
	
    std::vector<TcpConnectionPtr> oamsConnection;
    GetOAMConnection(oamsConnection);
    for(auto it : oamsConnection)
    {
        if(it != connection)
        {
            if(!GetDeviceInfo(connection, dInfo))
            {
                LOG_PRINT(LogLevel::error, "Get Device Info error");
                return;
            }
            //SendConnectToOamAdapter(it, packet, dInfo);
            uv::Packet::Head head;
            CreateHead(HeadType::D2A_HEAD, head, packet);
            head.s_msgID        = to_string(uv::Packet::MSG_NEW_CONNECT);

            std::string data = "IpAddress=" + GetCurrentName(connection);
            data += "&Mac=" + packet.GetMac();
            data += "&Type=" + packet.GetSource();
            data += "&HOP=" + packet.GetHop();
            data += "&RouteIndex=" + dInfo.s_routeIndex;
            data += "&" + packet.GetData();

	        LOG_PRINT(LogLevel::debug, "[SendMessage2Adapter: %s]", data.c_str());
            SendMessage2Adapter(head, data, data.length());
        }
    }

	/* 如果 source 是 RRU 需要更新上级 HUB 延时测量信息*/
    if(dInfo.s_source == to_string(uv::Packet::RRU))
    {
        SendUpdateHUBDelayMessage(connection, packet);
    }

    /* 需要优化，使用同一的消息发送接口 */
    SendConnectionMessage(connection, packet);
}

void BBU::DelayMeasurementProcess(uv::TcpConnectionPtr& connection, uv::Packet& packet)
{
    switch(std::stoi(packet.GetSource()))
    {
        case uv::Packet::HUB:
		    LOG_PRINT(LogLevel::debug, "[hub_delay_measurement]");
			HubDelayInfo(packet);
            break;
        case uv::Packet::RRU:
		    LOG_PRINT(LogLevel::debug, "[rru_delay_measurement]");
            RruDelayProcess(connection, packet);
            break;
        default:
		    LOG_PRINT(LogLevel::error, "delay measurement source error");
    }
}

void BBU::UpdateHUBDelayInfo(uv::Packet& packet)
{
    UpdateDelayInfo(packet.GetData(), packet.GetHop(), delay_map);
	
#if 1
    for(auto &it : delay_map)
    {
		LOG_PRINT(LogLevel::debug, "key=%s key.key=%s key.value=%s", 
                                    it.first.c_str(),
                                    it.second.key.c_str(),
                                    it.second.value.c_str());
    }
#endif
}

void BBU::HubDelayInfo(uv::Packet& packet)
{
	std::string data = packet.GetData();
    SplitStrings2Map(data, packet.GetHop(), delay_map);
    
#if 1
    for(auto &it : delay_map)
    {
        LOG_PRINT(LogLevel::debug, "key=%s key.key=%s key.value=%s", 
                                    it.first.c_str(),
                                    it.second.key.c_str(),
                                    it.second.value.c_str());
    }
#endif
}

void BBU::RruDelayProcess(uv::TcpConnectionPtr& connection, uv::Packet& packet)
{
    std::string delayULCompensation;
    std::string delayDLCompensation;

	if(!SaveRRUDelayInfo(connection, packet))
	{
		LOG_PRINT(LogLevel::error, "Save RRU Delay Info error");
		return ;
	}
	
	CalculationDelayCompensation(connection, delayULCompensation, delayDLCompensation);
	
    uv::Packet::Head head;
    CreateHead(HeadType::S2D_REVERSAL_HEAD, head, packet);
    head.s_msgID        = to_string(uv::Packet::MSG_DELAY_COMPENSATION);

	std::string data = "DelayULCompensation=" + delayULCompensation + "&DelayDLCompensation=" + delayDLCompensation;

    LOG_PRINT(LogLevel::debug, "[SendPackMessage: %s]", data.c_str());
    SendPackMessage(connection, head, data, data.length());
}

bool BBU::SaveRRUDelayInfo(uv::TcpConnectionPtr& connection, uv::Packet& packet)
{
	RRUDelayInfo_T rruDelayInfo;
	std::map<std::string, std::string> map;
	packet.SplitData2Map(map);

	if(!FindDataMapValue(map, "T2a", rruDelayInfo.T2a))
	{
		LOG_PRINT(LogLevel::error, "FindDataMapValue T2a error");
		return false;
	}

	if(!FindDataMapValue(map, "Ta3", rruDelayInfo.Ta3))
	{
		LOG_PRINT(LogLevel::error, "FindDataMapValue Ta3 error");
		return false;
	}
	
	if(!SetRRUDeviceDelayInfo(connection, rruDelayInfo))
	{
		LOG_PRINT(LogLevel::error, "Set RRU Device Delay Info error");
		return false;
	}
	return true;
}

bool BBU::QueryUhubConnection(uv::TcpConnectionPtr& connection, uv::TcpConnectionPtr& reconnection)
{
	DeviceInfo upHubDInfo;

	if(!FindUpHubDeviceInfo(connection, upHubDInfo))
	{
		LOG_PRINT(LogLevel::error, "Get Next Device Info error");
		return false;
	}
	reconnection = upHubDInfo.s_connection;
    return true;
}

bool BBU::CalculationDelayCompensation(uv::TcpConnectionPtr& connection, std::string& delayiULCompensation, std::string& delayiDLCompensation)
{
    /* TotalDL = T12 + TBdelayDL + T12' + T2a
     * TotalUL = T12 + TBdelayUL + T12' + Ta3
     *
     * T12 = (T14 -Toffset) / 2  
     * T14 为 BBU 到 HUB 一个回环的延时值，Toffset 为 HUB 内口时延
     *
     * TBdelay: hub 上联口到级联口，上联口到下联口的延时值
     *
     * T12' = (T14' - Toffset') / 2  
     * T12' 为 HUB 到 RRU 一个回环的延时值，Toffset' 为 RRU 内口时延
     * 
     */
	int level = 0;
	std::string key;
	std::string tbdelayDL;
	std::string tbdelayUL;
	std::string t14;
	int totalDL;
	int totalUL;
	int totalDLHUBDelay;
	int totalULHUBDelay;
	int t12;
	DeviceInfo dInfo;

	std::map<std::string, std::string> mDelayDL;
    std::map<std::string, std::string> mDelayUL;

	if(!GetDeviceInfo(connection, dInfo))
	{
		LOG_PRINT(LogLevel::error, "Get Device Info error");
		return false;
	}
	//LOG_PRINT(LogLevel::debug, "RouteIndex=%s", dInfo.s_routeIndex.c_str());

	level = std::stoi(dInfo.s_hop) - 1;
	if(level <= 0)
	{
		LOG_PRINT(LogLevel::error, "level error");
		return false;
	} 

	/* 计算 rru 上一级 hub delay */
	/* TBdelay DL */
	key = std::string(to_string(level) + "1" + dInfo.s_uport);
	if(!FindDelayMapValue(key, tbdelayDL))
	{
		LOG_PRINT(LogLevel::error, "FindDelayMapValue tbdelayDL error");
		return false;
	}

	/* TBdelay UL */
	key = std::string(to_string(level) + "2" + dInfo.s_uport);
	if(!FindDelayMapValue(key, tbdelayUL))
	{
		LOG_PRINT(LogLevel::error, "FindDelayMapValue tbdelayUL error");
		return false;
	}

	/* T14 */
	key = std::string(to_string(level) + "3" + dInfo.s_uport);
	if(!FindDelayMapValue(key, t14))
	{
		LOG_PRINT(LogLevel::error, "FindDelayMapValue t14 error");
		return false;
	}

	totalDLHUBDelay = stoi(tbdelayDL.c_str()) + ((stoi(t14.c_str()) - (RRUToffset)) / 2);
	totalULHUBDelay = stoi(tbdelayUL.c_str()) + ((stoi(t14.c_str()) - (RRUToffset)) / 2);

	/* 级联 hub delay 计算，级联的情况 hub port=0 uport=1 */
	level = level - 1;
	if(level >= 1)
	{
		for(level = level; level > 0; level--)
		{
			/* TBdelay DL */
			key = std::string(to_string(level) + "1" + to_string(m_base - 1));  // 最后一位为级联口
			if(!FindDelayMapValue(key, tbdelayDL))
			{
		        LOG_PRINT(LogLevel::error, "FindDelayMapValue tbdelayDL error");
				return false;
			}
			totalDLHUBDelay += stoi(tbdelayDL.c_str());

			/* TBdelay UL */
			key = std::string(to_string(level) + "2" + to_string(m_base - 1));
			if(!FindDelayMapValue(key, tbdelayUL))
			{
		        LOG_PRINT(LogLevel::error, "FindDelayMapValue tbdelayUL error");
				return false;
			}
			totalULHUBDelay += stoi(tbdelayUL.c_str());

			/* T14 */
			key = std::string(to_string(level) + "3" + to_string(m_base - 1));
			if(!FindDelayMapValue(key, t14))
			{
		        LOG_PRINT(LogLevel::error, "FindDelayMapValue t14 error");
				return false;
			}
			totalDLHUBDelay += stoi(tbdelayDL.c_str()) + ((stoi(t14.c_str()) - (HUBCascadeEToffset)) / 2);
			totalULHUBDelay += stoi(tbdelayUL.c_str()) + ((stoi(t14.c_str()) - (HUBCascadeEToffset)) / 2);
		}
	}

	t12 = ((BBUT14 - (HUBToffset * TOFFSETCYCLE)) / 2) / TOFFSETCYCLE;
	totalDL = t12 + totalDLHUBDelay + stoi(dInfo.s_rruDelayInfo.T2a.c_str());
	totalUL = t12 + totalULHUBDelay + stoi(dInfo.s_rruDelayInfo.Ta3.c_str());
	LOG_PRINT(LogLevel::debug, "totalDL=%d", totalDL);
	LOG_PRINT(LogLevel::debug, "totalUL=%d", totalUL);

    /* store delay offset value */
    RRUDelayOffset_T rruDelayOffset;
    rruDelayOffset.offsetDL = to_string(totalDL);
    rruDelayOffset.offsetUL = to_string(totalUL);
    if(!SetRRUDeviceDelayOffset(connection, rruDelayOffset))
    {
		LOG_PRINT(LogLevel::error, "Set RRU Device Delay Offset value error");
    }

	DeleteRRUTotalDelay(connection, tVectorDL);
	DeleteRRUTotalDelay(connection, tVectorDL);
	
    mDelayDL.insert(make_pair(dInfo.s_routeIndex, to_string(totalDL)));
    mDelayUL.insert(make_pair(dInfo.s_routeIndex, to_string(totalUL)));

    sortMapByValue(mDelayDL, tVectorDL);
    sortMapByValue(mDelayUL, tVectorUL);

	LOG_PRINT(LogLevel::debug, "Echo maxDelayDL Result: ");
    EchoSortResult(tVectorDL);

	LOG_PRINT(LogLevel::debug, "Echo maxDelayUL Result: ");
    EchoSortResult(tVectorUL);

	int maxDLDelay = stoi(tVectorDL.begin()->second.c_str());
	int maxULDelay = stoi(tVectorUL.begin()->second.c_str());
	LOG_PRINT(LogLevel::debug, "MaxDLDelay=%d MaxULDelay=%d", maxDLDelay, maxULDelay);

    /* if totalDL == maxDLDelay, update other connect RU Offset value */
    if(totalDL == maxDLDelay)
    {
        UpdataRRUDelayOffsetValue(connection, maxDLDelay, maxULDelay);
    }

    //delayiDLCompensation = to_string(stoi(tVectorDL.begin()->second.c_str()) - totalDL);
    //delayiULCompensation = to_string(stoi(tVectorUL.begin()->second.c_str()) - totalUL);
    delayiDLCompensation = to_string(maxDLDelay - totalDL);
    delayiULCompensation = to_string(maxULDelay - totalUL);

	return true;
}

void BBU::UpdataRRUDelayOffsetValue(uv::TcpConnectionPtr& connection, int maxDLDelay, int maxULDelay)
{
    DeviceInfo dInfo;
    uv::Packet::Head head;
    std::string delayULCompensation;
    std::string delayDLCompensation;
    std::vector<TcpConnectionPtr> rrusConnection;
    GetRRUsConnection(rrusConnection);
    for(auto it : rrusConnection)
    {	
        if(it == connection) continue;
        
        if(!GetDeviceInfo(it, dInfo))
        {
            LOG_PRINT(LogLevel::error, "Get Device Info error");
            return;
        }

        CreateHead(HeadType::B2R_HEAD, head);
        head.s_msgID        = to_string(uv::Packet::MSG_DELAY_COMPENSATION);
        
        delayULCompensation = to_string(maxULDelay - stoi(dInfo.s_rruDelayOffset.offsetUL));
        delayDLCompensation = to_string(maxDLDelay - stoi(dInfo.s_rruDelayOffset.offsetDL));
        std::string data = "DelayULCompensation=" + delayULCompensation + "&DelayDLCompensation=" + delayDLCompensation;

        LOG_PRINT(LogLevel::debug, "[SendPackMessage: %s]", data.c_str());
        SendPackMessage(it, head, data, data.length());
    }
}

bool BBU::FindDelayMapValue(std::string key, std::string& value)
{
	auto rst = delay_map.find(key);
	if(rst == delay_map.end())
	{
	    LOG_PRINT(LogLevel::error, "not find delay_map key");
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
		return false;
	}
	value = rst->second;
	return true;
}

int BBU::_system(std::string command)
{
    pid_t status;
    status = system(command.c_str());

    if(-1 == status){
	    LOG_PRINT(LogLevel::error, "system error!");
        return -1;
    } else {
        if(WIFEXITED(status)){
            if(0 == WEXITSTATUS(status)){
                return 0;
            } else {
	            LOG_PRINT(LogLevel::error, "run shell script fail, script exit code: %d", WEXITSTATUS(status));
                return -2;
            }
        } else {
	        LOG_PRINT(LogLevel::error, "exit status: %d", WEXITSTATUS(status));
            return -3;
        }
    }
    return 0;
}

bool BBU::write_file(std::string file, const std::string& data)
{
	std::ofstream fout;
	fout.open(file);
	if(!fout.is_open())
	{
	    LOG_PRINT(LogLevel::error, "open file error");
		return false;
	}
	fout << data << std::endl; 
	fout.close();
	return true;
}

bool BBU::read_file(std::string file, char* data, ssize_t size)
{
	std::ifstream fin;	
	
	fin.open(file);
	if(!fin.is_open())
	{
	    LOG_PRINT(LogLevel::error, "open file error");
		return false;
	}
	fin.getline(data, size);
	fin.close();
	return true;
}

std::vector<std::string> BBU::GetFiles(std::string cate_dir)
{
    std::vector<std::string> files;
    DIR *dir;
    struct dirent *ptr;

    if ((dir = opendir(cate_dir.c_str())) == NULL)
    {
	    LOG_PRINT(LogLevel::error, "open dir error");
        return files;
    }

    while ((ptr = readdir(dir)) != NULL)
    {
        if(strcmp(ptr->d_name,".") == 0 || strcmp(ptr->d_name,"..") == 0){
            continue;
        } else if(ptr->d_type == 8){
            files.push_back(ptr->d_name);
        } else if(ptr->d_type == 10){
            continue;
        } else if(ptr->d_type == 4){
            files.push_back(ptr->d_name);
            /*
            memset(base,'\0',sizeof(base));
            strcpy(base,basePath);
            strcat(base,"/");
            strcat(base,ptr->d_nSame);
            readFileList(base);
            */
        }
    }
    closedir(dir);

    //排序，按从小到大排序
    sort(files.begin(), files.end());
    return files;
}

void BBU::CreateHead(HeadType hType, uv::Packet::Head& head)
{
    switch(hType)
    {
        case HeadType::B2A_HEAD:
            {
                head.s_destination = to_string(uv::Packet::Destination::TO_OAM);
            }
            break;
        case HeadType::B2H_HEAD:
            {
                head.s_destination = to_string(uv::Packet::Destination::TO_HUB);
            }
            break;
        case HeadType::B2R_HEAD:
            {
                head.s_destination = to_string(uv::Packet::Destination::TO_RRU);
            }
            break;
        default:
            {
                LOG_PRINT(LogLevel::error, "DeviceType error");
                head.s_destination = "X";
            }
            break;
    }

    head.s_source       = m_source;
	head.s_mac          = m_mac;
    head.s_state        = to_string(uv::Packet::RESPONSE);
    head.s_hop          = m_hop;
    head.s_port         = m_port;
    head.s_uport        = m_uport;
    head.s_uuport       = m_uuport;
}

void BBU::CreateHead(HeadType hType, uv::Packet::Head& head, uv::Packet& packet)
{
    switch(hType)
    {
        case HeadType::S2D_REVERSAL_HEAD:
            {
                head.s_source       = packet.GetDestination();
                head.s_destination  = packet.GetSource();
                head.s_msgID        = packet.GetMsgID();
            }
            break;
        case HeadType::D2A_HEAD:
            {
                head.s_source       = packet.GetSource();
                head.s_destination  = to_string(uv::Packet::TO_OAM);
            }
            break;
        case HeadType::B2H_HEAD:
            {
                head.s_source       = packet.GetSource();
                head.s_destination  = to_string(uv::Packet::TO_HUB);
            }
            break;
        case HeadType::DEFAULT:
            {
                head.s_source       = packet.GetSource();
                head.s_destination  = packet.GetDestination();
                head.s_msgID        = packet.GetMsgID();
            }
            break;
        default:
            {
                LOG_PRINT(LogLevel::error, "DeviceType error");
                head.s_source       = "X";
                head.s_destination  = "X";
            }
            break;
    }

	head.s_mac          = packet.GetMac();
    head.s_state        = to_string(uv::Packet::RESPONSE);
    head.s_hop          = packet.GetHop();
    head.s_port         = packet.GetPort();
    head.s_uport        = packet.GetUPort();
    head.s_uuport       = packet.GetUUPort();
}

void BBU::SendMessage(shared_ptr<TcpConnection> connection, const char* buf, ssize_t size)
{
    if(uv::GlobalConfig::BufferModeStatus == uv::GlobalConfig::NoBuffer)
    {
        connection->write(buf, size, nullptr);
    } else {
        Packet packet;
		packet.pack(buf, size);
		connection->write(packet.Buffer().c_str(), packet.PacketSize(), nullptr);
    }
}

void BBU::SendConnectionMessage(uv::TcpConnectionPtr& connection, uv::Packet& packet)
{
    uv::Packet::Head head;
    CreateHead(HeadType::S2D_REVERSAL_HEAD, head, packet);

	std::string data = "ConnectResult=0";

    SendPackMessage(connection, head, data, data.length());
}

void BBU::SendPackMessage(uv::TcpConnectionPtr& connection, uv::Packet::Head head, std::string& data, ssize_t size)
{
    uv::Packet Packet;
    Packet.SetHead(head);

    Packet.PackMessage(data, size);

	/* 打印数据封装信息 */
	//Packet.EchoPackMessage();

	std::string send_buf = Packet.GetPacket();

	SendMessage(connection, send_buf.c_str(), send_buf.length());
}

void BBU::SendMessage2Adapter(uv::Packet::Head head, std::string& data, ssize_t size)
{
    std::vector<TcpConnectionPtr> oamsConnection;
    GetOAMConnection(oamsConnection);
    for(auto it : oamsConnection)
    {
        SendPackMessage(it, head, data, size);
    }
}

void BBU::SendPackMessageToAllDevice(DeviceType device, uv::Packet::Head head, std::string& data, ssize_t size)
{
    if(device == ALL_HUB_DEVICE)
    switch(device)
    {
        case ALL_HUB_DEVICE:
		    LOG_PRINT(LogLevel::debug, "[SendPackMessageToAllDevice: ALL_HUB_DEVICE]");
            break;
        case ALL_RRU_DEVICE:
		    LOG_PRINT(LogLevel::debug, "[SendPackMessageToAllDevice: ALL_RRU_DEVICE]");
            break;
        default:
		    LOG_PRINT(LogLevel::error, "SendPackMessageToAllDevice device error");
    }
}

void BBU::SendUpdateHUBDelayMessage(uv::TcpConnectionPtr& connection, uv::Packet& packet)
{
    shared_ptr<TcpConnection> reconnection;
    if(!QueryUhubConnection(connection, reconnection))
    {
		LOG_PRINT(LogLevel::error, "SendUpdateHUBDelayMessage not find hub rruid");
        return;
    }
    
    /* 封装消息，指定 HUB 更新时延测量 */
    uv::Packet::Head head;
    CreateHead(HeadType::B2H_HEAD, head, packet);
    head.s_state        = to_string(uv::Packet::REQUEST);
    head.s_msgID        = to_string(uv::Packet::MSG_UPDATE_DELAY);

    std::string data = "updataDelayInfo";

    LOG_PRINT(LogLevel::debug, "[SendPackMessage: %s]", data.c_str());
    SendPackMessage(reconnection, head, data, data.length());
}













void BBU::EchoSortResult(vector<PAIR>& tVector)
{
    for(int i = 0; i < static_cast<int>(tVector.size()); i++)
    {
	    LOG_PRINT(LogLevel::debug, "%s: %s", tVector[i].first.c_str(), tVector[i].second.c_str());
    }

	LOG_PRINT(LogLevel::debug, "Max Delay: %s", tVector.begin()->second.c_str());
}

