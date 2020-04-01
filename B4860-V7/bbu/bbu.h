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
#include <regex>

#include "uv11.h"

#define HUBToffset      3
#define RRUToffset      6
#define TOFFSETCYCLE    8.013       

class BBU :public uv::TcpServer
{
public:
    struct Head{
        std::string s_source;
        std::string s_destination;
        std::string s_state;
        std::string s_msgID;
        std::string s_rruid;
        std::string s_port;
        std::string s_uport;
    };

    enum DeviceType{
        ALL_HUB_DEVICE    = 1,
        ALL_RRU_DEVICE    = 2
    };

    BBU(uv::EventLoop* loop);
    //static void writeCallback(uv::WriteInfo& info);
    void SendMessage(uv::TcpConnectionPtr connection, const char* buf, ssize_t size);
	void SendConnectionMessage(uv::TcpConnectionPtr& connection, uv::PacketIR& packet);

    void SendPackMessage(uv::TcpConnectionPtr& connection, Head head, std::string& data, ssize_t size);
    void SendPackMessageToAllDevice(DeviceType device, Head head, std::string& data, ssize_t size);
    
    //void SendAllClientMessage(const char* buf, ssize_t size);

	void BBUMessageProcess(uv::TcpConnectionPtr& connection, uv::PacketIR& packet);
	void HUBMessageProcess(uv::TcpConnectionPtr& connection, uv::PacketIR& packet);
	void RRUMessageProcess(uv::TcpConnectionPtr& connection, uv::PacketIR& packet);
	void OAMMessageProcess(uv::TcpConnectionPtr& connection, uv::PacketIR& packet);

	void SetConnectionClient(uv::TcpConnectionPtr& connection, uv::PacketIR& packet);
	void DelayMeasurementProcess(uv::TcpConnectionPtr& connection, uv::PacketIR& packet);

	void UnPackData(uv::PacketIR& packet);
	
    /* 时延测量需要实现的几个接口函数 */
    /* 更新HUB时延测量信息 */
    void SendUpdateHUBDelayMessage(uv::PacketIR& packet);
    void UpdateHUBDelayInfo(uv::PacketIR& packet);
    bool QueryUhubConnection(std::string rruid, uv::TcpConnectionPtr& connection);
    /* 时延补偿计算，整个链路如何实现自动计算？补偿计算值排序，最大时延支持可配置 */
    void CalculationDelayCompensation();

    /* HUB 时延信息存储map维护，设备掉电需要更新map RRU新接入需要更新 */
    void HubDelayInfo(uv::PacketIR& packet);

    /* 实现拓扑查询：
     * 1. 查 rru 的上一级 hub 信息 
     * 2. 查 rru 整个链路的连接情况 rru_0_3_0_1_0_2 (rru端口0连接hub 3号口，hub 0号口连上级hub 1号口，hub 0号口连bbu 2号口)
     * 3.  
     */
    std::string QueryCompleteLink(std::string rruid);


    void NetworkTopology();

    void TestGet();

private:
    void OnMessage(uv::TcpConnectionPtr connection, const char* buf, ssize_t size);
};

#endif // BBU_SERVER_H

