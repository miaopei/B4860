/*************************************************************************
	> File Name: OamAdapter.h
	> Author: miaopei
	> Mail: miaopei@baicells.com 
	> Created Time: 2020年02月12日 星期三 11时16分50秒
 ************************************************************************/

#pragma once

#include <iostream>
#include <thread>
#include <chrono>
#include <string>

#include "uv11.h"

#define IFRNAME     "enp0s31f6"

class OamAdapter :public uv::TcpClient
{
public:
    OamAdapter(uv::EventLoop* loop);

    void onConnect(ConnectStatus status);
    void reConnect();
    void SendConnectMessage();
    void connectToServer(uv::SocketAddr& addr);
    void RecvMessage(const char* buf, ssize_t size);
	void ProcessRecvMessage(uv::Packet& packet);

    void ConnectResultProcess(uv::Packet& packet);

    void SetROamAdapterInfo();
    void SendROamAdapterDelayInfo();

    void SendPackMessage(uv::Packet::Head& head, std::string& data, ssize_t size);
    void SendMessage(const char* buf, ssize_t size);

    /* destination: 
     * 0 - HUB 
     * 1 - RRU
     * */
    void SendUpgradeMessage(std::string& destination, std::string& fileName, std::string& md5);

    void GetUpgradeResult();

    void TestFunc();

private:
    std::shared_ptr<uv::SocketAddr> sockAddr;
    std::shared_ptr<OamAdapter> clientptr_;
    std::string m_source;
	std::string m_mac;
    std::string m_hop;
    std::string m_port;
    std::string m_uport;
};

