#include <iostream>

#define PORT        30000
#define IFRNAME     "enp0s31f6"

#define _DEBUG
#include "Utilities.h"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#if 0
TEST_CASE("test utilities interfade function") {
    //const char *interface_name = "enp0s31f6";
    char* pdata = NULL;
    size_t size = 32;
    pdata = (char*)malloc(size * sizeof(char));
    if(pdata == NULL)
    {
        std::cout << "Error: malloc gateway memory error" << std::endl;
    }

    GetDeviceIP(IFRNAME, pdata, size);
    std::cout << "Device IP: " << pdata << std::endl;
    memset(pdata , 0, size);

    GetDeviceMAC(IFRNAME, pdata, size);
    std::cout << "Device MAC: " << pdata << std::endl;
    memset(pdata , 0, size);
    
    GetDeviceGateWay(IFRNAME, pdata, size);
    std::cout << "Device GateWay: " << pdata << std::endl;
    memset(pdata , 0, size);
    
    free(pdata);
    pdata = NULL;
}

TEST_CASE("test utilities log function") {
    LOG_PRINT(LogLevel::debug, "test utilities debug log function");
    LOG_PRINT(LogLevel::info, "test utilities info log function");
    LOG_PRINT(LogLevel::error, "test utilities error log function");
}
#endif
#if 0
TEST_CASE("test BHRO lib") {
    BHRO_API adapter;
    adapter.BHRO_INIT();

    adapter.GetTopo();
    adapter.DeviceUpGrade("0", "rHUP_1.tar", "123456789");
    adapter.DeviceUpGrade("0", "0_1_0_0", "rHUP_2.tar", "123456789");
    //adapter.TestTimer();
    //while(1){sleep(1);}
}

#endif

TEST_CASE("test stoi") {
    std::string str;
    switch(std::stoi(str))
    {
    case 0:
        LOG_PRINT(LogLevel::debug, "case 0");
        break;
    default:
        LOG_PRINT(LogLevel::debug, "default");
        break;
    }
}

