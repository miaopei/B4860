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

#define MAXINTERFACES 16
#define SORTMAP 1

using namespace std;

bool getmac(const char* inet, char* mac)
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
                        sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X",
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

#if SORTMAP
#include <map>
#include <vector>
#include <string>
#include <algorithm>

typedef pair<std::string, double> PAIR;

std::map<std::string, double> maxDelay;

double cmp(const PAIR& x, const PAIR& y)
{
    return x.second > y.second;
}

void sortMapByValue(std::map<std::string, double>& map, vector<PAIR>& tVector)
{
    for(auto &it : map)
        tVector.push_back(make_pair(it.first, it.second));

    sort(tVector.begin(), tVector.end(), cmp);
}
#endif

int main()
{
    char mac[MAXINTERFACES] = {0};
    char inet[] = "enp1s0";
    if(!getmac(inet, mac))
    {
        std::cout << "Error: GetMac error" << std::endl;
        //return 0;
    }
    std::cout << "Mac=" << mac << std::endl;

#if SORTMAP
    maxDelay.insert(make_pair("R_0_1", 11.032));
    maxDelay.insert(make_pair("R_0_2", 21.032));
    maxDelay.insert(make_pair("R_0_3", 41.032));
    maxDelay.insert(make_pair("R_0_4", 71.032));
    maxDelay.insert(make_pair("R_0_5", 31.032));

    vector<PAIR> tVector;
    sortMapByValue(maxDelay, tVector);

    for(int i = 0; i < static_cast<int>(tVector.size()); i++)
    {
        std::cout << tVector[i].first << ":" << tVector[i].second << std::endl;
    }

    std::cout << "Max Delay: " << tVector.begin()->second << std::endl;
    std::cout << "Min Delay: " << tVector.back().second << std::endl;

    vector<PAIR>::iterator itor;
    for(itor = tVector.begin(); itor != tVector.end(); itor++)
    {
        if(itor->first == "R_0_4")
        {
           tVector.erase(itor);
        }
    }

    for(int i = 0; i < static_cast<int>(tVector.size()); i++)
    {
        std::cout << tVector[i].first << ":" << tVector[i].second << std::endl;
    }

#endif

    std::string RouteIndex = "";
    if(RouteIndex.empty())
    {
        std::cout << "string is empty" << std::endl;
    } else {
        std::cout << "RouteIndex=" << RouteIndex << std::endl;
    }

    std::string str("   字符串   String    ");
    std::cout << str << std::endl;
    std::cout << str.size() << std::endl;
    str.erase(str.find_first_of(' '), str.find_first_not_of(' '));
    str.erase(str.find_last_not_of(' '));
    std::cout << str << std::endl;
    std::cout << str.size() << std::endl;

    return 0;
}

