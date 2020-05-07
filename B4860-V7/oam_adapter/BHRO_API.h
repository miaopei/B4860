/*************************************************************************
	> File Name: BHRO_API.cpp
	> Author: miaopei
	> Mail: miaopei@baicells.com 
	> Created Time: 2020年02月12日 星期三 11时16分50秒
 ************************************************************************/

#pragma once

#include <iostream>
#include <thread>
#include <memory>
#include <mutex>
#include <condition_variable>

#include "oam_adapter.h"

#define interface_name "enp0s31f6"

using namespace uv;
using namespace std;

using OamAdapterPtr = std::shared_ptr<OamAdapter>;

class BHRO_API
{
public:
    BHRO_API();
    void BHRO_INIT();
    bool GetToPo(uv::Packet packet);

private:
    EventLoop* loop_;
    OamAdapterPtr oam_adapter_;

    bool inited_;
};
