#ifndef  UV_PACKET_H
#define  UV_PACKET_H

#include <string>
#include <sstream>
#include <iomanip>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

#include <stdio.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include "PacketBuffer.h"
#include "Utilities.h"

using namespace std;

#define HEADLENGTH 		26

#define MAXINTERFACES 	11


//PacketIR:
//-----------------------------------------------------------------------------------------------------------------
//  head  | source | destination |   mac   | state  | msgID  |   HOP  |  port  | uPort  | length |  data  |  end   |
// 1 Byte | 1 Byte |    1 Byte   | 12 Byte | 1 Byte | 4 Byte | 1 Byte | 1 Byte | 1 Byte | 4 Byte | N Byte | 1 Byte |
//-----------------------------------------------------------------------------------------------------------------
// head: 数据包头校验 (0x7e)
// source：HUB、RRU、BBU、OAM (0,1,2,3)
// destination: 消息目标发给谁 HUB、RRU、BBU、OAM 
// mac: 设备连接的网口 mac 地址
// state: 请求、响应 (0,1)
// msgID：消息编号 (1001)
// RRUID: rruid为4 (4)   
// port: 设备本端端口号 (0,1)
// uPort: 设备连接对端端口号 (0-7)
// length: 0021 (data 数据长度)
// data: 用户数据
// end: 数据包结尾校验 (0xe7)
//
// eg: (head 14 Byte)
//      HUB MSG_CONNECT REQUEST TO_BBU RRUID_2 PORT_1 UPORT_4 length data
//      0   1040        0       2      2       1      4       0023   key0=value0&key1=value1
//      01040022140023key0=value0&key1=value1

namespace uv
{

class Packet
{
public:
    struct Head{
        std::string s_source;
        std::string s_destination;
		std::string s_mac;
        std::string s_state;
        std::string s_msgID;
        std::string s_hop;
        std::string s_port;
        std::string s_uport;
    };

    enum Source
    {
        HUB     = 0,
        RRU     = 1,
        BBU     = 2,
        OAM     = 3
    };

    enum Destination
    {
        TO_HUB     = 0,
        TO_RRU     = 1,
        TO_BBU     = 2,
        TO_OAM     = 3
    };

    enum State
    {
        REQUEST     = 0,
        RESPONSE    = 1 
    };

    enum MsgID
    {
        MSG_BEGIN   				= 1000,
        MSG_GET_KEY     			= 1001,
        MSG_GET_MSGID               = 1002,
        MSG_SET     				= 1003,
        MSG_UPGRADE 				= 1004,
        MSG_GET_LOG					= 1005,
        MSG_GET_NETWORK_TOPOLOGY	= 1006,
        MSG_RFTxStatus_SET          = 1007,
        MSG_CONNECT					= 1040,
        MSG_DELAY_MEASUREMENT 		= 1041,
        MSG_DELAY_COMPENSATION      = 1042,
        MSG_UPDATE_DELAY            = 1043,
        MSG_END                     = 9999
    };

    Packet();
    ~Packet();

    static int readFromBuffer(PacketBuffer*, Packet&);

    void pack(const char* data, uint16_t size);
    
    void SetHead(Head& head);

    std::string num2str(int num);
    void PackMessage(std::string& data, size_t size);

    void UnPackMessage();

    std::string GetPacket();
    std::string GetHead();
    std::string GetSource();
	std::string GetDestination();
	std::string GetMac();
	std::string GetState();
    std::string GetMsgID();
	std::string GetHop();
	std::string GetPort();
	std::string GetUPort();
    int GetLength();
    std::string GetData();

	void EchoPackMessage();
	void EchoUnPackMessage();

	std::vector<std::string> DataSplit(const std::string& in, const std::string& delim);
	void SplitData2Map(std::map<std::string, std::string>& map);

	bool GetDeviceMac(const char* inet, char* mac);

	//int Source2Destination();
	//int Destination2Source();
	
    const char* getData();
    const uint16_t DataSize();
    const std::string& Buffer();
    const uint32_t PacketSize();

    void swap(std::string& str);

    static std::string dec2hex(int dec);

    template<typename NumType>
    static void UnpackNum(const uint8_t* data, NumType& num);

    template<typename NumType>
    static void PackNum(char* data, NumType num);

    static uint32_t PacketMinSize();

public:
    enum DataMode
    {
        BigEndian,
        LittleEndian
    };

    static uint8_t HeadByte;
    static uint8_t EndByte;
    static DataMode Mode;

private:
    std::string m_source;
	std::string m_destination;
	std::string m_mac;
	std::string m_state;
    std::string m_msgID;
	std::string m_hop;
	std::string m_port;
	std::string m_uport;
    int m_length;
    std::string m_data;
    std::string m_packet;

    std::string buffer_;
    uint16_t dataSize_;
};

inline std::string Packet::dec2hex(int dec) //将int转成16进制字符串
{
    stringstream ioss; //定义字符串流
    string s_temp; //存放转化后字符
    //ioss << setiosflags(ios::uppercase) << hex << i; //以十六制(大写)形式输出
    ioss << resetiosflags(ios::uppercase) << hex << dec; //以十六制(小写)形式输出//取消大写的设置
    ioss >> s_temp;
    return std::string("0x" + s_temp);
}

template<typename NumType>
inline void Packet::UnpackNum(const uint8_t* data, NumType& num)
{
    num = 0;
    auto size = static_cast<int>(sizeof(NumType));
    if (Packet::DataMode::BigEndian == Packet::Mode)
    {
        for (int i = 0; i < size; i++)
        {
            num <<= 8;
            num |= data[i];

        }
    }
    else
    {
        for (int i = size-1; i >= 0; i--)
        {
            num <<= 8;
            num |= data[i];
        }
    }
}

template<typename NumType>
inline void Packet::PackNum(char* data, NumType num)
{
    int size = static_cast<int>(sizeof(NumType));
    if (Packet::DataMode::BigEndian == Packet::Mode)
    {
        for (int i = size-1; i >= 0; i--)
        {
            data[i] = num & 0xff;
            num >>= 8;
        }
    }
    else
    {
        for (int i = 0; i < size; i++)
        {
            data[i] = num & 0xff;
            num >>= 8;
        }
    }
}
}
#endif
