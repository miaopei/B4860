/*************************************************************************
	> File Name: hub.cpp
	> Author: miaopei
	> Mail: miaopei@baicells.com 
	> Created Time: 2020年02月12日 星期三 11时16分50秒
 ************************************************************************/

#include <iostream>
#include <string.h>

#include "hub.h"

using namespace uv;
using namespace std;

HUB::HUB(uv::EventLoop* loop)
    :TcpClient(loop),
    sockAddr(nullptr)
{
    setConnectStatusCallback(std::bind(&HUB::onConnect, this, std::placeholders::_1));
    setMessageCallback(std::bind(&HUB::RecvMessage, this, std::placeholders::_1, std::placeholders::_2));

    SetRHUBInfo();
    m_img_filename = "";
}

void HUB::connectToServer(uv::SocketAddr& addr)
{
    sockAddr = std::make_shared<uv::SocketAddr>(addr);
    connect(addr);
}

void HUB::onConnect(ConnectStatus status)
{
    if(status != ConnectStatus::OnConnectSuccess)
    {
        reConnect();
    } else {
        SendConnectMessage();
    }
}

void HUB::reConnect()
{
    uv::Timer* timer = new uv::Timer(loop_, 1000, 0, [this](uv::Timer* ptr)
    {
#if 0
		if(_system("echo \"`date '+%Y-%m-%d %H:%M:%S'`: LinkDown Reboot HUB.\" >> /etc/user/Snapshoot") < 0)
        {
            LOG_PRINT(LogLevel::error, "system echo execute error");
            return ;
        }

		if(_system("/sbin/reboot") < 0)
        {
			LOG_PRINT(LogLevel::error, "system reboot execute error");
            return ;
        }
#endif
#if 1
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
#endif

        ptr->close([](uv::Timer* ptr)
        {
            delete ptr;    
        });
    });
    timer->start();
}

void HUB::SendConnectMessage()
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

    if(eeprom_info.module_type.empty())
    {
        data += "&ModelName=NA";
    } else {
        data += "&ModelName=" + eeprom_info.module_type;
    }

    if(eeprom_info.sn.empty())
    {
        data += "&SerialNumber=NA";
    } else {
        data += "&SerialNumber=" + eeprom_info.sn;
    }

    if(eeprom_info.board_ver.empty())
    {
        data += "&HardwareVersion=NA";
    } else {
        data += "&HardwareVersion=" + eeprom_info.board_ver;
    }

    uv::Packet::Head head;
    CreateHead(uv::Packet::TO_BBU, head);
    head.s_msgID = to_string(uv::Packet::MSG_CONNECT);

    SendPackMessage(head, data, data.length());
}

void HUB::SetRHUBInfo()
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
    m_source    = to_string(uv::Packet::HUB);
    m_port      = "0";
    m_hop       = "1";
    m_uport     = "1";
    m_uuport    = "X";
 
#if 0
    int mpi_fd = gpmc_mpi_open(GPMC_MPI_DEV);
    /* 获取 rhub 的 port id 信息 */
    uint16_t rhub_port = get_rhup_port_id(mpi_fd, UP);

	LOG_PRINT(LogLevel::debug, "rhub_port=%d port=%d rruid=%d uport=%d", 
								rhub_port, ((rhub_port >> 8) & 0xf),
								((rhub_port >> 4) & 0xf), (rhub_port & 0xf));

    m_port  = to_string(((rhub_port >> 8) & 0xf));
    m_hop   = to_string(((rhub_port >> 4) & 0xf));
    m_uport = to_string((rhub_port & 0xf));

    gpmc_mpi_close(mpi_fd);
#endif
   
    free(pdata);
    pdata = NULL;
}

