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
        connect(*(sockAddr.get()));
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
    head.s_source = m_source;
    head.s_destination = to_string(uv::Packet::TO_BBU);
	head.s_mac = m_mac;
    head.s_state = to_string(uv::Packet::REQUEST);
    head.s_msgID = to_string(uv::Packet::MSG_CONNECT);
    head.s_hop = m_hop;
    head.s_port = m_port;
    head.s_uport = m_uport;

    SendPackMessage(head, data, data.length());
}

void HUB::SetRHUBInfo()
{
#if 0
    int mpi_fd = gpmc_mpi_open(GPMC_MPI_DEV);
    /* 获取 rhub 的 port id 信息 */
    uint16_t rhub_port = get_rhup_port_id(mpi_fd, UP);

    std::cout << "rhub_port = " << rhub_port << std::endl;
    std::cout << "port = " << ((rhub_port >> 8) & 0xf) << std::endl;
    std::cout << "rruid = " << ((rhub_port >> 4) & 0xf) << std::endl;
    std::cout << "uport = " << (rhub_port & 0xf) << std::endl;

    m_source = to_string(uv::Packet::HUB);
    m_port = to_string(((rhub_port >> 8) & 0xf));
    m_rruid = to_string(((rhub_port >> 4) & 0xf));
    m_uport = to_string((rhub_port & 0xf));

    gpmc_mpi_close(mpi_fd);
#endif
#if 0
	uv::Packet packet;
	char mac[32] = {0};
	if(!packet.GetDeviceMac(IFRNAME, mac))
    {
        std::cout << "Error: GetMac error" << std::endl;
        return ;
    }
#endif
	//m_mac = mac;
	m_mac = "F48E38DCD7CA";
    m_source = "0";
    m_port = "0";
    m_hop = "1";
    m_uport = "2";
}

void HUB::SendRHUBDelayInfo()
{
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

#if 0
    rhub_port = 16
    port = 0
    rruid = 1
    uport = 0
    delay1 = 1175
    delay2 = 24
    delay3 = 24
    delay4 = 24
    delay5 = 24
    delay6 = 24
    delay7 = 24
    delay8 = 24
    delay9 = 1
    rhub级联口的T14 = 1097088
    rhub下联 0 号口的T14 = 153
    rhub下联 1 号口的T14 = 1096878
    rhub下联 2 号口的T14 = 1096878
    rhub下联 3 号口的T14 = 1096878
    rhub下联 4 号口的T14 = 1096878
    rhub下联 5 号口的T14 = 1096878
    rhub下联 6 号口的T14 = 1096878
    rhub下联 7 号口的T14 = 1096878
#endif
}

void HUB::RecvMessage(const char* buf, ssize_t size)
{
    if(size < HEADLENGTH)
    {
        std::cout << "Message Length error." << std::endl;
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
			std::cout << "[ReceiveData: " << packet.DataSize() << ":" << packet.getData() << "]" << std::endl;
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
            std::cout << "[RCV:msg_connect]" << std::endl;
            ConnectResultProcess(packet);
            break;
        case uv::Packet::MSG_UPGRADE:
            std::cout << "[RCV:msg_upgrade]" << std::endl;
            UpgradeProcess(packet);            
            break;
        case uv::Packet::MSG_UPDATE_DELAY:
            std::cout << "[RCV:msg_updata_delay]" << std::endl;
            UpdataDelay(packet);
            break;
        default:
            std::cout << "[Error: MessageID Error]" << std::endl;
    }
}

void HUB::SendPackMessage(uv::Packet::Head& head, std::string& data, ssize_t size)
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
    std::cout << "[SendMessage: " << buf << "]" << std::endl;
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
    //SendRHUBDelayInfo();
    TestProcess(packet);
}

void HUB::UpdataDelay(uv::Packet& packet)
{	
    uv::Packet::Head head;
    head.s_source = to_string(uv::Packet::HUB);
    head.s_destination = to_string(uv::Packet::TO_BBU);
	head.s_mac = m_mac;
    head.s_state = to_string(uv::Packet::RESPONSE);
    head.s_msgID = to_string(uv::Packet::MSG_UPDATE_DELAY);
    head.s_hop = m_hop;
    head.s_port = m_port;
    head.s_uport = m_uport;

    //TestProcess(packet);
    std::string data = "delay1_up=34&delay2_up=35&delay3_up=36&delay4_up=37&delay5_up=38&delay1_down=36&delay2_down=37&delay3_down=38&delay4_down=39&delay5_down=37&t14_delay1=11488&t14_delay2=11488&t14_delay3=11488&t14_delay4=11488&t14_delay5=11488";
    
    SendPackMessage(head, data, data.length());
}

