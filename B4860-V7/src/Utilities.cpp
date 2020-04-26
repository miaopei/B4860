/*************************************************************************
	> File Name: Utilities.cpp
	> Author: miaopei
	> Mail: miaopei@baicells.com 
	> Created Time: 2020年02月12日 星期三 11时16分50秒
 ************************************************************************/

#include "Utilities.h"

bool GetDeviceIP(const char* interface_name, char* ip, size_t size)
{
    int s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0)
    {
        fprintf(stderr, "Create socket failed!errno=%d", errno);
        return false;
    }

    unsigned long nIP;
    struct ifreq ifr;

    strcpy(ifr.ifr_name, interface_name);
    if(ioctl(s, SIOCGIFADDR, &ifr) < 0)
    {
        return false;
    }

    nIP = *(unsigned long*)&ifr.ifr_broadaddr.sa_data[2];
    snprintf(ip, size, "%s", inet_ntoa(*(in_addr*)&nIP));

    close(s);
    return true;
}

bool GetDeviceMAC(const char* interface_name, char* mac, size_t size)
{
    int s = socket(AF_INET, SOCK_DGRAM, 0);
    if(s < 0)
    {
        fprintf(stderr, "Create socket failed!errno=%d", errno);
        return false;
    }

    struct ifreq ifr;
    unsigned char nmac[6];

    strcpy(ifr.ifr_name, interface_name);
    if(ioctl(s, SIOCGIFHWADDR, &ifr) < 0)
    {
        return false;
    }

    memcpy(nmac, ifr.ifr_hwaddr.sa_data, sizeof(nmac));
    snprintf(mac, size, "%02X%02X%02X%02X%02X%02X",
                        nmac[0], nmac[1], nmac[2], nmac[3], nmac[4], nmac[5]);

    close(s);
    return true;
}

bool GetDeviceGateWay(const char* interface_name, char* gateway, size_t size)  
{  
    FILE *fp;  
    char buf[512] = { 0 };  
    uint32_t defaultRoutePara[4] = { 0 };

    fp = fopen(PATH_ROUTE, "r");  
    if(NULL == fp)  
    {  
        perror("popen error");  
        return false;  
    }

    while(fgets(buf, sizeof(buf), fp) != NULL)  
    {
        sscanf(buf, "%*s %x %x %x %*x %*x %*x %x %*x %*x %*x\n", 
                    (uint32_t *)&defaultRoutePara[1], 
                    (uint32_t *)&defaultRoutePara[0], 
                    (uint32_t *)&defaultRoutePara[3], 
                    (uint32_t *)&defaultRoutePara[2]);

        if(NULL != strstr(buf, interface_name))
        {
            //如果FLAG标志中有 RTF_GATEWAY
            if(defaultRoutePara[3] & RTF_GATEWAY)
            {
                uint32_t ip = defaultRoutePara[0];
                snprintf(gateway, size, "%d.%d.%d.%d", 
                                    (ip & 0xff), 
                                    (ip >> 8) & 0xff, 
                                    (ip >> 16) & 0xff, 
                                    (ip >> 24) & 0xff);
                break;
            }
        }
        memset(buf, 0, sizeof(buf));
    }  
    fclose(fp);  
    fp = NULL;
    return true;
}  

std::vector<std::string> DataSplit(const std::string& in, const std::string& delim)
{
    vector<string> res;
    if("" == in) return res;

    char *strs = new char[in.length() + 1];
    strcpy(strs, in.c_str());

    char *d = new char[delim.length() + 1];
    strcpy(d, delim.c_str());

    char *p = strtok(strs, d);
    while(p) {
        string s = p;
        res.push_back(s);
        p = strtok(NULL, d);
    }

    return res;
}

void LogPrint(LogLevel level, const char* pFile, int iLine, const char* fmt, ...)
{
    time_t t = time(0); 
    char timeStr[32] = {0};
    va_list ap;
    char log_buf[LOG_BUF_SIZE];
    int log_head = 0;

    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", localtime(&t)); 

    log_head = snprintf(log_buf, LOG_BUF_SIZE, "[%s][%s:%d] > ", 
                        timeStr, pFile, iLine);

    va_start(ap, fmt);
    vsnprintf(&log_buf[log_head], LOG_BUF_SIZE - log_head, fmt, ap);
    va_end(ap);

    switch(level){
    case LogLevel::debug:
        uv::LogWriter::Instance()->debug(log_buf);
        break;
    case LogLevel::info:
        uv::LogWriter::Instance()->info(log_buf);
        break;
    case LogLevel::error:
        uv::LogWriter::Instance()->error(log_buf);
        break;
    default:
        uv::LogWriter::Instance()->error("Error: log level error");
    }
}




