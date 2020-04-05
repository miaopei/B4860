
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

void uv::Packet::SetHead(std::string sour, 
							   std::string dest, 
							   std::string state, 
							   std::string msgID, 
							   std::string rruid, 
							   std::string port, 
							   std::string uport)
{
	m_source = sour;
	m_destination = dest;
	m_state = state;
	m_msgID = msgID;
	m_rruid = rruid;
	m_port = port;
	m_uport = uport;
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
        std::cout << "The data is too big" << std::endl;
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
	m_state = packet.substr(2, 1);
	m_msgID = packet.substr(3, 4);
	m_rruid = packet.substr(7, 1);
	m_port = packet.substr(8, 1);
	m_uport = packet.substr(9, 1);
    m_length = std::stoi(packet.substr(10, 4));
	m_data = packet.substr(14);
}

std::string uv::Packet::GetPacket() 
{ 
	return m_packet;
}

std::string uv::Packet::GetHead() 
{ 
	return std::string(m_source + m_destination + m_state + m_msgID + m_rruid + m_port + m_uport); 
}

std::string uv::Packet::GetSource() 
{ 
	return m_source; 
}

std::string uv::Packet::GetDestination()
{
	return m_destination;
}

std::string uv::Packet::GetState()
{
	return m_state;
}

std::string uv::Packet::GetMsgID() 
{ 
	return m_msgID; 
}

std::string uv::Packet::GetRRUID()
{
	return m_rruid;
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
	std::cout << "封装 packet:" 
		<< "\n\tPacket: " << m_packet
		<< "\n\tSource: " << m_source << " [HUB,RRU,BBU,OAM]"
		<< "\n\tDestination: " << m_destination << " [HUB,RRU,BBU,OAM]"
		<< "\n\tState: " << m_state << " [REQUEST,RESPONSE]"
		<< "\n\tMsgID: " << m_msgID 
		<< "\n\tRRUID: " << m_rruid
		<< "\n\tPort: " << m_port
		<< "\n\tUPort: " << m_uport
		<< "\n\tLength: " << m_length
		<< "\n\tData: " << m_data
		<< std::endl;
}

void uv::Packet::EchoUnPackMessage()
{
	std::cout << "解析 packet:" 
		<< "\n\tPacket: " << m_packet
		<< "\n\tSource: " << m_source << " [HUB,RRU,BBU,OAM]"
		<< "\n\tDestination: " << m_destination << " [HUB,RRU,BBU,OAM]"
		<< "\n\tState: " << m_state << " [REQUEST,RESPONSE]"
		<< "\n\tMsgID: " << m_msgID 
		<< "\n\tRRUID: " << m_rruid
		<< "\n\tPort: " << m_port
		<< "\n\tUPort: " << m_uport
		<< "\n\tLength: " << m_length
		<< "\n\tData: " << m_data
		<< std::endl;
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
