/*************************************************************************
	> File Name: RRU.cpp
	> Author: miaopei
	> Mail: miaopei@baicells.com 
	> Created Time: 2020年02月12日 星期三 11时16分50秒
 ************************************************************************/

#include <iostream>
#include <string.h>

#include "rru.h"

using namespace uv;
using namespace std;

RRU::RRU(uv::EventLoop* loop)
    :TcpClient(loop),
    sockAddr(nullptr)
{
    setConnectStatusCallback(std::bind(&RRU::onConnect, this, std::placeholders::_1));
    setMessageCallback(std::bind(&RRU::RecvMessage, this, std::placeholders::_1, std::placeholders::_2));

    SetRRRUInfo();
    m_img_filename = "";
}

void RRU::connectToServer(uv::SocketAddr& addr)
{
    sockAddr = std::make_shared<uv::SocketAddr>(addr);
    connect(addr);
}

void RRU::onConnect(ConnectStatus status)
{
    if(status != ConnectStatus::OnConnectSuccess)
    {
        reConnect();
    } else {
        SendConnectMessage();
    }
}

void RRU::reConnect()
{
    uv::Timer* timer = new uv::Timer(loop_, 1000, 0, [this](uv::Timer* ptr)
    {
        char* pdata = NULL;
        size_t size = 32;
        pdata = (char*)malloc(size * sizeof(char));
        if(pdata == NULL)
        {		
            LOG_PRINT(LogLevel::error, "malloc gateway memory error");
        }
#if 0
        GetDeviceGateWay(IFRNAME, pdata, size);
        LOG_PRINT(LogLevel::debug, "ReConnect Device GateWay: %s", pdata);
#endif
#if 1
        GetDeviceIP(IFRNAME, pdata, size);	
        LOG_PRINT(LogLevel::debug, "ReConnect Device IP: %s", pdata);
#endif

        SocketAddr addr(pdata, PORT, SocketAddr::Ipv4);
        connect(addr);

        free(pdata);
        pdata = NULL;

        ptr->close([](uv::Timer* ptr)
        {
            delete ptr;    
        });
    });
    timer->start();
}

void RRU::SendConnectMessage()
{
    std::string data = "Status=";
    char res[32];
    if(read_file(UpgradeResult, res, sizeof(res)))
    {
        if(strcmp(res, "0") != 0)
        {
            data += res;
            write_file(UpgradeResult, "0");
        } else {
            data += "1";
        }
    } else {
        data += "1";
    }

    if(read_file(SoftwareVersion, res, sizeof(res)))
    {
        data += "&SoftwareVersion=" + std::string(res);
    } else {
        data += "&SoftwareVersion=X";
    }

    data += "&ModelName=NA";
    data += "&SerialNumber=NA";
    data += "&HardwareVersion=NA";

    uv::Packet::Head head;
    CreateHead(uv::Packet::TO_BBU, head);
    head.s_msgID = to_string(uv::Packet::MSG_CONNECT);

    SendPackMessage(head, data, data.length());
}

void RRU::SetRRRUInfo()
{
    char* pdata = NULL;
    size_t size = 32;
    pdata = (char*)malloc(size * sizeof(char));
    if(pdata == NULL)
    {
        LOG_PRINT(LogLevel::error, "malloc memory error");
    }
    GetDeviceMAC(IFRNAME, pdata, size);
	LOG_PRINT(LogLevel::debug, "Device Mac: %s", pdata);

    m_mac       = pdata;
    m_source    = to_string(uv::Packet::RRU);
    m_port      = "0";
    m_hop       = "2";
    m_uport     = "2";
    m_uuport    = "2";
 
    free(pdata);
    pdata = NULL;
}

void RRU::RecvMessage(const char* buf, ssize_t size)
{
    if(size < HEADLENGTH)
    {
		LOG_PRINT(LogLevel::error, "Message Length error.");
        return;
    }

    /* 接收到的数据解析 */
	auto packetbuf = getCurrentBuf();
	if (nullptr != packetbuf)
	{
		packetbuf->append(buf, static_cast<int>(size));
		uv::Packet packet;
		while (0 == packetbuf->readPacket(packet))
		{
			//LOG_PRINT(LogLevel::debug, "[ReceiveData: %d:%s]", packet.DataSize(), packet.getData());
			packet.UnPackMessage();

			/* 打印解包信息 */
			packet.EchoUnPackMessage();

			ProcessRecvMessage(packet);
		}
	}
}

void RRU::ProcessRecvMessage(uv::Packet& packet)
{
	switch(std::stoi(packet.GetMsgID()))
    {
        case uv::Packet::MSG_CONNECT:
            ConnectResultProcess(packet);
            break;
        case uv::Packet::MSG_UPGRADE:
            UpgradeProcess(packet);            
            break;
        case uv::Packet::MSG_SET:                                                                  
            DataSetProcess(packet);
            break;
        default:
			LOG_PRINT(LogLevel::error, "MessageID Error");
    }
}

