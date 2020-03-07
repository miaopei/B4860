#include <iostream>
#include <stdint.h>
#include <string>
#include <sstream>

#include <cstdio>
#include <stdio.h>
#include <cstring>
#include <map>
#include "test.h"
#include "PacketIR.h"
#include "Connect.h"

using namespace std;


string dec2hex2(int dec, size_t length)
{
    string s_temp; //存放转化后字符
    
    int i;
    for (i = length - 1; i >= 0; i--)
    {
        s_temp[i] = (dec % 256) & 0xFF;
        dec /= 256;

    }
    return s_temp;
}

string toHex(int num) {
    const string HEX = "0123456789abcdef";
    if (num == 0) return "0";
    string result;
    int count = 0;
    while (num && count++ < 8) {
        result = HEX[(num & 0xf)] + result;
        num >>= 4;
    }
    return result;
}

void DectoHex(int dec, unsigned char *output, int length)  
{  
    const char * hex = "0123456789ABCDEF";
    for (int i = 0; i<length; i++)  
    {  
        output[length - i -1] = hex[(dec >> i * 4) & 0x0F]; 
    } 
} 

void dec2char(int dec, char* buf, size_t size)
{
    sprintf(buf, "%d", dec);
}

int main()
{
#if 0
    test::Data testData = {"Mr.Miaow", 30};
    
    //test << Data;

    std::ostringstream stream;
    stream << testData;
    const std::string serializedData = stream.str();
    std::cout << "serializedData=" << serializedData << std::endl;


    std::string dx = "11001x";
    std::cout << "type=" << dx.substr(0, 1) << std::endl;
    std::cout << "msgID=" << dx.substr(1, 4) << std::endl;
#endif

// 封装 packet
#if 0
    std::string data = "key=value&key2=value2";
    uv::PacketIR packet;
    //uv::PacketIR packet("1", "1666");
    if(packet.SetHead("1", "1888"))
    {
        packet.PackMessage(data, data.length());
        std::cout << "封装 packet:" << std::endl;
        std::cout << "\tGetPacket: " << packet.GetPacket() << std::endl;
        std::cout << "\tGetHead: " << packet.GetHead() << std::endl;
        std::cout << "\tGetType: " << packet.GetType() << std::endl;
        std::cout << "\tGetMsgID: " << packet.GetMsgID() << std::endl;
        std::cout << "\tGetData: " << packet.GetData() << std::endl;
    }
#endif

// 解析 packet
#if 0
    std::string data = "11666key=value&key2=value2&key3=value3";

    uv::PacketIR packet;
    packet.UnPackMessage(data);

    std::cout << "解析 packet:" << std::endl; 
    std::cout << "\tGetPacket: " << packet.GetPacket() << std::endl;
    std::cout << "\tGetHead: " << packet.GetHead() << std::endl;
    std::cout << "\tGetType: " << packet.GetType() << std::endl;
    std::cout << "\tGetMsgID: " << packet.GetMsgID() << std::endl;
    std::cout << "\tGetData: " << packet.GetData() << std::endl;

#endif

#if 0
    enum test
    {
        TEST_0 = 0,
        TEST_1 = 1,
        TEST_2 = 2
    };

    std::string tostring;
    std::cout << "tostring=" << to_string(TEST_0) << std::endl;
    std::cout << "tostring=" << to_string(TEST_1) << std::endl;
    std::cout << "tostring=" << to_string(TEST_2) << std::endl;
#endif


    std::map<std::string, Connect::ClientInfo> ConnectInfo;

    ConnectInfo.insert(pair<std::string, Connect::ClientInfo>("10", Connect::ClientInfo("1", "1040", "5")));
    ConnectInfo.insert(pair<std::string, Connect::ClientInfo>("11", Connect::ClientInfo("1", "1041", "6")));
    ConnectInfo.insert(pair<std::string, Connect::ClientInfo>("12", Connect::ClientInfo("1", "1042", "7")));

#if 0
    for (auto it = ConnectInfo.begin(); it != ConnectInfo.end(); ++it) {
        std::cout << it->first << " - > " 
            << it->second.s_type << " " 
            << it->second.s_RRUID << " " 
            << it->second.s_port <<std::endl;
    }
#endif

#if 1
    for(auto &it : ConnectInfo)
    {
         std::cout << it.first << " - > " 
            << it.second.s_type << " " 
            << it.second.s_RRUID << " " 
            << it.second.s_port <<std::endl;
    }
#endif

    return 0;
}

