/*************************************************************************
	> File Name: Utilities.cpp
	> Author: miaopei
	> Mail: miaopei@baicells.com 
	> Created Time: 2020年02月12日 星期三 11时16分50秒
 ************************************************************************/

#include "Utilities.h"

int getSubnetMask()
{
    std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
    struct sockaddr_in *sin = NULL;
    struct ifaddrs *ifa = NULL, *ifList;

    if (getifaddrs(&ifList) < 0)
    {
        return -1;
    }

    for (ifa = ifList; ifa != NULL; ifa = ifa->ifa_next)
    {
        if(ifa->ifa_addr->sa_family == AF_INET)
        {
            //printf("n>>> interfaceName: %sn", ifa->ifa_name);
            std::cout << "interfaceName: " << ifa->ifa_name << std::endl;

            sin = (struct sockaddr_in *)ifa->ifa_addr;
            printf(">>> ipAddress: %sn", inet_ntoa(sin->sin_addr));

            sin = (struct sockaddr_in *)ifa->ifa_dstaddr;
            printf(">>> broadcast: %sn", inet_ntoa(sin->sin_addr));

            sin = (struct sockaddr_in *)ifa->ifa_netmask;
            printf(">>> subnetMask: %sn", inet_ntoa(sin->sin_addr));
        }
    }
    freeifaddrs(ifList);

    return 0;
}


void DispNetInfo(const char* szDevName)
{
    int s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0)
    {
        fprintf(stderr, "Create socket failed!errno=%d", errno);
        return;
    }

    struct ifreq ifr;
    unsigned char mac[6];
    unsigned long nIP, nNetmask, nBroadIP;

    printf("%s:\n", szDevName);

    strcpy(ifr.ifr_name, szDevName);
    if (ioctl(s, SIOCGIFHWADDR, &ifr) < 0)
    {
        return;
    }
    memcpy(mac, ifr.ifr_hwaddr.sa_data, sizeof(mac));
    printf("\tMAC: %02x-%02x-%02x-%02x-%02x-%02x\n",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    strcpy(ifr.ifr_name, szDevName);
    if (ioctl(s, SIOCGIFADDR, &ifr) < 0)
    {
        nIP = 0;
    }
    else
    {
        nIP = *(unsigned long*)&ifr.ifr_broadaddr.sa_data[2];
    }
    printf("\tIP: %s\n", inet_ntoa(*(in_addr*)&nIP));

    strcpy(ifr.ifr_name, szDevName);
    if (ioctl(s, SIOCGIFBRDADDR, &ifr) < 0)
    {
        nBroadIP = 0;
    }
    else
    {
        nBroadIP = *(unsigned long*)&ifr.ifr_broadaddr.sa_data[2];
    }
    printf("\tBroadIP: %s\n", inet_ntoa(*(in_addr*)&nBroadIP));

    strcpy(ifr.ifr_name, szDevName);
    if (ioctl(s, SIOCGIFNETMASK, &ifr) < 0)
    {
        nNetmask = 0;
    }
    else
    {
        nNetmask = *(unsigned long*)&ifr.ifr_netmask.sa_data[2];
    }
    printf("\tNetmask: %s\n", inet_ntoa(*(in_addr*)&nNetmask));
    close(s);
}

