#include <iostream>
#include <cstring>
#include "include/Packet.h"
#include "include/LogWriter.h"

using namespace uv;


uint8_t Packet::HeadByte = 0x7e;
uint8_t Packet::EndByte = 0xe7;
Packet::DataMode Packet::Mode = Packet::DataMode::LittleEndian;

Packet::Packet()
    :m_length(0),
    m_data(""),
    m_packet(""),
    buffer_(""),
    dataSize_(0)
{

}

uv::Packet::~Packet()
{

}

void uv::Packet::SetHead(Head& head)
{
	m_source = head.s_source;
	m_destination = head.s_destination;
	m_mac = head.s_mac;
	m_state = head.s_state;
	m_msgID = head.s_msgID;
	m_hop = head.s_hop;
	m_port = head.s_port;
	m_uport = head.s_uport;
}

std::string uv::Packet::num2str(int num)
{
    char ss[10];
    sprintf(ss, "%04d", num);
    return ss;
}

void uv::Packet::PackMessage(std::string& data, size_t size)
{
    if(size >= 10000)
    {
		LOG_PRINT(LogLevel::error, "The data is too big");
        return;
    }

	m_data = data;
    m_length = size;
	m_packet = std::string(GetHead() + num2str(size) + data);	 
}

void uv::Packet::UnPackMessage()
{
	std::string packet = std::string(getData(), DataSize());
	// 需要增加 packet 解析校验
	m_packet = packet;
	m_source = packet.substr(0, 1);
	m_destination= packet.substr(1, 1);
	m_mac = packet.substr(2, 12);
	m_state = packet.substr(14, 1);
	m_msgID = packet.substr(15, 4);
	m_hop = packet.substr(19, 1);
	m_port = packet.substr(20, 1);
	m_uport = packet.substr(21, 1);
    m_length = std::stoi(packet.substr(22, 4));
	m_data = packet.substr(26);
}

std::string uv::Packet::GetPacket() 
{ 
	return m_packet;
}

std::string uv::Packet::GetHead() 
{ 
	return std::string(m_source + m_destination + m_mac + m_state + m_msgID + m_hop + m_port + m_uport); 
}

std::string uv::Packet::GetSource() 
{ 
	return m_source; 
}

std::string uv::Packet::GetDestination()
{
	return m_destination;
}

std::string uv::Packet::GetMac()
{
	return m_mac;
}

std::string uv::Packet::GetState()
{
	return m_state;
}

std::string uv::Packet::GetMsgID() 
{ 
	return m_msgID; 
}

std::string uv::Packet::GetHop()
{
	return m_hop;
}

std::string uv::Packet::GetPort()
{
	return m_port;
}

std::string uv::Packet::GetUPort()
{
	return m_uport;
}

int uv::Packet::GetLength()
{
    return m_length;
}

std::string uv::Packet::GetData() 
{
	return m_data;
}

void uv::Packet::EchoPackMessage()
{
	LOG_PRINT(LogLevel::debug, "Pack packet:\n\tPacket: %s\
								\n\tSource: %s [HUB,RRU,BBU,OAM]\
								\n\tDestination: %s [HUB,RRU,BBU,OAM]\
								\n\tMac: %s\
								\n\tState: %s [REQUEST,RESPONSE]\
								\n\tMsgID: %s\
								\n\tHOP: %s\
								\n\tPort: %s\
								\n\tUPort: %s\
								\n\tLength: %d\
								\n\tData: %s", 
								m_packet.c_str(), m_source.c_str(),
								m_destination.c_str(), m_mac.c_str(),
								m_state.c_str(), m_msgID.c_str(),
								m_hop.c_str(), m_port.c_str(),
								m_uport.c_str(), m_length, m_data.c_str());
}

void uv::Packet::EchoUnPackMessage()
{
	LOG_PRINT(LogLevel::debug, "UnPack packet:\n\tPacket: %s\
								\n\tSource: %s [HUB,RRU,BBU,OAM]\
								\n\tDestination: %s [HUB,RRU,BBU,OAM]\
								\n\tMac: %s\
								\n\tState: %s [REQUEST,RESPONSE]\
								\n\tMsgID: %s\
								\n\tHOP: %s\
								\n\tPort: %s\
								\n\tUPort: %s\
								\n\tLength: %d\
								\n\tData: %s", 
								m_packet.c_str(), m_source.c_str(),
								m_destination.c_str(), m_mac.c_str(),
								m_state.c_str(), m_msgID.c_str(),
								m_hop.c_str(), m_port.c_str(),
								m_uport.c_str(), m_length, m_data.c_str());
}

