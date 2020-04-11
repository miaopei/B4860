#include <functional>
#include <memory>
#include <string>

#include "include/TcpServer.h"
#include "include/LogWriter.h"

using namespace std;
using namespace uv;


TcpServer::TcpServer(EventLoop* loop, bool tcpNoDelay)
    :loop_(loop),
    tcpNoDelay_(tcpNoDelay),
    accetper_(nullptr),
    onMessageCallback_(nullptr),
    onNewConnectCallback_(nullptr),
    onConnectCloseCallback_(nullptr),
    timerWheel_(loop)
{

}

TcpServer:: ~TcpServer()
{

}

void TcpServer::setTimeout(unsigned int seconds)
{
    timerWheel_.setTimeout(seconds);
}

void uv::TcpServer::onAccept(EventLoop * loop, UVTcpPtr client)
{
    string key;
    SocketAddr::AddrToStr(client.get(), key, ipv_);

    uv::LogWriter::Instance()->info("new connect  " + key);
    shared_ptr<TcpConnection> connection(new TcpConnection(loop, key, client));
    if (connection)
    {
        connection->setMessageCallback(std::bind(&TcpServer::onMessage, this, placeholders::_1, placeholders::_2, placeholders::_3));
        connection->setConnectCloseCallback(std::bind(&TcpServer::closeConnection, this, placeholders::_1));
        addConnnection(key, connection);
        timerWheel_.insertNew(connection);
        if (onNewConnectCallback_)
            onNewConnectCallback_(connection);
    }
    else
    {
        uv::LogWriter::Instance()->error("create connection fail. :" + key);
    }
}

int TcpServer::bindAndListen(SocketAddr& addr)
{
    ipv_ = addr.Ipv();
    accetper_ = std::make_shared<TcpAccepter>(loop_, tcpNoDelay_);
    auto rst = accetper_->bind(addr);
    if (0 != rst)
    {
        return rst;
    }
    accetper_->setNewConnectinonCallback(std::bind(&TcpServer::onAccept, this, std::placeholders::_1, std::placeholders::_2));
    timerWheel_.start();
    return accetper_->listen();
}

void TcpServer::addConnnection(std::string& name,TcpConnectionPtr connection)
{
	connectionInfo_.insert(pair<std::string, DeviceInfo>(name, cInfo_));
    connnections_.insert(pair<string,shared_ptr<TcpConnection>>(std::move(name),connection));
}

void TcpServer::removeConnnection(string& name)
{
	/* 需要处理hub delay info dalay_map*/
	auto rst = connectionInfo_.find(name);
    if(rst != connectionInfo_.end())
    {
    	if(rst->second.s_source != to_string(uv::Packet::OAM))
    	{
			if(rst->second.s_source == to_string(uv::Packet::HUB))
			{
				DeleteHubDelay(rst->second.s_hop, delay_map);
			} 
			else if(rst->second.s_source == to_string(uv::Packet::RRU))
			{
				if(!DeleteRRUTotalDelay(rst->second.s_connection, tVectorDL))
				{
					uv::LogWriter::Instance()->error("Error: DeleteRRUTotalDelay tVectorDL error");
				}
				if(!DeleteRRUTotalDelay(rst->second.s_connection, tVectorUL))
				{
					uv::LogWriter::Instance()->error("Error: DeleteRRUTotalDelay tVectorUL error");
				}
			}
    	}
    }

    connnections_.erase(name);
	connectionInfo_.erase(name);
}

shared_ptr<TcpConnection> TcpServer::getConnnection(string& name)
{
    auto rst = connnections_.find(name);
    if(rst == connnections_.end())
    {
        return nullptr;
    }
    return rst->second;
}

void TcpServer::getAllConnection(std::map<std::string ,TcpConnectionPtr>  &allconnnections)
{
    allconnnections = connnections_;
}

std::string TcpServer::GetCurrentName(TcpConnectionPtr connection)
{
	for(auto &it : connnections_)
	{
		if(it.second == connection)
		{
			return it.first;
			break;
		}
	}
	return "";
}

