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

struct ThreadArg
{
    EventLoop* loop_;
    OamAdapterPtr oam_adapter;
    std::mutex mutex;
    std::condition_variable condition;
    bool inited;
    int flag;
};
static struct ThreadArg threadArg_;

class BHRO_API
{
public:
    BHRO_API();
    void ConnectBBU(ThreadArg& threadArg);
    void BHRO_INIT();
    void AdapterProcess(std::string method, uv::Packet& packet);
    bool GetToPo(uv::Packet& packet);

private:

};