void RRU::SendPackMessage(uv::Packet::Head& head, std::string& data, ssize_t size)
{
    uv::Packet packet;
    packet.SetHead(head);

    packet.PackMessage(data, size);

    /* 打印数据封装信息 */
    packet.EchoPackMessage();
    
    std::string send_buf = packet.GetPacket();
    
    SendMessage(send_buf.c_str(), send_buf.length());
}

void RRU::HeartSendPackMessage(uv::Packet::Head head, std::string data, ssize_t size)
{
    uv::Packet packet;
    packet.SetHead(head);

    packet.PackMessage(data, size);

    /* 打印数据封装信息 */
    //packet.EchoPackMessage();
    
    std::string send_buf = packet.GetPacket();
    
    SendMessage(send_buf.c_str(), send_buf.length());
}

void RRU::SendMessage(const char* buf, ssize_t size)
{
	//LOG_PRINT(LogLevel::debug, "[SendMessage: %s]", buf);
    if(uv::GlobalConfig::BufferModeStatus == uv::GlobalConfig::NoBuffer)
    {
        //writeInLoop(buf, (unsigned int)size, nullptr);
        write(buf, (unsigned int)size);
    } else {
        uv::Packet packet;
        packet.pack(buf, size);
        write(packet.Buffer().c_str(), packet.PacketSize());
    }
}

void RRU::SendMessage2OAM(uv::Packet::MsgID msgID, std::string data)
{
    uv::Packet::Head head;
    CreateHead(uv::Packet::TO_OAM, head);
    head.s_msgID = to_string(msgID);
    SendPackMessage(head, data, data.length());
}

void RRU::ConnectResultProcess(uv::Packet& packet)
{
    /* connect bbu success, process delay info and send to bbu */
    /* delay 500ms for update hub delay info */
    std::this_thread::sleep_for(chrono::milliseconds(500)); // 延时 500ms
    SendRRRUDelayInfo();
}

void RRU::SendRRRUDelayInfo()
{
    uv::Packet::Head head;
    CreateHead(uv::Packet::TO_BBU, head);
    head.s_msgID = to_string(uv::Packet::MSG_DELAY_MEASUREMENT);
    std::string data = "T2a=123&Ta3=456";
    
    SendPackMessage(head, data, data.length());
}

void RRU::DataSetProcess(uv::Packet& packet)
{
    LOG_PRINT(LogLevel::debug, "Set Data:%s", packet.GetData().c_str());
    std::vector<std::string> key;
    std::vector<std::string> param = packet.DataSplit(packet.GetData(), "&");
    for(auto res : param)
    {
        LOG_PRINT(LogLevel::debug, "%s", res.c_str());
		
        key = packet.DataSplit(res, "=");
		switch(CALC_STRING_HASH(key[0])){
	        case "Reboot"_HASH:{
				if(key[1].compare("1"))
	            {
                    SendMessage2OAM(uv::Packet::MSG_SET_OAM, "Status=6");
					/* 重启设备操作 */
#if 0
			        if(_system("/sbin/reboot") < 0)
			        {
						LOG_PRINT(LogLevel::error, "system reboot execute error");
			            return ;
			        }
#endif
	            }
	            break;
	        }
		}
    }
}

void RRU::UpgradeProcess(uv::Packet& packet)
{
    std::thread back(std::bind(&RRU::UpgradeThread, this, packet));
    back.detach();
    this_thread::sleep_for(chrono::milliseconds(200));
}

void RRU::UpgradeThread(uv::Packet& packet)
{
    uv::Packet::Head head;
    CreateHead(uv::Packet::TO_OAM, head);
    head.s_msgID = to_string(uv::Packet::MSG_SET_OAM);
    std::string data = "Status=4";
    SendPackMessage(head, data, data.length());

    if(!FtpDownloadFile(packet))
    {
		LOG_PRINT(LogLevel::error, "FtpDownloadFile error");
        /* 给BBU 发送 ftp download 失败消息 */
        SendUpgradeFailure(packet, "4");
        return ;
    } else {
		LOG_PRINT(LogLevel::debug, "FtpDownloadFile success");
        /* 执行升级命令 */
        if(_system("/etc/user/user_update_sh") < 0)
        {
			LOG_PRINT(LogLevel::error, "system user_update_sh execute error");
            if(_system("/etc/user/user_update_error_sh") < 0)
            {
				LOG_PRINT(LogLevel::error, "system user_update_error_sh execute error");
            }
            /* 给BBU 发送 调用升级命令 失败消息 */
            SendUpgradeFailure(packet, "5");
            return ;
        }
        /* 写入升级版本 */
        write_file(SoftwareVersion, m_img_filename);

        /* 写入升级标志 */
	    write_file(UpgradeResult, "5");

        /* 重启设备操作 */
        if(_system("/sbin/reboot") < 0)
        {
			LOG_PRINT(LogLevel::error, "system reboot execute error");
            return ;
        }
    }
}

void RRU::SendUpgradeFailure(uv::Packet& packet, const std::string errorno)
{
    uv::Packet::Head head;
    CreateHead(uv::Packet::TO_OAM, head);
    head.s_msgID = to_string(uv::Packet::MSG_ALARM);

    std::string data = std::string("AlarmEvent=" + errorno);
    data += "&Status=3";
    
    SendPackMessage(head, data, data.length());
}

