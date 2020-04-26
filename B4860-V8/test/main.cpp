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

#include <time.h>
#include <stdarg.h>

#define _DEBUG

#define LOG_BUF_SIZE (4096)
extern void LogDebug(const char* pFile, int iLine, const char* fmt, ...)
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

    std::cout << "log_buf: " << log_buf << std::endl;
}

#ifdef _DEBUG
#define TRACE_PRINTF(format, ...) \
    printf("[%s:%s]%s:%d > " format "\n", __DATE__, __TIME__, __FUNCTION__, __LINE__, ##__VA_ARGS__);

#define LOG_DEBUG(fmt...) \
    LogDebug(__FUNCTION__, __LINE__, fmt); 
#endif

using namespace std;

void TestLog_123456789()
{
    LOG_DEBUG("test log");
}

int main()
{
    TestLog_123456789();
    return 0;
}