bool TcpServer::SetConnectionInfo(TcpConnectionPtr connection, DeviceInfo& cInfo)
{
	std::string cName = GetCurrentName(connection);

	if(cName.empty())
	{
		uv::LogWriter::Instance()->error("Error: not find connection name");
		return false;
	}
	
	auto rst = connectionInfo_.find(cName);
    if(rst == connectionInfo_.end())
    {
        return false;
    } 
	connectionInfo_[rst->first] = cInfo;
	return true;
}

void TcpServer::GetHUBsConnection(std::vector<TcpConnectionPtr>& hubsConnection)
{
	for(auto &it : connectionInfo_)
	{
		if(it.second.s_source == to_string(uv::Packet::HUB))
		{
			hubsConnection.push_back(it.second.s_connection);
		}
	}
}

void TcpServer::GetRRUsConnection(std::vector<TcpConnectionPtr>& rrusConnection)
{
	for(auto &it : connectionInfo_)
	{
		if(it.second.s_source == to_string(uv::Packet::RRU))
		{
			rrusConnection.push_back(it.second.s_connection);
		}
	}
}

void TcpServer::GetOAMConnection(std::vector<TcpConnectionPtr>& oamConnection)
{
	for(auto &it : connectionInfo_)
	{
		if(it.second.s_source == to_string(uv::Packet::OAM))
		{
			oamConnection.push_back(it.second.s_connection);
		}
	}
}

void TcpServer::GetNetworkTopology(std::map<std::string, DeviceInfo>& netTopology)
{
	netTopology = connectionInfo_;
}

#if 0
std::vector<std::string> TcpServer::Split(const std::string& in, const std::string& delim)
{
    vector<string> ret;
    try
    {
        regex re{delim};
        return vector<string>{
            sregex_token_iterator(in.begin(), in.end(), re, -1),
            sregex_token_iterator()
        };      
    }
    catch(const std::exception& e)
    {
        std::cout << "error:" << e.what() << std::endl;
    }
    return ret;
}
#endif

std::vector<std::string> TcpServer::Split(const std::string& in, const std::string& delim)
{
    vector<string> res;
    if("" == in) return res;

    char *strs = new char[in.length() + 1];
    strcpy(strs, in.c_str());

    char *d = new char[delim.length() + 1];
    strcpy(d, delim.c_str());

    char *p = strtok(strs, d);
    while(p) {
        string s = p;
        res.push_back(s);
        p = strtok(NULL, d);
    }

    return res;
}

void TcpServer::SplitStrings2Map(const std::string &input, std::string rruid, std::map<std::string, atom>& map)
{
    std::string key;
    atom v;
    int cout = 0;
    vector<string>ret = Split(input.c_str(), "&");
    int ret_size = ret.size();
	m_base = ret_size / 3;
	std::cout << "m_base = " << m_base << std::endl;
    for(int i = 0; i < ret_size; i++)
    {
        /* key = rruid + type + port(delay num)  
         * type: rhub_delay_up(1) rhub_delay_down(2) t14_delay(3) 
         * delay num: 1-9
         */
        cout = (i % m_base) + 1;
        //cout = i % m_base;
        if(i < m_base)
        {
            key = std::string(rruid + "1" + to_string(cout));
            vector<string>kv = Split(ret[i].c_str(), "=");
            v.key = kv[0].c_str();
            v.value = kv[1].c_str();
            map.insert(_KV_(key, v));
        } else if((i >= m_base) && (i < (m_base*2)))
        {
            key = std::string(rruid + "2" + to_string(cout));
            vector<string>kv = Split(ret[i].c_str(), "=");
            v.key = kv[0].c_str();
            v.value = kv[1].c_str();
            map.insert(_KV_(key, v));
        } else if((i >= (m_base*2)) && (i < (m_base*3)))
        {
            key = std::string(rruid + "3" + to_string(cout));
            vector<string>kv = Split(ret[i].c_str(), "=");
            v.key = kv[0].c_str();
            v.value = kv[1].c_str();
            map.insert(_KV_(key, v));
        }
    }
}