bool RRU::FtpDownloadFile(uv::Packet& packet)
{
    std::string fileName;
    std::string md5;
    std::map<std::string, std::string> map;
    packet.SplitData2Map(map);
    if(!FindDataMapValue(map, "fileName", fileName))
    {
		LOG_PRINT(LogLevel::error, "not find fileName");
        return false;
    }

    m_img_filename = fileName.substr(0, fileName.rfind('.'));
    
    if(!FindDataMapValue(map, "md5", md5))
    {
		LOG_PRINT(LogLevel::error, "not find md5");
        return false;
    }

    std::string ftpServerAddr = std::string(bbu_addr + ":21");
    ftplib *ftp = new ftplib();
	if(!ftp->Connect(ftpServerAddr.c_str()))
    {
		LOG_PRINT(LogLevel::error, "ftp connect error");
        return false;
    }

	if(!ftp->Login("anonymous", ""))
    {
		LOG_PRINT(LogLevel::error, "ftp login error");
        return false;
    }

    if(!ftp->Get("/etc/user/rHUP.tar", fileName.c_str(), ftplib::image))
    {
		LOG_PRINT(LogLevel::error, "ftp get file error");
        return false;
    }

	LOG_PRINT(LogLevel::debug, "file md5=%s", md5file("/etc/user/rHUP.tar").c_str());
    if(md5 != md5file("/etc/user/rHUP.tar"))
    {
		LOG_PRINT(LogLevel::error, "md5 check error");
        return false;
    }

	ftp->Quit();
    return true;
}

bool RRU::FindDataMapValue(std::map<std::string, std::string>& map, std::string key, std::string& value)
{
    auto rst = map.find(key);
    if(rst == map.end())
    {       
		LOG_PRINT(LogLevel::error, "not find dataMap key");
        return false;

    }   
    value = rst->second;
    return true;
}

int RRU::_system(std::string command)
{
    pid_t status;
    status = system(command.c_str());

    if(-1 == status){
		LOG_PRINT(LogLevel::error, "system error!");
        return -1;
    } else {
        if(WIFEXITED(status)){
            if(0 == WEXITSTATUS(status)){
                return 0;
            } else {
				LOG_PRINT(LogLevel::error, "run shell script fail, script exit code: %d", WEXITSTATUS(status));
                return -2;
            }
        } else {
			LOG_PRINT(LogLevel::error, "exit status: %d", WEXITSTATUS(status));
            return -3;
        }
    }
    return 0;
}

bool RRU::write_file(std::string file, const std::string& data)
{
	std::ofstream fout;
	fout.open(file);
	if(!fout.is_open())
	{
		LOG_PRINT(LogLevel::error, "open file error");
		return false;
	}
	fout << data << std::endl; 
	fout.close();
	return true;
}

bool RRU::read_file(std::string file, char* data, ssize_t size)
{
	std::ifstream fin;	
	
	fin.open(file);
	if(!fin.is_open())
	{
		LOG_PRINT(LogLevel::error, "open file error");
		return false;
	}
	fin.getline(data, size);
	fin.close();
	return true;
}

void RRU::Heart()
{
    LOG_PRINT(LogLevel::debug, "Start Heart ...");
    std::thread t1(std::bind(&RRU::HandleHeart, this, (void*)this));
    t1.detach();
}

void RRU::HandleHeart(void* arg)
{
    EventLoop loop;
    RRU* rru = (RRU*)arg;

    std::string data = "heartbeat";
    uv::Packet::Head head;
    CreateHead(uv::Packet::TO_BBU, head);
    head.s_msgID = to_string(uv::Packet::MSG_HEART_BEAT);

    uv::Timer timer(&loop, 30000, 30000,                                                                  
        [&rru, head, data](Timer*)
    {
        rru->HeartSendPackMessage(head, data, data.length());
    });
    timer.start();
    loop.run();
}

void RRU::CreateHead(uv::Packet::Destination dType, uv::Packet::Head& head)
{
    switch(dType)
    {
        case uv::Packet::Destination::TO_BBU:
            {
                head.s_destination = to_string(uv::Packet::Destination::TO_BBU);
            }
            break;
        case uv::Packet::Destination::TO_OAM:
            {
                head.s_destination = to_string(uv::Packet::Destination::TO_OAM);
            }
            break;
        default:
            {
                LOG_PRINT(LogLevel::error, "DeviceType error");
                head.s_destination = "X";
            }
            break;
    }

    head.s_source       = m_source;
	head.s_mac          = m_mac;
    head.s_state        = to_string(uv::Packet::RESPONSE);
    head.s_hop          = m_hop;
    head.s_port         = m_port;
    head.s_uport        = m_uport;
    head.s_uuport       = m_uuport;
}

size_t RRU::CALC_STRING_HASH(const string& str){
	// 获取string对象得字符串值并传递给HAHS_STRING_PIECE计算，获取得返回值为该字符串HASH值
	return HASH_STRING_PIECE(str.c_str());
}