void HUB::SendRHUBDelayInfo()
{
#if 0
    std::string data;
    RHUBDelayInfoCalculate(data);

    uv::Packet::Head head;
    CreateHead(uv::Packet::TO_BBU, head);
    head.s_msgID = to_string(uv::Packet::MSG_DELAY_MEASUREMENT);

    SendPackMessage(head, data, data.length());
#endif

#if 0
    int mpi_fd = gpmc_mpi_open(GPMC_MPI_DEV);
    
    struct delay_measurement_info* dm_info = (struct delay_measurement_info*)malloc(sizeof(struct delay_measurement_info));

    /* 获取 rhub 的处理时延  */
    //struct rhup_data_delay* rhup_delay = (struct rhup_data_delay*)malloc(sizeof(struct rhup_data_delay));
    get_rhup_delay(mpi_fd, UP, dm_info->rhup_delay);

#if 0
    std::cout << "delay1 = " << rhup_delay->delay1 
              << "\ndelay2 = " << rhup_delay->delay2
              << "\ndelay3 = " << rhup_delay->delay3
              << "\ndelay4 = " << rhup_delay->delay4
              << "\ndelay5 = " << rhup_delay->delay5
              << "\ndelay6 = " << rhup_delay->delay6
              << "\ndelay7 = " << rhup_delay->delay7
              << "\ndelay8 = " << rhup_delay->delay8
              << "\ndelay9 = " << rhup_delay->delay9
              << std::endl;
#endif

    /* 获取 rhub T14 测量时延信息 */
    //struct rhup_t14_delay* t14_delay = (struct rhup_t14_delay*)malloc(sizeof(struct rhup_t14_delay));
    get_rhup_t14_delay(mpi_fd, dm_info->t14_delay);

#if 0
    std::cout << "rhub级联口的T14 = " << t14_delay->delay1
              << "\nrhub下联 0 号口的T14 = " << t14_delay->delay2
              << "\nrhub下联 1 号口的T14 = " << t14_delay->delay3
              << "\nrhub下联 2 号口的T14 = " << t14_delay->delay4
              << "\nrhub下联 3 号口的T14 = " << t14_delay->delay5
              << "\nrhub下联 4 号口的T14 = " << t14_delay->delay6
              << "\nrhub下联 5 号口的T14 = " << t14_delay->delay7
              << "\nrhub下联 6 号口的T14 = " << t14_delay->delay8
              << "\nrhub下联 7 号口的T14 = " << t14_delay->delay9
              << std::endl;

    std::cout << "Toffset = " << TOFFSET << std::endl;
#endif

    /* 封装时延测量数据并发送到 BBU */ 

#if 1
    std::cout << "delay1 = " << dm_info->rhup_delay->delay1 
              << "\ndelay2 = " << dm_info->rhup_delay->delay2
              << "\ndelay3 = " << dm_info->rhup_delay->delay3
              << "\ndelay4 = " << dm_info->rhup_delay->delay4
              << "\ndelay5 = " << dm_info->rhup_delay->delay5
              << "\ndelay6 = " << dm_info->rhup_delay->delay6
              << "\ndelay7 = " << dm_info->rhup_delay->delay7
              << "\ndelay8 = " << dm_info->rhup_delay->delay8
              << "\ndelay9 = " << dm_info->rhup_delay->delay9
              << std::endl;
#endif

#if 1
    std::cout << "rhub级联口的T14 = " << dm_info->t14_delay->delay1
              << "\nrhub下联 0 号口的T14 = " << dm_info->t14_delay->delay2
              << "\nrhub下联 1 号口的T14 = " << dm_info->t14_delay->delay3
              << "\nrhub下联 2 号口的T14 = " << dm_info->t14_delay->delay4
              << "\nrhub下联 3 号口的T14 = " << dm_info->t14_delay->delay5
              << "\nrhub下联 4 号口的T14 = " << dm_info->t14_delay->delay6
              << "\nrhub下联 5 号口的T14 = " << dm_info->t14_delay->delay7
              << "\nrhub下联 6 号口的T14 = " << dm_info->t14_delay->delay8
              << "\nrhub下联 7 号口的T14 = " << dm_info->t14_delay->delay9
              << std::endl;

    std::cout << "Toffset = " << TOFFSET << std::endl;
#endif

    uv::Packet packet;

    packet.SetHead(to_string(uv::Packet::HUB),
                   to_string(uv::Packet::TO_BBU),
                   to_string(uv::Packet::RESPONSE),
                   to_string(uv::Packet::MSG_DELAY_MEASUREMENT),
                   m_rruid, m_port, m_uport);

    

    //free(rhup_delay);
    //free(t14_delay);
    free(dm_info);
    gpmc_mpi_close(mpi_fd);
#endif
}

