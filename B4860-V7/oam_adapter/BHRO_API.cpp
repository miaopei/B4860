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
    :inited_(false)
{
    uv::GlobalConfig::BufferModeStatus = uv::GlobalConfig::ListBuffer;
}

void BHRO_API::ConnectBBU()
{
    EventLoop* loop = new EventLoop();
    {
        loop_ = loop;
        std::lock_guard<std::mutex> lock(mutex_);

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
        oam_adapter_ = oamAdapter;
        inited_ = true;

        free(pdata);
        pdata = NULL;
    }
    condition_.notify_one();
    loop->run();
}

void BHRO_API::AdapterProcess()
{
    {
        std::unique_lock<std::mutex> lock(mutex_);
        condition_.wait(lock, [this] { return inited_; });
    }
    uv::Packet packet;

    oam_adapter_->GetNetworkTopology();

    std::this_thread::sleep_for(chrono::milliseconds(100)); // 延时 500ms

    if(oam_adapter_->GetRSPPacket(packet))
    {
        LOG_PRINT(LogLevel::debug, "success, packet=%s", packet.GetPacket().c_str());
    } else {
        LOG_PRINT(LogLevel::error, "failure");
    }
}

void BHRO_API::BHRO_INIT()
{
    LOG_PRINT(LogLevel::debug, "BHRO INIT ...");
    std::thread t1(std::bind(&BHRO_API::ConnectBBU, this));
    //t1.join();
    t1.detach();
}

bool BHRO_API::GetToPo(uv::Packet& packet)
{
    std::thread t2(std::bind(&BHRO_API::AdapterProcess, this));
    t2.join();
#if 0
    oam_adapter_->GetNetworkTopology();

    std::this_thread::sleep_for(chrono::milliseconds(100)); // 延时 500ms
    if(oam_adapter_->GetRSPPacket(packet))
    {
        LOG_PRINT(LogLevel::debug, "success, packet=%s", packet.GetPacket().c_str());
    } else {
        LOG_PRINT(LogLevel::error, "failure");
    }
#endif

    return true;
#if 0
    Timer* pTimer = new Timer(loop_, 500, 3,
        [](Timer* handle)
    {
        LOG_PRINT(LogLevel::info, "timer callback run");
        if(oam_adapter_->GetRSPPacket(packet))
        {
            handle->close([](Timer* ptr)
            {
                LOG_PRINT(LogLevel::info, "release timer ptr");
                delete ptr;
            });
            return true;
        }
    });
    pTimer->start();
    return false;
#endif
}

