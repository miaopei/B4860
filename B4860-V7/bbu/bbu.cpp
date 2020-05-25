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
    LOG_PRINT(LogLevel::debug, "main: carrierIdx[0] T14=%s", para->t14);
	m_carrierIdx_0_T14 = para->t14;
	
	para->carrierIdx = 1;
	ipc_init_nxp(1, para);
    LOG_PRINT(LogLevel::debug, "main: carrierIdx[1] T14=%s", para->t14);
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
    m_uport     = "X";
    m_uuport    = "X";
    
    free(pdata);
    pdata = NULL;
}

void BBU::OnConnectClose(shared_ptr<TcpConnection> connection)
{
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

        std::string data = "ip=" + dInfo.s_ip;
        data += "&mac=" + dInfo.s_mac;
        data += "&source=" + dInfo.s_source;
        data += "&hop=" + dInfo.s_hop;
        data += "&routeIndex=" + dInfo.s_routeIndex;
        data += "&upgradeResult=" + dInfo.s_upgradeState;

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

#if 0
            BHRO_T_PACKET *bhro_packet = (BHRO_T_PACKET*)malloc(packet.DataSize());
            memcpy(bhro_packet, packet.getData(), packet.DataSize());
            LOG_PRINT(LogLevel::debug, "source=%d destination=%d len=%d",
                                        bhro_packet->packet_head.source,
                                        bhro_packet->packet_head.destination,
                                        bhro_packet->packet_head.len);

            //BHRO_T_CONNECT_REQ *connect_req = (BHRO_T_CONNECT_REQ*)malloc(bhro_packet->packet_head.len + 1);
            //memcpy(connect_req, bhro_packet->tlv_data, bhro_packet->packet_head.len + 1);
            //LOG_PRINT(LogLevel::debug, "data=%d", connect_req->resultID);
            LOG_PRINT(LogLevel::debug, "data=%d", ((BHRO_T_CONNECT_REQ*)bhro_packet->tlv_data)->resultID);
            
            //free(connect_req);
            //connect_req = NULL;

            free(bhro_packet);
            bhro_packet = NULL;
#endif
#if 1
			packet.UnPackMessage();

			/* 打印解包信息 */
			packet.EchoUnPackMessage();

			ProcessRecvMessage(connection, packet);
#endif
        }
    }
}