void HUB::RHUBDelayInfoCalculate(std::string& data)
{
#if 0
    int mpi_fd = gpmc_mpi_open(GPMC_MPI_DEV);
    
    /* 获取 rhub 的处理时延  */
    struct rhup_data_delay* rhub_up = (struct rhup_data_delay*)malloc(sizeof(struct rhup_data_delay));
    get_rhup_delay(mpi_fd, UP, rhub_up);
    data = std::string("delay1_up=" + to_string(rhub_up->delay1) + 
								   "&delay2_up=" + to_string(rhub_up->delay2) +
								   "&delay3_up=" + to_string(rhub_up->delay3) +
								   "&delay4_up=" + to_string(rhub_up->delay4) +
								   "&delay5_up=" + to_string(rhub_up->delay5) +
								   "&delay6_up=" + to_string(rhub_up->delay6) +
								   "&delay7_up=" + to_string(rhub_up->delay7) +
								   "&delay8_up=" + to_string(rhub_up->delay8) +
								   "&delay9_up=" + to_string(rhub_up->delay9));

    struct rhup_data_delay* rhub_down = (struct rhup_data_delay*)malloc(sizeof(struct rhup_data_delay));
    get_rhup_delay(mpi_fd, DOWN, rhub_down);
	data += std::string("&delay1_down=" + to_string(rhub_down->delay1) + 
						"&delay2_down=" + to_string(rhub_down->delay2) +
						"&delay3_down=" + to_string(rhub_down->delay3) +
						"&delay4_down=" + to_string(rhub_down->delay4) +
						"&delay5_down=" + to_string(rhub_down->delay5) +
						"&delay6_down=" + to_string(rhub_down->delay6) +
						"&delay7_down=" + to_string(rhub_down->delay7) +
						"&delay8_down=" + to_string(rhub_down->delay8) +
						"&delay9_down=" + to_string(rhub_down->delay9));
    
    /* 获取 rhub T14 测量时延信息 */
    struct rhup_t14_delay* t14_delay = (struct rhup_t14_delay*)malloc(sizeof(struct rhup_t14_delay));
    get_rhup_t14_delay(mpi_fd, t14_delay);
	data += std::string("&t14_delay1=" + to_string(t14_delay->delay1) + 
						"&t14_delay2=" + to_string(t14_delay->delay2) +
						"&t14_delay3=" + to_string(t14_delay->delay3) +
						"&t14_delay4=" + to_string(t14_delay->delay4) +
						"&t14_delay5=" + to_string(t14_delay->delay5) +
						"&t14_delay6=" + to_string(t14_delay->delay6) +
						"&t14_delay7=" + to_string(t14_delay->delay7) +
						"&t14_delay8=" + to_string(t14_delay->delay8) +
						"&t14_delay9=" + to_string(t14_delay->delay9));

    //data += std::string("&toffset=" + to_string(TOFFSET));
    
    free(rhub_up);
    free(rhub_down);
    free(t14_delay);
    gpmc_mpi_close(mpi_fd);
#endif
}

void HUB::RecvMessage(const char* buf, ssize_t size)
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

void HUB::ProcessRecvMessage(uv::Packet& packet)
{
	switch(std::stoi(packet.GetMsgID()))
    {
        case uv::Packet::MSG_CONNECT:
            ConnectResultProcess(packet);
            break;
        case uv::Packet::MSG_UPGRADE:
            UpgradeProcess(packet);            
            break;
        case uv::Packet::MSG_UPDATE_DELAY:
            UpdataDelay(packet);
            break;
        case uv::Packet::MSG_SET:                                                                  
            DataSetProcess(packet);
            break;
        default:
			LOG_PRINT(LogLevel::error, "MessageID Error");
    }
}

void HUB::SendPackMessage(uv::Packet::Head& head, std::string& data, ssize_t size)
{
    uv::Packet packet;
    packet.SetHead(head);

    packet.PackMessage(data, size);

    /* 打印数据封装信息 */
    packet.EchoPackMessage();
    
    std::string send_buf = packet.GetPacket();
    
    SendMessage(send_buf.c_str(), send_buf.length());
}

void HUB::HeartSendPackMessage(uv::Packet::Head head, std::string data, ssize_t size)
{
    uv::Packet packet;
    packet.SetHead(head);

    packet.PackMessage(data, size);

    /* 打印数据封装信息 */
    //packet.EchoPackMessage();
    
    std::string send_buf = packet.GetPacket();
    
    SendMessage(send_buf.c_str(), send_buf.length());
}

void HUB::SendMessage(const char* buf, ssize_t size)
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

void HUB::SendMessage2OAM(uv::Packet::MsgID msgID, std::string data)
{
    uv::Packet::Head head;
    CreateHead(uv::Packet::TO_OAM, head);
    head.s_msgID = to_string(msgID);
    SendPackMessage(head, data, data.length());
}

void HUB::ConnectResultProcess(uv::Packet& packet)
{
    //SendRHUBDelayInfo();
    TestProcess(packet);
}

void HUB::UpdataDelay(uv::Packet& packet)
{	
    //std::string data;
    //RHUBDelayInfoCalculate(data);

    uv::Packet::Head head;
    CreateHead(uv::Packet::TO_BBU, head);
    head.s_msgID = to_string(uv::Packet::MSG_UPDATE_DELAY);

    //TestProcess(packet);
    std::string data = "delay1_up=34&delay2_up=35&delay3_up=36&delay4_up=37&delay5_up=38&delay1_down=36&delay2_down=37&delay3_down=38&delay4_down=39&delay5_down=37&t14_delay1=11488&t14_delay2=11488&t14_delay3=11488&t14_delay4=11488&t14_delay5=11488";
    
    SendPackMessage(head, data, data.length());
}

