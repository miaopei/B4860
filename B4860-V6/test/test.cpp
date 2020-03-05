#include <iostream>
#include <stdint.h>
#include <string>
#include <sstream>

#include <cstdio>
#include <stdio.h>
#include <cstring>

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
    uint8_t a = 3;
    uint16_t c = 1001;
    uint16_t d = 20;

    char str[20];
    //dec2char(c, str, sizeof(uint16_t));
    //
    char data[] = {'m', 'i', 'a', 'o', 'w'};
    memset(str, 0, sizeof(str));

    memcpy(&str[0], &a, sizeof(uint8_t));
    memcpy(&str[1], &c, sizeof(uint16_t));
    memcpy(&str[3], &d, sizeof(uint16_t));
    memcpy(&str[5], &data, sizeof(data));

    std::cout << "str=" << str << std::endl;


    return 0;
}