void BBU::ProcessRecvMessage(uv::TcpConnectionPtr connection, uv::Packet& packet)
{
	switch(std::stoi(packet.GetDestination()))
	{
		case uv::Packet::TO_BBU:
			BBUMessageProcess(connection, packet);
			break;
		case uv::Packet::TO_HUB:
			HUBMessageProcess(connection, packet);
			break;
		case uv::Packet::TO_RRU:
			RRUMessageProcess(connection, packet);
			break;
		case uv::Packet::TO_OAM:
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
        case uv::Packet::MSG_UPGRADE:
            UpgradeResultProcess(connection, packet);
            break;
        case uv::Packet::MSG_GET_NETWORK_TOPOLOGY:
            NetworkTopologyMessageProcess(connection, packet);
            break;
		case uv::Packet::MSG_DELAY_MEASUREMENT:
			DelayMeasurementProcess(connection, packet);
			break;
        case uv::Packet::MSG_UPDATE_DELAY:
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
            /* 设置升级标志为真正升级 
             * 0 - 非升级状态
             * 1 - 升级成功状态
             * 2 - 升级失败状态
             * 3 - 正在升级状态
             * */
            if(!SetDeviceInfo(it, "upgradeState", "3"))
            {
				LOG_PRINT(LogLevel::error, "Set Device upgradeState error");
                return ;
            }

            send_buf = packet.GetPacket();
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

    /* 设置升级标志为真正升级 
     * 0 - 非升级状态
     * 1 - 升级成功状态
     * 2 - 升级失败状态
     * 3 - 正在升级状态
     * */
    if(!SetDeviceInfo(to_connect, "upgradeState", "3"))
    {
		LOG_PRINT(LogLevel::error, "Set Device upgradeState error");
        return ;
    }

    data = "fileName=" + fileName + "&md5=" + md5;
    packet.PackMessage(data, data.length());
    send_buf = packet.GetPacket();
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
    data = "fileName=" + fileName + "/" + files[0] + "&md5=" + md5file(std::string("/tmp/ftp/" + fileName + "/" + files[0]).c_str()) + "&fileName=" + fileName + "/" + files[1] + "&md5=" + md5file(std::string("/tmp/ftp/" + fileName + "/" + files[1]).c_str()) + "&fileName=" + fileName + "/" + files[2] + "&md5=" + md5file(std::string("/mnt/ftp/" + fileName + "/" + files[2]).c_str());

	if(!FindDataMapValue(map, "routeIndex", routeIndex))
    {
        std::vector<TcpConnectionPtr> rrusConnection;
        GetRRUsConnection(rrusConnection);
        for(auto it : rrusConnection)
        {	
            /* 设置升级标志为真正升级 
             * 0 - 非升级状态
             * 1 - 升级成功状态
             * 2 - 升级失败状态
             * 3 - 正在升级状态
             * */
            if(!SetDeviceInfo(it, "upgradeState", "3"))
            {
				LOG_PRINT(LogLevel::error, "Set Device upgradeState error");
                return ;
            }

            packet.PackMessage(data, data.length());
            send_buf = packet.GetPacket();
            SendMessage(it, send_buf.c_str(), send_buf.length());
        }
        return ;
    }

    if(!GetConnectByRouteIndex(routeIndex, to_connect))
    {
		LOG_PRINT(LogLevel::error, "Get Connect by routeIndex error");
        return ;
    }
    
    /* 设置升级标志为真正升级 
     * 0 - 非升级状态
     * 1 - 升级成功状态
     * 2 - 升级失败状态
     * 3 - 正在升级状态
     * */
    if(!SetDeviceInfo(to_connect, "upgradeState", "3"))
    {
		LOG_PRINT(LogLevel::error, "Set Device upgradeState error");
        return ;
    }

    packet.PackMessage(data, data.length());
    send_buf = packet.GetPacket();
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
		LOG_PRINT(LogLevel::error, "not find dataMapValue routeIndex");

        std::vector<TcpConnectionPtr> rrusConnection;
        GetRRUsConnection(rrusConnection);
        for(auto it : rrusConnection)
        {	
            send_buf = packet.GetPacket();
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
    SendMessage(to_connect, send_buf.c_str(), send_buf.length());
}

void BBU::UpgradeResultProcess(uv::TcpConnectionPtr& connection, uv::Packet& packet)
{
    if(!WriteUpgradeResultToDevice(connection, packet))
    {
		LOG_PRINT(LogLevel::error, "WriteUpgradeResultToDevice error");
		return ;
    }
}

void BBU::NetworkTopologyMessageProcess(uv::TcpConnectionPtr& connection, uv::Packet& packet)
{
    uv::Packet::Head head;
    CreateHead(HeadType::S2D_REVERSAL_HEAD, head, packet);
	
    std::string data = "";

    std::map<std::string, DeviceInfo> netTopology;
    GetNetworkTopology(netTopology);

    for(auto &it : netTopology)
    {
        if(it.second.s_source == to_string(uv::Packet::OAM))
            continue;

        data += "ip=" + it.second.s_ip + "&mac=" + it.second.s_mac + "&source=" + it.second.s_source + "&hop=" + it.second.s_hop + "&upgradeState=" + it.second.s_upgradeState + "&routeIndex=" + it.second.s_routeIndex + "#";
    } 

    SendPackMessage(connection, head, data, data.length());
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

    /* 如果 source 是 RRU 需要更新上级 HUB 延时测量信息*/
    if(dInfo.s_source == to_string(uv::Packet::RRU))
    {
        SendUpdateHUBDelayMessage(connection, packet);
    }

	/* Version Check */
	/* TODO*/

    /* upgrade state check */
    if(!WriteUpgradeResultToDevice(connection, packet))
    {
		LOG_PRINT(LogLevel::error, "WriteUpgradeResultToDevice error");
		return;
    }

    if(packet.GetSource() != to_string(uv::Packet::OAM))
    {
        if(!SetDeviceRouteIndex(connection))
        {
            LOG_PRINT(LogLevel::error, "Set Device RouteIndex error");
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

            std::string data = "ip=" + GetCurrentName(connection);
            data += "&mac=" + packet.GetMac();
            data += "&source=" + packet.GetSource();
            data += "&hop=" + packet.GetHop();
            data += "&routeIndex=" + dInfo.s_routeIndex;
            data += "&upgradeResult=" + dInfo.s_upgradeState;
            data += "&softwareVersion=" + dInfo.s_softwareVersion;

            SendMessage2Adapter(head, data, data.length());
        }
    }

    /* 需要优化，使用同一的消息发送接口 */
    SendConnectionMessage(connection, packet);
}

bool BBU::WriteUpgradeResultToDevice(uv::TcpConnectionPtr& connection, uv::Packet& packet)
{
    std::string resultID;
    std::string softwareVersion;
    std::map<std::string, std::string> map;
	packet.SplitData2Map(map);
	if(!FindDataMapValue(map, "ResultID", resultID))
    {
		LOG_PRINT(LogLevel::error, "resultID not find");
        return false;
    }

    if(!SetDeviceInfo(connection, "upgradeState", resultID))
    {
		LOG_PRINT(LogLevel::error, "Set Device upgradeState error");
        return false;
    }

    if(!FindDataMapValue(map, "softwareVersion", softwareVersion))
    {
		LOG_PRINT(LogLevel::error, "softwareVersion not find");
        return false;
    }

    if(!SetDeviceInfo(connection, "softwareVersion", softwareVersion))
    {
		LOG_PRINT(LogLevel::error, "Set Device softwareVersion error");
        return false;
    }

    return true;
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


void BBU::UnPackData(uv::Packet& packet, std::map<std::string, std::string>& map)
{
	/* Json 存储数据 jsoncpp，考虑同时支持 xml 存储数据 */
	//packet.SplitData2Map(map);
}


void BBU::UpdateHUBDelayInfo(uv::Packet& packet)
{
    UpdateDelayInfo(packet.GetData(), packet.GetHop(), delay_map);
	
#if 0
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
    
#if 0
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
	
	NetworkTopology();
	
	CalculationDelayCompensation(connection, delayULCompensation, delayDLCompensation);
	
	/* TODO：RRU 接入后其他的 RRU 时延补偿需要更新，有可能新接入的 RRU 时延是最大的 */
	/* 疑问：一个 loop */
	//UpdataRRUDelayCompensation(connection, packet);

    uv::Packet::Head head;
    CreateHead(HeadType::S2D_REVERSAL_HEAD, head, packet);
    head.s_msgID        = to_string(uv::Packet::MSG_DELAY_COMPENSATION);

	std::string data = "delayULCompensation=" + delayULCompensation + "&delayDLCompensation=" + delayDLCompensation;

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

void BBU::UpdataRRUDelayCompensation(uv::TcpConnectionPtr& connection, uv::Packet& packet)
{
	std::string delayULCompensation;
    std::string delayDLCompensation;
	std::string data;
	std::vector<TcpConnectionPtr> rrusConnection;
	std::string send_buf;

	uv::Packet::Head head;
    CreateHead(HeadType::S2D_REVERSAL_HEAD, head, packet);
    head.s_msgID        = to_string(uv::Packet::MSG_DELAY_COMPENSATION);

	GetRRUsConnection(rrusConnection);

	for(auto it : rrusConnection)
	{	
		if(it == connection)
		{
		    LOG_PRINT(LogLevel::debug, "conntion=%s", connection);
			continue;
		}
		
		LOG_PRINT(LogLevel::debug, "it=%s connection=%s", it, connection);
		
		CalculationDelayCompensation(it, delayULCompensation, delayDLCompensation);
	    data = "delayULCompensation=" + delayULCompensation + "&delayDLCompensation=" + delayDLCompensation;
		SendPackMessage(it, head, data, data.length());
	}
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
	LOG_PRINT(LogLevel::debug, "RouteIndex=%s", dInfo.s_routeIndex.c_str());

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
			key = std::string(to_string(level) + "1" + "1");  // 最后一位为级联口
			if(!FindDelayMapValue(key, tbdelayDL))
			{
		        LOG_PRINT(LogLevel::error, "FindDelayMapValue tbdelayDL error");
				return false;
			}
			totalDLHUBDelay += stoi(tbdelayDL.c_str());

			/* TBdelay UL */
			key = std::string(to_string(level) + "2" + "1");
			if(!FindDelayMapValue(key, tbdelayUL))
			{
		        LOG_PRINT(LogLevel::error, "FindDelayMapValue tbdelayUL error");
				return false;
			}
			totalULHUBDelay += stoi(tbdelayUL.c_str());

			/* T14 */
			key = std::string(to_string(level) + "3" + "1");
			if(!FindDelayMapValue(key, t14))
			{
		        LOG_PRINT(LogLevel::error, "FindDelayMapValue t14 error");
				return false;
			}
			totalDLHUBDelay += ((stoi(t14.c_str()) - (RRUToffset)) / 2);
			totalULHUBDelay += ((stoi(t14.c_str()) - (RRUToffset)) / 2);
		}
	}

	t12 = ((BBUT14 - (HUBCascadeEToffset * TOFFSETCYCLE)) / 2) / TOFFSETCYCLE;
	totalDL = t12 + totalDLHUBDelay + stoi(dInfo.s_rruDelayInfo.T2a.c_str());
	totalUL = t12 + totalULHUBDelay + stoi(dInfo.s_rruDelayInfo.Ta3.c_str());
	LOG_PRINT(LogLevel::debug, "totalDL=%d", totalDL);
	LOG_PRINT(LogLevel::debug, "totalUL=%d", totalUL);

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
	LOG_PRINT(LogLevel::debug, "maxDLDelay=%d maxULDelay=%d", maxDLDelay, maxULDelay);

    delayiDLCompensation = to_string(stoi(tVectorDL.begin()->second.c_str()) - totalDL);
    delayiULCompensation = to_string(stoi(tVectorUL.begin()->second.c_str()) - totalUL);

	return true;
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
	    LOG_PRINT(LogLevel::error, "not find dataMap key");
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
        if(strcmp(ptr->d_name,".") == 0 || strcmp(ptr->d_name,"..") == 0){    ///current dir OR parrent dir
            continue;
        } else if(ptr->d_type == 8){    ///file
            //printf("d_name:%s/%s\n",basePath,ptr->d_name);
            files.push_back(ptr->d_name);
        } else if(ptr->d_type == 10){    ///link file
            //printf("d_name:%s/%s\n",basePath,ptr->d_name);
            continue;
        } else if(ptr->d_type == 4){    ///dir
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
	LOG_PRINT(LogLevel::debug, "[SendMessage: %s]", buf);
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

    SendPackMessage(reconnection, head, data, data.length());
}













void BBU::EchoSortResult(vector<PAIR>& tVector)
{
    for(int i = 0; i < static_cast<int>(tVector.size()); i++)
    {
	    LOG_PRINT(LogLevel::debug, "%s:%s", tVector[i].first.c_str(), tVector[i].second.c_str());
    }

	LOG_PRINT(LogLevel::debug, "Max Delay: %s", tVector.begin()->second.c_str());
}

void BBU::NetworkTopology()
{
    std::map<std::string, DeviceInfo> netTopology;
    GetNetworkTopology(netTopology);

    for(auto &it : netTopology)
    { 
        LOG_PRINT(LogLevel::debug, "\n\tnetTopology: -> %s %s %s %s %s %s %s %s %s %s",
									it.second.s_ip.c_str(),
									it.second.s_mac.c_str(), 
                                    it.second.s_source.c_str(),
                                    it.second.s_hop.c_str(),
									it.second.s_port.c_str(), 
                                    it.second.s_uport.c_str(),
									it.second.s_routeIndex.c_str(), 
                                    it.second.s_upgradeState.c_str(),
                                    it.second.s_rruDelayInfo.T2a.c_str(),
                                    it.second.s_rruDelayInfo.Ta3.c_str());
    }
}


