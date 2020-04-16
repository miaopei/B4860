#ifndef UV_TCP_SERVER_H
#define UV_TCP_SERVER_H

#include <iostream>
#include <functional>
#include <set>
#include <map>
#include <vector>
#include <cstring>
#include <regex>
#include <memory>

#include "TcpAccepter.h"
#include "TcpConnection.h"
#include "TimerWheel.h"
#include "Packet.h"

namespace uv
{

using OnConnectionStatusCallback =  std::function<void (std::weak_ptr<TcpConnection> )> ;

//no thread safe.
class TcpServer
{
public:
	typedef struct RRUDelayInfo_S
	{
		std::string T2a;
		std::string Ta3;
	}RRUDelayInfo_T;
	
	struct DeviceInfo
    {
    	std::string s_ip;
    	TcpConnectionPtr s_connection;
        std::string s_source;
		std::string s_mac;
        std::string s_hop;
        std::string s_port;
        std::string s_uport;
		std::string s_routeIndex;
        int s_upgradeState;
		RRUDelayInfo_T s_rruDelayInfo;
		#if 0
        DeviceInfo(){};
        DeviceInfo(std::string ip, TcpConnectionPtr connect, std::string source, std::string rruid, std::string port, std::string uport)
        {
        	s_ip = ip;
			s_connection = connect;
            s_source = source;
            s_RRUID = rruid;
            s_port = port;
            s_uport = uport;
        };
		#endif
    };	
	
    TcpServer(EventLoop* loop, bool tcpNoDelay = true);
    virtual ~TcpServer();
    int bindAndListen(SocketAddr& addr);
    void addConnnection(std::string& name,TcpConnectionPtr connection);
    void removeConnnection(std::string& name);
    TcpConnectionPtr getConnnection(std::string& name);
    void closeConnection(std::string& name);
    void getAllConnection(std::map<std::string ,TcpConnectionPtr>  &allconnnections);

    void onMessage(TcpConnectionPtr connection,const char* buf,ssize_t size);
    void setMessageCallback(OnMessageCallback callback);

    void setNewConnectCallback(OnConnectionStatusCallback callback);
    void setConnectCloseCallback(OnConnectionStatusCallback callback);

    void write(TcpConnectionPtr connection,const char* buf,unsigned int size, AfterWriteCallback callback = nullptr);
    void write(std::string& name,const char* buf,unsigned int size, AfterWriteCallback callback =nullptr);
    void writeInLoop(TcpConnectionPtr connection,const char* buf,unsigned int size,AfterWriteCallback callback);
    void writeInLoop(std::string& name,const char* buf,unsigned int size,AfterWriteCallback callback);

	std::string GetCurrentName(TcpConnectionPtr connection);
	bool SetConnectionInfo(TcpConnectionPtr connection, DeviceInfo& cInfo);
	void GetHUBsConnection(std::vector<TcpConnectionPtr>& hubsConnection);
	void GetRRUsConnection(std::vector<TcpConnectionPtr>& rrusConnection);
	void GetOAMConnection(std::vector<TcpConnectionPtr>& oamConnection);
	void GetNetworkTopology(std::map<std::string, DeviceInfo>& netTopology);

	/* 实现key-value数据插入及修改 */
	std::vector<std::string> Split(const std::string& in, const std::string& delim);
	typedef struct atom_node
	{
	    std::string key;
	    std::string value;

	} atom;
	typedef std::pair <std::string, struct atom_node> _KV_ ;
	std::map<std::string, atom> delay_map;
	void SplitStrings2Map(const std::string &input, std::string rruid, std::map<std::string, atom>& map);
	void DeleteHubDelay(std::string rruid, std::map<std::string, atom>& map);
    void UpdateDelayInfo(const std::string &input, std::string rruid, std::map<std::string, atom>& map);

	/* 最大时延补偿 map，最大时延可配置 */
    typedef std::pair<std::string, std::string> PAIR;	
    vector<PAIR> tVectorDL;
    vector<PAIR> tVectorUL;
    static double cmp(const PAIR& x, const PAIR& y);
    void sortMapByValue(std::map<std::string, std::string>& map, vector<PAIR>& tVector);

	std::string CreateRouteIndex(uv::TcpConnectionPtr& connection);
    bool FindNextDeviceInfo(int level, DeviceInfo& next_dInfo);
	bool DeleteRRUTotalDelay(uv::TcpConnectionPtr& connection, vector<PAIR>& tVector);

	bool SetDeviceRouteIndex(uv::TcpConnectionPtr& connection);
	bool GetDeviceInfo(uv::TcpConnectionPtr& connection, DeviceInfo& dInfo);
	bool SetDeviceInfo(uv::TcpConnectionPtr& connection, std::string key, std::string value);

    bool GetConnectByRouteIndex(std::string& routeIndex, uv::TcpConnectionPtr& connection);

	bool SetRRUDeviceDelayInfo(uv::TcpConnectionPtr& connection, RRUDelayInfo_T& rruDelayInfo);

    bool FindUpHubDeviceInfo(uv::TcpConnectionPtr& connection, DeviceInfo& upHubDInfo);

    void setTimeout(unsigned int);
private:
    void onAccept(EventLoop* loop, UVTcpPtr client);
protected:
    EventLoop* loop_;
private:
    bool tcpNoDelay_;
    SocketAddr::IPV ipv_;
    std::shared_ptr <TcpAccepter> accetper_;
    std::map<std::string ,TcpConnectionPtr>  connnections_;
	DeviceInfo cInfo_;
	std::map<std::string, DeviceInfo> connectionInfo_;

    OnMessageCallback onMessageCallback_;
    OnConnectionStatusCallback onNewConnectCallback_;
    OnConnectionStatusCallback onConnectCloseCallback_;
    TimerWheel timerWheel_;

	int m_base = 0;
};


}
#endif
