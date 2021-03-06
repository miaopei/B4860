/*************************************************************************
	> File Name: bbu.h
	> Author: miaopei
	> Mail: miaopei@baicells.com 
	> Created Time: 2020年02月12日 星期三 11时15分58秒
 ************************************************************************/
#pragma once

#ifndef BBU_SERVER_H
#define BBU_SERVER_H 

#include <iostream>
#include <cstdio>
#include <cstring>
#include <map>
#include <vector>
#include <set>

#include <unistd.h>
#include <dirent.h>

#include <string>
#include <algorithm>
#include <fstream>

#include "md5.h"
#include "uv11.h"

#define HUBToffset          3
#define HUBCascadeEToffset  3
#define HUBDownToffset      53
#define RRUToffset          6
#define TOFFSETCYCLE        8.013       

#define PORT        30000
#define IFRNAME     "enp0s31f6"

#if 0
#ifdef __cplusplus
extern "C" {
#endif

#include "bpoam_message.h"

int ipc_init_nxp(int rat_id, FUN_T_PARA *para);

void bpoamDelayT14Req(void *arg);
void bpoamDelayT14Rsp(void *arg);

#ifdef __cplusplus
}
#endif
#endif

#define BBUT14  580

class BBU :public uv::TcpServer
{
public:
    enum HeadType {
        B2A_HEAD            = 0,
        B2H_HEAD            = 1,
        B2R_HEAD            = 2,
        S2D_REVERSAL_HEAD   = 3,
        D2A_HEAD            = 4,
        DEFAULT             = 5,
    };
    
    enum DeviceType{
        ALL_HUB_DEVICE    = 1,
        ALL_RRU_DEVICE    = 2
    };

    BBU(uv::EventLoop* loop);
    void ProcessRecvMessage(uv::TcpConnectionPtr connection, uv::Packet& packet);
	
    void SendMessage(uv::TcpConnectionPtr connection, const char* buf, ssize_t size);
	void SendConnectionMessage(uv::TcpConnectionPtr& connection, uv::Packet& packet);

    void SendPackMessage(uv::TcpConnectionPtr& connection, uv::Packet::Head head, std::string& data, ssize_t size);
    void SendPackMessageToAllDevice(DeviceType device, uv::Packet::Head head, std::string& data, ssize_t size);
    void SendMessage2Adapter(uv::Packet::Head head, std::string& data, ssize_t size);

	void BBUMessageProcess(uv::TcpConnectionPtr& connection, uv::Packet& packet);
	void HUBMessageProcess(uv::TcpConnectionPtr& connection, uv::Packet& packet);
	void RRUMessageProcess(uv::TcpConnectionPtr& connection, uv::Packet& packet);
	void OAMMessageProcess(uv::TcpConnectionPtr& connection, uv::Packet& packet);

	void SetConnectionClient(uv::TcpConnectionPtr& connection, uv::Packet& packet);
	void DelayMeasurementProcess(uv::TcpConnectionPtr& connection, uv::Packet& packet);

    /* 时延测量需要实现的几个接口函数 */
    /* 更新HUB时延测量信息 */
    void SendUpdateHUBDelayMessage(uv::TcpConnectionPtr& connection, uv::Packet& packet);
    void UpdateHUBDelayInfo(uv::Packet& packet);
    bool QueryUhubConnection(uv::TcpConnectionPtr& connection, uv::TcpConnectionPtr& reconnection);
    /* 时延补偿计算，整个链路如何实现自动计算？补偿计算值排序，最大时延支持可配置 */
    bool CalculationDelayCompensation(uv::TcpConnectionPtr& connection, std::string& delayiULCompensation, std::string& delayiDLCompensation);

	/* RRU 接入后其他的 RRU 时延补偿需要更新，有可能新接入的 RRU 时延是最大的 */
	void UpdataRRUDelayOffsetValue(uv::TcpConnectionPtr& connection, int maxDLDelay, int maxULDelay);
	
	bool FindDelayMapValue(std::string key, std::string& value);

	bool FindDataMapValue(std::map<std::string, std::string>& map, std::string key, std::string& value);

    /* HUB 时延信息存储map维护，设备掉电需要更新map RRU新接入需要更新 */
    void HubDelayInfo(uv::Packet& packet);

    /* BBU 时延测量数据处理，返回最大时延补偿 */
    void RruDelayProcess(uv::TcpConnectionPtr& connection, uv::Packet& packet);

    /* 实现拓扑查询：
     * 1. 查 rru 的上一级 hub 信息 
     * 2. 查 rru 整个链路的连接情况 rru_0_3_0_1_0_2 (rru端口0连接hub 3号口，hub 0号口连上级hub 1号口，hub 0号口连bbu 2号口)
     * 3.  
     */
    std::string QueryCompleteLink(std::string rruid);

	bool SaveRRUDelayInfo(uv::TcpConnectionPtr& connection, uv::Packet& packet);

    void NetworkTopologyMessageProcess(uv::TcpConnectionPtr& connection, uv::Packet& packet);

    void UpgradeResultProcess(uv::TcpConnectionPtr& connection, uv::Packet& packet);

    void HUBUpgradeProcess(uv::TcpConnectionPtr& connection, uv::Packet& packet);
    void RRUUpgradeProcess(uv::TcpConnectionPtr& connection, uv::Packet& packet);
    void RRURFTxStatusProcess(uv::TcpConnectionPtr& connection, uv::Packet& packet);

	int _system(std::string command);

	bool write_file(std::string file, const std::string& data);
	bool read_file(std::string file, char* data, ssize_t size);
    
    std::vector<std::string> GetFiles(std::string cate_dir);

    void CreateHead(HeadType hType, uv::Packet::Head& head);
    void CreateHead(HeadType hType, uv::Packet::Head& head, uv::Packet& packet);

    void DevicesDataSetProcess(uv::Packet& packet);
	
    void EchoSortResult(vector<PAIR>& tVector);

private:
    void OnMessage(uv::TcpConnectionPtr connection, const char* buf, ssize_t size);
    void OnConnectClose(uv::TcpConnectionPtr connection);
	uint32_t m_carrierIdx_0_T14;
	uint32_t m_carrierIdx_1_T14;
	bool CloseDeviceConnectFlag;
    std::string m_mac;
    std::string m_source;
    std::string m_hop;
    std::string m_port;
    std::string m_uport;
    std::string m_uuport;
};

#endif // BBU_SERVER_H