void HUB::DataSetProcess(uv::Packet& packet)
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
				if(key[1].compare("1") == 0)
	            {
                    SendMessage2OAM(uv::Packet::MSG_SET_OAM, "Status=6");
                    
                    if(_system("echo \"`date '+%Y-%m-%d %H:%M:%S'`: OAM Reboot HUB.\" >> /etc/user/Snapshoot") < 0)
			        {
						LOG_PRINT(LogLevel::error, "system echo execute error");
			            return ;
			        }

                    /* 重启设备操作 */
                    std::this_thread::sleep_for(chrono::milliseconds(1000)); // 延时 1s
			        if(_system("/sbin/reboot") < 0)
			        {
						LOG_PRINT(LogLevel::error, "system reboot execute error");
			            return ;
			        }
	            }
	            break;
	        }
		}
    }
}

void HUB::UpgradeProcess(uv::Packet& packet)
{
    std::thread back(std::bind(&HUB::UpgradeThread, this, packet));
    back.detach();
    this_thread::sleep_for(chrono::milliseconds(200));
}

void HUB::UpgradeThread(uv::Packet& packet)
{
    SendMessage2OAM(uv::Packet::MSG_SET_OAM, "Status=4");

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
            SendUpgradeFailure(packet, "3");
            return ;
        }
        /* 写入升级版本 */
        write_file(SoftwareVersion, m_img_filename);

        /* 写入升级标志 */
	    write_file(UpgradeResult, "5");

        if(_system("echo \"`date '+%Y-%m-%d %H:%M:%S'`: Upgrade Reboot HUB.\" >> /etc/user/Snapshoot") < 0)
        {
            LOG_PRINT(LogLevel::error, "system echo execute error");
            return ;
        }

        /* 重启设备操作 */
        std::this_thread::sleep_for(chrono::milliseconds(1000)); // 延时 1s
        if(_system("/sbin/reboot") < 0)
        {
			LOG_PRINT(LogLevel::error, "system reboot execute error");
            return ;
        }
    }
}

void HUB::SendUpgradeFailure(uv::Packet& packet, const std::string errorno)
{
    uv::Packet::Head head;
    CreateHead(uv::Packet::TO_OAM, head);
    head.s_msgID = to_string(uv::Packet::MSG_ALARM);

    std::string data = std::string("AlarmEvent=" + errorno);
    data += "&Status=3";
    
    SendPackMessage(head, data, data.length());
}

bool HUB::FtpDownloadFile(uv::Packet& packet)
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

bool HUB::FindDataMapValue(std::map<std::string, std::string>& map, std::string key, std::string& value)
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

int HUB::_system(std::string command)
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


bool HUB::write_file(std::string file, const std::string& data)
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

bool HUB::read_file(std::string file, char* data, ssize_t size)
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


void HUB::Heart()
{
    LOG_PRINT(LogLevel::debug, "Start Heart ...");
    std::thread t1(std::bind(&HUB::HandleHeart, this, (void*)this));
    t1.detach();
}

void HUB::HandleHeart(void* arg)
{
    EventLoop loop;
    HUB* hub = (HUB*)arg;

    std::string data = "heartbeat";
    uv::Packet::Head head;
    CreateHead(uv::Packet::TO_BBU, head);
    head.s_msgID = to_string(uv::Packet::MSG_HEART_BEAT);

    uv::Timer timer(&loop, 30000, 30000,                                                                  
        [&hub, head, data](Timer*)
    {
        hub->HeartSendPackMessage(head, data, data.length());
    });
    timer.start();
    loop.run();
}

void HUB::CreateHead(uv::Packet::Destination dType, uv::Packet::Head& head)
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

size_t HUB::CALC_STRING_HASH(const string& str){
	// 获取string对象得字符串值并传递给HAHS_STRING_PIECE计算，获取得返回值为该字符串HASH值
	return HASH_STRING_PIECE(str.c_str());
}

