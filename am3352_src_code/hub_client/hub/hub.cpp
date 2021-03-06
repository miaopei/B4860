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
    uv::Timer* timer = new uv::Timer(loop_, 500, 0, [this](uv::Timer* ptr)
    {
        char* pdata = NULL;
        size_t size = 32;
        pdata = (char*)malloc(size * sizeof(char));
        if(pdata == NULL)
        {		
            LOG_PRINT(LogLevel::error, "malloc gateway memory error");
        }
#if 1
        GetDeviceGateWay(IFRNAME, pdata, size);
        LOG_PRINT(LogLevel::debug, "ReConnect Device GateWay: %s", pdata);
#endif
#if 0
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

void HUB::SendConnectMessage()
{
    std::string data = "ResultID=0";
    
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

    m_mac = pdata;
	m_source = to_string(uv::Packet::HUB);
    m_uuport = "X";
	
    int mpi_fd = gpmc_mpi_open(GPMC_MPI_DEV);
    /* 获取 rhub 的 port id 信息 */
    uint16_t rhub_port = get_rhup_port_id(mpi_fd, UP);

	LOG_PRINT(LogLevel::debug, "rhub_port=%d port=%d rruid=%d uport=%d", 
								rhub_port, ((rhub_port >> 8) & 0xf),
								((rhub_port >> 4) & 0xf), (rhub_port & 0xf));

    m_port = to_string(((rhub_port >> 8) & 0xf));
    m_hop = to_string(((rhub_port >> 4) & 0xf));
    m_uport = to_string((rhub_port & 0xf));

    gpmc_mpi_close(mpi_fd);

    
    free(pdata);
    pdata = NULL;
}

void HUB::SendRHUBDelayInfo()
{
    std::string data;
    RHUBDelayInfoCalculate(data);

    uv::Packet::Head head;
	CreateHead(uv::Packet::TO_BBU, head);
    head.s_msgID = to_string(uv::Packet::MSG_DELAY_MEASUREMENT);

    SendPackMessage(head, data, data.length());
}

void HUB::RHUBDelayInfoCalculate(std::string& data)
{
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

void HUB::ConnectResultProcess(uv::Packet& packet)
{
    SendRHUBDelayInfo();
}

void HUB::UpdataDelay(uv::Packet& packet)
{	
    std::string data;
    RHUBDelayInfoCalculate(data);

    uv::Packet::Head head;
    CreateHead(uv::Packet::TO_BBU, head);
    head.s_msgID = to_string(uv::Packet::MSG_UPDATE_DELAY);

    SendPackMessage(head, data, data.length());
}

void HUB::UpgradeProcess(uv::Packet& packet)
{
    std::thread back(std::bind(&HUB::UpgradeThread, this, packet));
    back.detach();
    this_thread::sleep_for(chrono::milliseconds(200));
}

void HUB::UpgradeThread(uv::Packet& packet)
{
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
        /* 重启设备操作 */
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
    CreateHead(uv::Packet::TO_BBU, head);
    head.s_msgID = packet.GetMsgID();
    head.s_source = packet.GetSource();
    head.s_destination = to_string(uv::Packet::TO_BBU);
	head.s_mac = packet.GetMac();
    head.s_state = to_string(uv::Packet::RESPONSE);
    head.s_msgID = packet.GetMsgID();
    head.s_hop = packet.GetHop();
    head.s_port = packet.GetPort();
    head.s_uport = packet.GetUPort();

    std::string data = std::string("ResultID=" + errorno);
    
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
