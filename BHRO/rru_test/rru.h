/*************************************************************************
	> File Name: RRU.h
	> Author: miaopei
	> Mail: miaopei@baicells.com 
	> Created Time: 2020年02月12日 星期三 11时16分50秒
 ************************************************************************/

#pragma once

#include <string>
#include <map>
#include <sys/wait.h>
#include <sys/types.h>
#include <iostream>
#include <fstream>

#include "ftplib.h"
#include "md5.h"
#include "uv11.h"

#define SoftwareVersion     "./SoftwareVersion"
#define UpgradeResult       "./UpgradeResult"

#define PORT        30000
#define IFRNAME     "enp0s31f6"

constexpr size_t HASH_STRING_PIECE(const char *string_piece, size_t hashNum=0){
	return *string_piece ? HASH_STRING_PIECE(string_piece+1, (hashNum*131)+*string_piece) : hashNum;
}
	
constexpr size_t operator "" _HASH(const char *string_pice, size_t){
	return HASH_STRING_PIECE(string_pice);
}

class RRU :public uv::TcpClient
{
public:
    RRU(uv::EventLoop* loop);

    void onConnect(ConnectStatus status);
    void reConnect();
    void SendConnectMessage();
    void connectToServer(uv::SocketAddr& addr);
    void RecvMessage(const char* buf, ssize_t size);
	void ProcessRecvMessage(uv::Packet& packet);

    void SetRRRUInfo();
    void SendRRRUDelayInfo();
    void ConnectResultProcess(uv::Packet& packet);
    void UpdataDelay(uv::Packet& packet);

    void SendPackMessage(uv::Packet::Head& head, std::string& data, ssize_t size);
    void HeartSendPackMessage(uv::Packet::Head head, std::string data, ssize_t size);
    void SendMessage(const char* buf, ssize_t size);

    void UpgradeProcess(uv::Packet& packet);
    bool FindDataMapValue(std::map<std::string, std::string>& map, std::string key, std::string& value);
    int _system(std::string command);

	bool write_file(std::string file, const std::string& data);
	bool read_file(std::string file, char* data, ssize_t size);

    void UpgradeThread(uv::Packet& packet);
    bool FtpDownloadFile(uv::Packet& packet);
    void SendUpgradeFailure(uv::Packet& packet, const std::string errorno);

    void DataSetProcess(uv::Packet& packet);

    void Heart();
    void HandleHeart(void* arg);

    void CreateHead(uv::Packet::Destination dType, uv::Packet::Head& head);

	size_t CALC_STRING_HASH(const string& str);

    void SendMessage2OAM(uv::Packet::MsgID msgID, std::string data);

    std::string bbu_addr;

private:
    std::shared_ptr<uv::SocketAddr> sockAddr;
    std::shared_ptr<RRU> clientptr_;
    std::string m_source;
	std::string m_mac;
    std::string m_hop;
    std::string m_port;
    std::string m_uport;
    std::string m_uuport;
    std::string m_img_filename;
};

