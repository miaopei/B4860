/*************************************************************************
	> File Name: main.cpp
	> Author: miaopei
	> Mail: miaopei@baicells.com 
	> Created Time: 2020年02月12日 星期三 11时16分50秒
 ************************************************************************/

#include <iostream>
#include <thread>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <vector>

#include "oam_adapter.h"

using namespace uv;
using namespace std;

using OamAdapterPtr = std::shared_ptr<OamAdapter>;

struct ThreadArg
{
    OamAdapterPtr oam_adapter;
    std::mutex mutex ;
    std::condition_variable condition ;
    bool inited;
};

constexpr size_t HASH_STRING_PIECE(const char *string_piece, size_t hashNum=0){
    return *string_piece ? HASH_STRING_PIECE(string_piece+1, (hashNum*131)+*string_piece) : hashNum;
}

constexpr size_t operator "" _HASH(const char *string_pice, size_t){
    return HASH_STRING_PIECE(string_pice);
}

size_t CALC_STRING_HASH(const string& str){
  	// 获取string对象得字符串值并传递给HAHS_STRING_PIECE计算，获取得返回值为该字符串HASH值
    return HASH_STRING_PIECE(str.c_str());
}

std::string& ClearHeadTailSpace(std::string& str)
{
    if (str.empty())
    {
        return str;
    }
 
    str.erase(0, str.find_first_not_of(" "));
    str.erase(str.find_last_not_of(" ") + 1);

	return str;
}

std::vector<std::string> Split(std::string srcStr, const std::string delim)
{
	int nPos = 0;
	std::vector<string> vec;
	nPos = ClearHeadTailSpace(srcStr).find(delim.c_str());
	while(-1 != nPos)
	{
		std::string temp = srcStr.substr(0, nPos);
		vec.push_back(temp);
		srcStr = srcStr.substr(nPos+1);
		nPos = ClearHeadTailSpace(srcStr).find(delim.c_str());
	}
	if(!srcStr.empty())
		vec.push_back(srcStr);
	
	return vec;
}

void CliUsage()
{
	std::cout << "Cli Usage:" 
			  << "\n  [help]: print cli usage"
			  << "\n  [upgrade device fileName md5]: upgrade device"
			  << "\n\t device: 0 - ALL HUB, 1 - ALL RRU"
			  << "\n\t fileName: upgrade file name, include path"
			  << "\n\t md5: file md5"
			  << "\n  [exit]: exit program"
			  << std::endl;
}

void InitializeOamAdapterConnect(SocketAddr& addr, ThreadArg& threadArg)
{
	EventLoop loop;
	{
        std::lock_guard<std::mutex> lock(threadArg.mutex);

        auto oamAdapter = std::make_shared<OamAdapter>(&loop);
        oamAdapter->connectToServer(addr);
		threadArg.oam_adapter = oamAdapter;
        
        threadArg.inited = true ;
	}
    threadArg.condition.notify_one();
	loop.run();
}

void CliProcess(ThreadArg& threadArg)
{
    {
        std::unique_lock<std::mutex> lock(threadArg.mutex);
        threadArg.condition.wait(lock, [&threadArg] { return threadArg.inited; });
    }
	EventLoop loop;

	std::this_thread::sleep_for(chrono::milliseconds(500)); // 延时 500ms

    std::string cli_args;
	vector<string> args;
    while(1)
    {
    	args.clear();
        std::cout << "OamAdapter > ";
        getline(cin, cli_args);
		args = Split(cli_args, " ");
			
		if(args.size() == 0)
			continue;
		
		switch(CALC_STRING_HASH(args[0])){
	        case "help"_HASH:{
				CliUsage();
	            break;
	        }
	        case "upgrade"_HASH:{
	            if(args.size() != 4)
				{
					std::cout << "upgrade command error" << std::endl;
					continue;
				}
				threadArg.oam_adapter->SendUpgradeMessage(args[1], args[2], args[3]);
	            break;
	        }
	        case "exit"_HASH:{
	            exit(0);
	        }
	        default:
	            cout<<"Cli Command Error"<<endl;
	            break;
    	}
    }
	
	loop.run();
}

int main(int argc, char** argv)
{
	std::string serverIP;

	uv::GlobalConfig::BufferModeStatus = uv::GlobalConfig::ListBuffer;
	
	if(argc != 2)
    {
        fprintf(stdout, "usage: %s server_ip_address\neg.%s 192.168.1.1\n", argv[0], argv[0]);
        return 0;
    }
    serverIP = argv[1];	
    
    SocketAddr addr(serverIP.c_str(), 30000, SocketAddr::Ipv4);
	
    struct ThreadArg threadArg;
    threadArg.inited = false;

	std::thread t1(std::bind(&InitializeOamAdapterConnect, addr, std::ref(threadArg)));
    //跨线程发送消息
    std::thread t2(std::bind(&CliProcess, std::ref(threadArg)));
    t1.join();
    t2.join();

	return 0;
}