void HUB::UpgradeProcess(uv::Packet& packet)
{
#if 0
    std::thread t1(std::bind(&HUB::UpgradeThread, this, packet));
    t1.join();
#endif

#if 1
    std::thread back(std::bind(&HUB::UpgradeThread, this, packet));
    back.detach();
    //this_thread::sleep_for(chrono::milliseconds(200));
#endif
}

void HUB::UpgradeThread(uv::Packet& packet)
{
    if(!FtpDownloadFile(packet))
    {
        std::cout << "Error: FtpDownloadFile error" << std::endl;
        /* 给BBU 发送 ftp download 失败消息 */
        SendUpgradeFailure(packet, "4");
        return ;
    } else {
        std::cout << "Info: FtpDownloadFile success" << std::endl;
        /* 执行升级命令 */
        if(_system("/etc/user/user_update_sh") < 0)
        {
            std::cout << "Error: system user_update_sh execute error" << std::endl;
            if(_system("/etc/user/user_update_error_sh") < 0)
            {
                std::cout << "Error: system user_update_error_sh execute error" << std::endl;
            }
            /* 给BBU 发送 调用升级命令 失败消息 */
            SendUpgradeFailure(packet, "5");
            return ;
        }
        /* 重启设备操作 */
        if(_system("/sbin/reboot") < 0)
        {
            std::cout << "Error: system reboot execute error" << std::endl;
            return ;
        }
    }
}

void HUB::SendUpgradeFailure(uv::Packet& packet, const std::string errorno)
{
    uv::Packet::Head head;
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
        std::cout << __FUNCTION__ << ":" << __LINE__ << ":" 
                  << " > Error: not find fileName" << std::endl;
        return false;
    }
    
    if(!FindDataMapValue(map, "md5", md5))
    {
        std::cout << __FUNCTION__ << ":" << __LINE__ << ":" 
                  << " > Error: not find md5" << std::endl;
        return false;
    }

    std::string ftpServerAddr = std::string(bbu_addr + ":21");
    ftplib *ftp = new ftplib();
	if(!ftp->Connect(ftpServerAddr.c_str()))
    {
        std::cout << "Error: ftp connect error" << std::endl;
        return false;
    }

	if(!ftp->Login("anonymous", ""))
    {
        std::cout << "Error: ftp login error" << std::endl;
        return false;
    }

    if(!ftp->Get("/etc/user/rHUP.tar", fileName.c_str(), ftplib::image))
    {
        std::cout << "Error: ftp get file error" << std::endl;
        return false;
    }

    std::cout << "file md5=" << md5file("/etc/user/rHUP.tar") << std::endl;
    if(md5 != md5file("/etc/user/rHUP.tar"))
    {
        std::cout << "Error: md5 check error" << std::endl;
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
        std::cout << "Error: not find dataMap key" << std::endl;
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
        std::cout << "Error: system error!" << std::endl;
        return -1;
    } else {
        if(WIFEXITED(status)){
            if(0 == WEXITSTATUS(status)){
                return 0;
            } else {
                std::cout << "Error: run shell script fail, script exit code: " << WEXITSTATUS(status) << std::endl;
                return -2;
            }
        } else {
            std::cout << "Error: exit status: " << WEXITSTATUS(status) << std::endl;
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
		std::cout << "Error: open file error" << std::endl;
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
		std::cout << "Error: open file error" << std::endl;
		return false;
	}
	fin.getline(data, size);
	fin.close();
	return true;
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
    
    head.s_source = to_string(uv::Packet::HUB);
    head.s_destination = to_string(uv::Packet::TO_BBU);
	head.s_mac = m_mac;
    head.s_state = to_string(uv::Packet::RESPONSE);
    head.s_msgID = to_string(uv::Packet::MSG_DELAY_MEASUREMENT);
    head.s_hop = m_hop;
    head.s_port = m_port;
    head.s_uport = m_uport;

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



