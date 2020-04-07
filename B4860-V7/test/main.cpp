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

typedef pair<std::string, double> PAIR;

std::map<std::string, double> maxDelay;

void MaxDelaySort(const PAIR& p)
{
    
}

int main()
{
    char mac[32] = {0};
    char inet[] = "enp0s31f6";
    if(!getmac(inet, mac))
    {
        std::cout << "Error: GetMac error" << std::endl;
        return 0;
    }
    std::cout << "Mac=" << mac << std::endl;

    return 0;
}