void HUB::EepromInfoRead()
{
    ifstream fin(EepromInfo);
    if(!fin)
    {
        LOG_PRINT(LogLevel::error, "Parameter file misseed.");
        return;
    }

    while(!fin.eof())
    {
        std::string str;
        getline(fin, str);
        int pos = str.find("=");
        if(pos == -1)
            continue;

        std::string key = str.substr(0, pos);
        std::string value = str.substr(pos+1, str.length());

        switch(CALC_STRING_HASH(key)){
        case "SN"_HASH:
            {
                eeprom_info.sn = value;
                LOG_PRINT(LogLevel::debug, "SN=%s", value.c_str());
                break;
            }
        case "MAC"_HASH:
            {
                eeprom_info.mac = value;
                LOG_PRINT(LogLevel::debug, "MAC=%s", value.c_str());
                break;
            }
        case "AGING"_HASH:
            {
                eeprom_info.aging = value;
                LOG_PRINT(LogLevel::debug, "AGING=%s", value.c_str());
                break;
            }
        case "AGING_MASK"_HASH:
            {
                eeprom_info.aging_mask = value;
                LOG_PRINT(LogLevel::debug, "AGING_MASK=%s", value.c_str());
                break;
            }
        case "AGING_COUNT"_HASH:
            {
                eeprom_info.aging_count = value;
                LOG_PRINT(LogLevel::debug, "AGING_COUNT=%s", value.c_str());
                break;
            }
        case "AGING_FLAG"_HASH:
            {
                eeprom_info.aging_flag = value;
                LOG_PRINT(LogLevel::debug, "AGING_FLAG=%s", value.c_str());
                break;
            }
        case "BOARD_VER"_HASH:
            {
                eeprom_info.board_ver = value;
                LOG_PRINT(LogLevel::debug, "BOARD_VER=%s", value.c_str());
                break;
            }
        case "MODULE_TYPE"_HASH:
            {
                eeprom_info.module_type = value;
                LOG_PRINT(LogLevel::debug, "MODULE_TYPE=%s", value.c_str());
                break;
            }
        }
    }
}









void HUB::TestProcess(uv::Packet& packet)
{
    uv::Packet::Head head;

    struct rhub_data_delay* rhub_up = (struct rhub_data_delay*)malloc(sizeof(struct rhub_data_delay));
    TestGetRhubDelay(0, rhub_up);
    std::string data = std::string("delay1_up=" + to_string(rhub_up->delay1) + 
								   "&delay2_up=" + to_string(rhub_up->delay2) +
								   "&delay3_up=" + to_string(rhub_up->delay3) +
								   "&delay4_up=" + to_string(rhub_up->delay4) +
								   "&delay5_up=" + to_string(rhub_up->delay5));

    struct rhub_data_delay* rhub_down = (struct rhub_data_delay*)malloc(sizeof(struct rhub_data_delay));
    TestGetRhubDelay(1, rhub_down);
	data += std::string("&delay1_down=" + to_string(rhub_down->delay1) + 
						"&delay2_down=" + to_string(rhub_down->delay2) +
						"&delay3_down=" + to_string(rhub_down->delay3) +
						"&delay4_down=" + to_string(rhub_down->delay4) +
						"&delay5_down=" + to_string(rhub_down->delay5));

    struct rhub_t14_delay* t14_delay = (struct rhub_t14_delay*)malloc(sizeof(struct rhub_t14_delay));
    TestGetRhubT14Delay(t14_delay);
	data += std::string("&t14_delay1=" + to_string(t14_delay->delay1) + 
						"&t14_delay2=" + to_string(t14_delay->delay2) +
						"&t14_delay3=" + to_string(t14_delay->delay3) +
						"&t14_delay4=" + to_string(t14_delay->delay4) +
						"&t14_delay5=" + to_string(t14_delay->delay5));
    
    CreateHead(uv::Packet::TO_BBU, head);
    head.s_msgID = to_string(uv::Packet::MSG_DELAY_MEASUREMENT);

    SendPackMessage(head, data, data.length());

    free(rhub_up);
    free(rhub_down);
    free(t14_delay);
}

void HUB::TestGetRhubDelay(uint8_t dir, struct rhub_data_delay* rhub_delay)
{
    if(dir == 0)
    {
        rhub_delay->delay1 = 24;
        rhub_delay->delay2 = 25;
        rhub_delay->delay3 = 25;
        rhub_delay->delay4 = 26;
        rhub_delay->delay5 = 27;
    } else if (dir == 1) {
        rhub_delay->delay1 = 26;
        rhub_delay->delay2 = 27;
        rhub_delay->delay3 = 28;
        rhub_delay->delay4 = 27;
        rhub_delay->delay5 = 29;
    }
}

void HUB::TestGetRhubT14Delay(struct rhub_t14_delay* t14_delay)
{
    t14_delay->delay1 = 11456;
    t14_delay->delay2 = 11457;
    t14_delay->delay3 = 11458;
    t14_delay->delay4 = 11459;
    t14_delay->delay5 = 11457;
}


