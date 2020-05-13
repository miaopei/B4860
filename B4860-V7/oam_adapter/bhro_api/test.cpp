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

    adapter.GetTopo();
    while(1){sleep(1);}
	return 0;
}