std::vector<std::string> uv::Packet::DataSplit(const std::string& in, const std::string& delim)
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

void uv::Packet::SplitData2Map(std::map<std::string, std::string>& map)
{
    vector<string>ret = DataSplit(GetData().c_str(), "&");
	
    int ret_size = ret.size();
    for(int i = 0; i < ret_size; i++)
    {
        vector<string>kv = DataSplit(ret[i].c_str(), "=");
		map.insert(std::make_pair(kv[0].c_str(), kv[1].c_str()));
    }
}

bool uv::Packet::GetDeviceMac(const char* inet, char* mac)
{
    int fd, interface;
    struct ifreq buf[MAXINTERFACES];
    struct ifconf ifc;

    if((fd = socket(AF_INET, SOCK_DGRAM, 0)) >= 0)
    {
        int i = 0;
        ifc.ifc_len = sizeof(buf);
        ifc.ifc_buf = (caddr_t)buf;
        if (!ioctl(fd, SIOCGIFCONF, (char *)&ifc))
        {
            interface = ifc.ifc_len / sizeof(struct ifreq);
            //printf("interface num is %d\n", interface);
            while (i < interface)
            {
                //printf("net device %s\n", buf[i].ifr_name);
                if(!strcmp(inet, buf[i].ifr_name))
                {
                    if (!(ioctl(fd, SIOCGIFHWADDR, (char *)&buf[i])))
                    {
                        sprintf(mac, "%02X%02X%02X%02X%02X%02X",
                                (unsigned char)buf[i].ifr_hwaddr.sa_data[0],
                                (unsigned char)buf[i].ifr_hwaddr.sa_data[1],
                                (unsigned char)buf[i].ifr_hwaddr.sa_data[2],
                                (unsigned char)buf[i].ifr_hwaddr.sa_data[3],
                                (unsigned char)buf[i].ifr_hwaddr.sa_data[4],
                                (unsigned char)buf[i].ifr_hwaddr.sa_data[5]);

                        return true;
                    }
                }
                i++;
            }
        }
    }
    return false;
}

int uv::Packet::readFromBuffer(PacketBuffer* packetbuf, Packet& out)
{
    std::string data("");
    while (true)
    {
        auto size = packetbuf->readSize();
        //数据小于包头大小
        if (size < PacketMinSize())
        {
            return -1;
        }
		
        //找包头
        uint16_t dataSize;
        packetbuf->readBufferN(data, sizeof(dataSize)+1);
        if ((uint8_t)data[0] != HeadByte) //包头不正确，从下一个字节开始继续找
        {
            data.clear();
            packetbuf->clearBufferN(1);
            continue;
        }
        UnpackNum((uint8_t*)data.c_str() + 1, dataSize);
        uint16_t msgsize = dataSize + PacketMinSize();	
        //包不完整
        if (size < msgsize)
        {
            return -1;
        }

        packetbuf->clearBufferN(sizeof(dataSize)+1);
        packetbuf->readBufferN(data, dataSize +1);

        //检查包尾
        if ((uint8_t)data.back() == EndByte)
        {
            packetbuf->clearBufferN(dataSize +1);
            break;
        }
    }
    out.swap(data);
    return 0;
}

void uv::Packet::pack(const char* data, uint16_t size)
{
    dataSize_ = size;
    buffer_.resize(size+ PacketMinSize());

    buffer_[0] = HeadByte;
    PackNum(&buffer_[1],size);

    std::copy(data, data + size, &buffer_[sizeof(HeadByte) + sizeof(dataSize_)]);
    buffer_.back() = EndByte;
}

const char* uv::Packet::getData()
{
    return buffer_.c_str()+sizeof(HeadByte)+sizeof(dataSize_);
}

const uint16_t uv::Packet::DataSize()
{
    return dataSize_;
}

const std::string& uv::Packet::Buffer()
{
    return buffer_;
}

const uint32_t uv::Packet::PacketSize()
{
    return (uint32_t)buffer_.size();
}

void uv::Packet::swap(std::string& str)
{
    buffer_.swap(str);
    dataSize_ = (uint16_t)(buffer_.size() - PacketMinSize());
}


uint32_t uv::Packet::PacketMinSize()
{
    return 4;
}
