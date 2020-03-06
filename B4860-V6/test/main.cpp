#include <iostream>
#include <stdint.h>
#include <string>
#include <sstream>

#include <cstdio>
#include <stdio.h>
#include <cstring>
#include "test.h"
#include "Head.h"

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
    test::Data testData = {"Mr.Miaow", 30};
    
    //test << Data;

    std::ostringstream stream;
    stream << testData;
    const std::string serializedData = stream.str();
    std::cout << "serializedData=" << serializedData << std::endl;


    std::string dx = "11001x";
    std::cout << "type=" << dx.substr(0, 1) << std::endl;
    std::cout << "msgID=" << dx.substr(1, 4) << std::endl;

    std::string data = "key=value&key2=value2";
    uv::Head head("1", "1666");
    //head.SetHead("1", "1888");
    head.PackMessage(data, data.length());
    std::cout << "GetPacket=" << head.GetPacket() << std::endl;
    std::cout << "GetHead=" << head.GetHead() << std::endl;
    std::cout << "GetType=" << head.GetType() << std::endl;
    std::cout << "GetMsgID=" << head.GetMsgID() << std::endl;
    std::cout << "GetData=" << head.GetData() << std::endl;

    return 0;
}