void TcpServer::DeleteHubDelay(std::string rruid, std::map<std::string, atom>& map)
{
	if(m_base == 0)
	{
		std::cout << "Error: m_base error" << std::endl;
		return;
	}
	
    std::string key;
    int cout = 0;
    for(int i = 0; i < (m_base*3); i++)
    {
        cout = (i % m_base) + 1;
        key = std::string(rruid + to_string((i/m_base)+1) + to_string(cout));
        //std::cout << "key=" << key << std::endl;
        map.erase(key);
    }
}

void TcpServer::UpdateDelayInfo(const std::string &input, std::string rruid, std::map<std::string, atom>& map)
{
    DeleteHubDelay(rruid, map);
	//std::this_thread::sleep_for(chrono::milliseconds(500)); // 延时 100ms 
    SplitStrings2Map(input, rruid, map);
}

double TcpServer::cmp(const PAIR& x, const PAIR& y)
{
    return stoi(x.second.c_str()) > stoi(y.second.c_str());
}

void TcpServer::sortMapByValue(std::map<std::string, std::string>& map, vector<PAIR>& tVector)
{
    for(auto &it : map)
        tVector.push_back(make_pair(it.first, it.second));

    sort(tVector.begin(), tVector.end(), cmp);
}

std::string TcpServer::CreateRouteIndex(uv::TcpConnectionPtr& connection)
{
    int level = 0;
    std::string RouteIndex;
    DeviceInfo dInfo;
	DeviceInfo next_dInfo;

	if(!GetDeviceInfo(connection, dInfo))
	{
		std::cout << __FILE__ << ":" << __LINE__ << ":" <<__FUNCTION__ 
                  << " > Error: Get Device Info error" << std::endl;
		return "";
	}

	level = std::stoi(dInfo.s_hop);
	RouteIndex = dInfo.s_port + "_" + dInfo.s_uport;

    level = level - 1;
    if(level < 0)
	{
		std::cout << __FILE__ << ":" << __LINE__ << ":" <<__FUNCTION__ 
                  << "#Error: level error" << std::endl;
		return "";
	} 
	if(level == 0)
	{
		return RouteIndex;
	}
    
    if(level >= 1)
    {
        for(level = level; level > 0; level--)
        {
            if(!FindNextDeviceInfo(level, next_dInfo))
            {
                std::cout << __FILE__ << ":" << __LINE__ << ":" <<__FUNCTION__ 
                    << "#Error: level error" << std::endl;
                return "";
            }
            RouteIndex += "_" + next_dInfo.s_port + "_" + next_dInfo.s_uport;
        }
    }
    return RouteIndex;
}

bool TcpServer::FindNextDeviceInfo(int level, DeviceInfo& next_dInfo)
{
    for(auto &it : connectionInfo_)
    {
        if(it.second.s_hop == to_string(level))
        {
            next_dInfo = it.second;
            return true;
        }
    }
    return false;
}

bool TcpServer::DeleteRRUTotalDelay(uv::TcpConnectionPtr& connection, vector<PAIR>& tVector)
{
    std::string routeIndex = CreateRouteIndex(connection);

	vector<PAIR>::iterator itor;
	for(itor = tVector.begin(); itor != tVector.end(); itor++)
	{
		if(itor->first == routeIndex)
		{
			tVector.erase(itor);
			return true;
		}
	}
	return false;
}

bool TcpServer::SetDeviceRouteIndex(uv::TcpConnectionPtr& connection)
{
	std::string RouteIndex = CreateRouteIndex(connection);
	std::string cName = GetCurrentName(connection);
	
	if(RouteIndex.empty())
	{
		uv::LogWriter::Instance()->error("Error: CreateRouteIndex failure");
		return false;
	}
	
	if(cName.empty())
	{
		uv::LogWriter::Instance()->error("Error: not find connection name");
		return false;
	}
	
	auto rst = connectionInfo_.find(cName);
    if(rst == connectionInfo_.end())
    {
    	uv::LogWriter::Instance()->error("Error: not find connection");
        return false;
    }
	rst->second.s_routeIndex = RouteIndex;
	return true;
}

