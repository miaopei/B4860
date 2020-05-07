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

}

void BHRO_API::BHRO_INIT()
{
    EventLoop* loop = new EventLoop();
    loop_ = loop;

    uv::GlobalConfig::BufferModeStatus = uv::GlobalConfig::ListBuffer;

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

    loop->run();
	
    free(pdata);
    pdata = NULL;
}

bool BHRO_API::GetToPo(uv::Packet packet)
{
    oam_adapter_->GetNetworkTopology();

    std::this_thread::sleep_for(chrono::milliseconds(100)); // 延时 500ms

    if(oam_adapter_->GetRSPPacket(packet))
        return true;
    return false;
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

