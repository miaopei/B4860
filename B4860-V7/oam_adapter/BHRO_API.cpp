/*************************************************************************
	> File Name: BHRO_API.cpp
	> Author: miaopei
	> Mail: miaopei@baicells.com 
	> Created Time: 2020年02月12日 星期三 11时16分50秒
 ************************************************************************/

#include <iostream>

#include "BHRO_API.h"

using namespace uv;
using namespace std;

BHRO_API::BHRO_API()
{
    threadArg_.inited = false;
    uv::GlobalConfig::BufferModeStatus = uv::GlobalConfig::ListBuffer;
}

void BHRO_API::ConnectBBU(ThreadArg& threadArg)
{
    EventLoop* loop = new EventLoop();
    {
        threadArg.loop_ = loop;
        std::lock_guard<std::mutex> lock(threadArg.mutex);

        char* pdata = NULL;
        size_t size = 32;
        pdata = (char*)malloc(size * sizeof(char));
        if(pdata == NULL)
        {		
            LOG_PRINT(LogLevel::error, "malloc gateway memory error");
        }

        GetDeviceIP(interface_name, pdata, size);	
        LOG_PRINT(LogLevel::debug, "Device IP: %s", pdata);

        SocketAddr addr(pdata, 30000, SocketAddr::Ipv4);

        auto oamAdapter = std::make_shared<OamAdapter>(loop);
        oamAdapter->connectToServer(addr);
        threadArg.oam_adapter = oamAdapter;
        threadArg.inited = true;

        free(pdata);
        pdata = NULL;
        threadArg.flag = 1;
    }
    threadArg.condition.notify_one();
    loop->run();
}

void BHRO_API::BHRO_INIT()
{
    LOG_PRINT(LogLevel::debug, "BHRO INIT ...");
    std::thread t1(std::bind(&BHRO_API::ConnectBBU, this, std::ref(threadArg_)));
    //t1.join();
    t1.detach();
}

void BHRO_API::ConditionWait()
{
    {
        std::unique_lock<std::mutex> lock(threadArg_.mutex);
        threadArg_.condition.wait(lock, [] { return threadArg_.inited; });
    }
    std::this_thread::sleep_for(chrono::milliseconds(100)); // 延时 500ms
}

void BHRO_API::DeviceUpGrade(std::string destination, std::string fileName, std::string md5)
{
    ConditionWait();
    threadArg_.oam_adapter->SendUpgradeMessage(destination, fileName, md5);
}

void BHRO_API::DeviceUpGrade(std::string destination, std::string routeIndex, std::string fileName, std::string md5)
{
    ConditionWait();
    threadArg_.oam_adapter->SendUpgradeMessage(destination, routeIndex, fileName, md5);
}

void BHRO_API::SetRFTx(std::string RFTxStatus)
{
    ConditionWait();
    threadArg_.oam_adapter->SendRFTxMessage(RFTxStatus);
}

void BHRO_API::SetRFTx(std::string routeIndex, std::string RFTxStatus)
{
    ConditionWait();
    threadArg_.oam_adapter->SendRFTxMessage(routeIndex, RFTxStatus);
}

void BHRO_API::GetTopo()
{
    ConditionWait();
    threadArg_.oam_adapter->GetNetworkTopology();

    std::this_thread::sleep_for(chrono::milliseconds(100)); // 延时 100ms

    uv::Packet packet;
    if(threadArg_.oam_adapter->GetRSPPacket(packet))
    {
        LOG_PRINT(LogLevel::debug, "success, packet=%s", packet.GetPacket().c_str());
    } else {
        LOG_PRINT(LogLevel::error, "failure");
    }
}