bool TcpServer::GetDeviceInfo(uv::TcpConnectionPtr& connection, DeviceInfo& dInfo)
{
	std::string cName = GetCurrentName(connection);
		
	if(cName.empty())
	{
		uv::LogWriter::Instance()->error("Error: not find connection name");
		return false;
	}
	
	auto rst = connectionInfo_.find(cName);
    if(rst == connectionInfo_.end())
    {
    	uv::LogWriter::Instance()->error("Error: not find connection");
        return false;
    }
	dInfo = rst->second;
	return true;
}

bool TcpServer::SetRRUDeviceDelayInfo(uv::TcpConnectionPtr& connection, RRUDelayInfo_T& rruDelayInfo)
{
	std::string cName = GetCurrentName(connection);
	
	if(cName.empty())
	{
		uv::LogWriter::Instance()->error("Error: not find connection name");
		return false;
	}
	
	auto rst = connectionInfo_.find(cName);
    if(rst == connectionInfo_.end())
    {
    	uv::LogWriter::Instance()->error("Error: not find connection");
        return false;
    }
	rst->second.s_rruDelayInfo = rruDelayInfo;
	return true;
}

void TcpServer::closeConnection(string& name)
{
    auto connection = getConnnection(name);
    if (nullptr != connection)
    {
        connection->close([this](std::string& name)
        {
            auto connection = getConnnection(name);
            if (nullptr != connection)
            {
                if (onConnectCloseCallback_)
                {
                    onConnectCloseCallback_(connection);
                }

				/* 需要处理hub delay info dalay_map*/
				auto rst = connectionInfo_.find(name);
			    if(rst != connectionInfo_.end())
			    {
			    	if(rst->second.s_source != to_string(uv::Packet::OAM))
			    	{
						if(rst->second.s_source == to_string(uv::Packet::HUB))
						{
							DeleteHubDelay(rst->second.s_hop, delay_map);
						} 
						else if(rst->second.s_source == to_string(uv::Packet::RRU))
						{
							if(!DeleteRRUTotalDelay(rst->second.s_connection, tVectorDL))
							{
								uv::LogWriter::Instance()->error("Error: DeleteRRUTotalDelay tVectorDL error");
							}
							if(!DeleteRRUTotalDelay(rst->second.s_connection, tVectorUL))
							{
								uv::LogWriter::Instance()->error("Error: DeleteRRUTotalDelay tVectorUL error");
							}
						}
			    	}
			    }
				
                connnections_.erase(name);
				connectionInfo_.erase(name);
            }

        });
    }
}

void TcpServer::onMessage(TcpConnectionPtr connection,const char* buf,ssize_t size)
{
    if(onMessageCallback_)
        onMessageCallback_(connection,buf,size);
    timerWheel_.insert(connection);
}

void TcpServer::setMessageCallback(OnMessageCallback callback)
{
    onMessageCallback_ = callback;
}

void TcpServer::write(shared_ptr<TcpConnection> connection,const char* buf,unsigned int size, AfterWriteCallback callback)
{
    if(nullptr != connection)
    {
        connection->write(buf,size, callback);
    }
    else if (callback)
    {
        WriteInfo info = { WriteInfo::Disconnected,const_cast<char*>(buf),size };
        callback(info);
    }
}

void TcpServer::write(string& name,const char* buf,unsigned int size,AfterWriteCallback callback)
{
    auto connection = getConnnection(name);
    write(connection, buf, size, callback);
}

void TcpServer::writeInLoop(shared_ptr<TcpConnection> connection,const char* buf,unsigned int size,AfterWriteCallback callback)
{
    if(nullptr != connection)
    {
        connection->writeInLoop(buf,size,callback);
    }
    else if (callback)
    {
        uv::LogWriter::Instance()->warn("try write a disconnect connection.");
        WriteInfo info = { WriteInfo::Disconnected,const_cast<char*>(buf),size };
        callback(info);
    }
}

void TcpServer::writeInLoop(string& name,const char* buf,unsigned int size,AfterWriteCallback callback)
{
    auto connection = getConnnection(name);
    writeInLoop(connection, buf, size, callback);
}

void TcpServer::setNewConnectCallback(OnConnectionStatusCallback callback)
{
    onNewConnectCallback_ = callback;
}

void  TcpServer::setConnectCloseCallback(OnConnectionStatusCallback callback)
{
    onConnectCloseCallback_ = callback;
}
