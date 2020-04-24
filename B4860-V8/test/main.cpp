#include <iostream>

#include <map>
#include <vector>
#include <set>
#include <cstring>
#include <regex>

#include <stdio.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <string.h>

using namespace std;

void log_echo(std::string data)
{
    std::cout << "Log: " << data << std::endl;
}

int main()
{
    log_echo(std::string(__FUNCTION__ + ":" + __LINE__ + " > " +"This is a test"));
    return 0;
}

