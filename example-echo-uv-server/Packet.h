
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


using namespace std;

#define HEADLENGTH 		26

#define MAXINTERFACES 	11


//PacketIR:
//-----------------------------------------------------------------------------------------------------------------
//  head  | source | destination |   mac   | state  | msgID  | HOP  |  port  | uPort  | length |  data  |  end   |
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

typedef struct BHRO_S_MSG_HEADER {
    uint8_t source;
    uint8_t destination;
    uint8_t mac[6];
    uint8_t state;
    uint16_t msgID;
    uint8_t hop;
    uint8_t port;
    uint8_t uport;
} BHRO_T_MSG_HEADER;

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

}
#endif
