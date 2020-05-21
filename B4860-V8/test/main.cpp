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


#define ENUM_TO_STR(ENUM) std::string(#ENUM)

enum DIRECTION{NORTH, SOUTH, WEST, EAST};

int main()
{
    std::cout << "Hello, " << ENUM_TO_STR(0) << "!\n";
    std::cout << "Hello, " << ENUM_TO_STR(SOUTH) << "!\n";

    return 0;
}

