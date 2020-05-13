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

#define SOCKETPORT      30000
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
    void ConditionWait();

    /* destination: 
     * 0 - HUB 
     * 1 - RRU
     * */
    void DeviceUpGrade(std::string destination, std::string fileName, std::string md5);
    void DeviceUpGrade(std::string destination, std::string routeIndex, std::string fileName, std::string md5);

    void SetRFTx(std::string RFTxStatus);
    void SetRFTx(std::string routeIndex, std::string RFTxStatus);

	void GetTopo();

    void TestTimer();

private:

};
