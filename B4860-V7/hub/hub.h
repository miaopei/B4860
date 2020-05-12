/*************************************************************************
	> File Name: hub.h
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

#define TOFFSET 3


#if 0
#ifdef __cplusplus
extern "C" {
#endif

#include "rhub.h"

uint16_t get_rhup_port_id(int mpi_fd, uint8_t port);
void get_rhup_cpri_stat(int mpi_fd, uint8_t dir, uint8_t port, struct rhup_cpri_stat* cpri_stat);
void get_rhup_delay(int mpi_fd, uint8_t dir, struct rhup_data_delay* rhup_delay);
uint16_t get_rhup_port_stat(int mpi_fd);
void get_rhup_t14_delay(int mpi_fd, struct rhup_t14_delay* t14_delay);

int gpmc_mpi_open(const char* gpmc_mpi_dev);
void gpmc_mpi_close(int fd);
unsigned int gpmc_mpi_write(int fd,unsigned int addr,unsigned int wdata);
unsigned int gpmc_mpi_read(int fd,unsigned int addr);
unsigned int gpmc_mpi_write_device(int fd,unsigned int module_addr, unsigned int reg_addr, unsigned int reg_wdata);
unsigned int gpmc_mpi_read_device(int fd,unsigned int module_addr, unsigned int reg_addr);

#ifdef __cplusplus
}
#endif

struct delay_measurement_info {
    struct rhup_data_delay* rhup_delay;
    struct rhup_t14_delay* t14_delay;
};
#endif

struct rhub_data_delay{
    uint16_t delay1;
    uint16_t delay2;
    uint16_t delay3;
    uint16_t delay4;
    uint16_t delay5;
};

struct rhub_t14_delay{
    uint32_t delay1;
    uint32_t delay2;
    uint32_t delay3;
    uint32_t delay4;
    uint32_t delay5;
};

struct delay_measurement_info {
    char head[HEADLENGTH+1];
    struct rhub_data_delay* rhub_delay_up;
    struct rhub_data_delay* rhub_delay_down;
    struct rhub_t14_delay* rhub_t14;
};

class HUB :public uv::TcpClient
{
public:
    HUB(uv::EventLoop* loop);

    void onConnect(ConnectStatus status);
    void reConnect();
    void SendConnectMessage();
    void connectToServer(uv::SocketAddr& addr);
    void RecvMessage(const char* buf, ssize_t size);
	void ProcessRecvMessage(uv::Packet& packet);

    void SetRHUBInfo();
    void SendRHUBDelayInfo();
    void RHUBDelayInfoCalculate(std::string& data);

    void ConnectResultProcess(uv::Packet& packet);
    void UpdataDelay(uv::Packet& packet);

    void SendPackMessage(uv::Packet::Head& head, std::string& data, ssize_t size);
    void SendMessage(const char* buf, ssize_t size);

    void UpgradeProcess(uv::Packet& packet);
    bool FindDataMapValue(std::map<std::string, std::string>& map, std::string key, std::string& value);
    int _system(std::string command);

	bool write_file(std::string file, const std::string& data);
	bool read_file(std::string file, char* data, ssize_t size);

    void UpgradeThread(uv::Packet& packet);
    bool FtpDownloadFile(uv::Packet& packet);
    void SendUpgradeFailure(uv::Packet& packet, const std::string errorno);

    void Heart();
    void HandleHeart(void* arg);


    void TestProcess(uv::Packet& packet);
    void TestGetRhubDelay(uint8_t dir, struct rhub_data_delay* rhub_delay);
    void TestGetRhubT14Delay(struct rhub_t14_delay* t14_delay);

    std::string bbu_addr;

private:
    std::shared_ptr<uv::SocketAddr> sockAddr;
    std::shared_ptr<HUB> clientptr_;
    std::string m_source;
	std::string m_mac;
    std::string m_hop;
    std::string m_port;
    std::string m_uport;

};

