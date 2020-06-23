/*************************************************************************
	> File Name: test.cpp
	> Author: miaopei
	> Mail: miaopei@baicells.com 
	> Created Time: 2020年02月12日 星期三 11时16分50秒
 ************************************************************************/

#include <iostream>

#include "BHRO_API.h"

using namespace uv;
using namespace std;

int main(int argc, char** argv)
{
    BHRO_API adapter;
    adapter.BHRO_INIT();

    //adapter.GetTopo();
    //adapter.DeviceUpGrade("0", "rHUP_1.tar", "123456789");
    //adapter.DeviceUpGrade("0", "0_2", "rHUP_2.tar", "123456789");
#if 1
    int i = 20;
    while(i != 0){
        sleep(1);
        i--;
    }
    //adapter.DeviceUpGrade("0", "rHUP_1.tar", "123456789");
    //adapter.DeviceDataSet("0", "power=123&Rx=332");
    //adapter.DeviceDataSet("0", "0_2", "power=123&Rx=332");
    adapter.DeviceDataSet("0", "Reboot=0&Reboot=1&Test=1");
#endif
    while(1){sleep(1);}

	return 0;
}

