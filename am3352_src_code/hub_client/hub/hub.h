/*************************************************************************
	> File Name: hub.h
	> Author: miaopei
	> Mail: miaopei@baicells.com 
	> Created Time: 2020年02月12日 星期三 11时16分50秒
 ************************************************************************/

#pragma once

#include <string>

#include "uv11.h"

#define TOFFSET 3

#define IFRNAME     "eth0"

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

    void ConnectResultProcess(uv::Packet& packet);
    void UpdataDelay(uv::Packet& packet);

    void SendPackMessage(uv::Packet::Head& head, std::string& data, ssize_t size);
    void SendMessage(const char* buf, ssize_t size);

private:
    std::shared_ptr<uv::SocketAddr> sockAddr;
    std::shared_ptr<HUB> clientptr_;
    std::string m_source;
	std::string m_mac;
    std::string m_hop;
    std::string m_port;
    std::string m_uport;
};

