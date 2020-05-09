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

void BHRO_API::AdapterProcess(std::string method, uv::Packet& packet)
{
    {
        std::unique_lock<std::mutex> lock(threadArg_.mutex);
        threadArg_.condition.wait(lock, [] { return threadArg_.inited; });
    }
    std::this_thread::sleep_for(chrono::milliseconds(100)); // 延时 500ms

    threadArg_.oam_adapter->GetNetworkTopology();
    if(threadArg_.oam_adapter->GetRSPPacket(packet))
    {
        LOG_PRINT(LogLevel::debug, "success, packet=%s", packet.GetPacket().c_str());
    } else {
        LOG_PRINT(LogLevel::error, "failure");
    }
#if 0
    switch(CALC_STRING_HASH(args[0])){
    case "upgrade"_HASH:
        {
            if(args.size() < 4)
            {
                LOG_PRINT(LogLevel::error, "upgrade command error");
                continue;
            }
            if(args[1] == "0" || args[1] == "1")
            {
                if(args.size() == 4){
                    threadArg.oam_adapter->SendUpgradeMessage(args[1], args[2], args[3]);
                } else if(args.size() == 5){
                    threadArg.oam_adapter->SendUpgradeMessage(args[1], args[2], args[3], args[4]);
                } else {
                    std::cout << "upgrade command error" << std::endl;
                    continue;
                }
            } else {
                std::cout << "upgrade device number error" << std::endl;
                continue;
            }
            break;
        }
    case "get_topo"_HASH:{
                             threadArg.oam_adapter->GetNetworkTopology();
                             break;
                         }
    case "RFTxStatus_set"_HASH:{
                                   if(args.size() < 2)
                                   {
                                       std::cout << "RFTxStatus_set command error" << std::endl;
                                       continue;
                                   }
                                   if(args.size() == 2){
                                       threadArg.oam_adapter->SendRFTxMessage(args[1]);
                                   } else if(args.size() == 3){
                                       threadArg.oam_adapter->SendRFTxMessage(args[1], args[2]);
                                   } else {
                                       std::cout << "RFTxStatus_set command error" << std::endl;
                                       continue;
                                   }
                                   break;
                               }
    case "exit"_HASH:{
                         exit(0);
                     }
    default:
                     cout<<"Cli Command Error"<<endl;
                     break;
    }
#endif
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

