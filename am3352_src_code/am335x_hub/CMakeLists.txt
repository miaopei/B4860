cmake_minimum_required(VERSION 3.5.1)

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm) 
set(CMAKE_C_COMPILER "arm-linux-gnueabihf-gcc")
set(CMAKE_CXX_COMPILER "arm-linux-gnueabihf-g++")

#project(RHUB)

AUX_SOURCE_DIRECTORY(./src DIR_SRCS)
SET(SOURCES ${DIR_SRCS})

INCLUDE_DIRECTORIES(./src)

ADD_LIBRARY (rhub SHARED ${SOURCES})
ADD_LIBRARY (rhub_static STATIC ${SOURCES})

SET_TARGET_PROPERTIES (rhub_static PROPERTIES OUTPUT_NAME "rhub")
GET_TARGET_PROPERTY (OUTPUT_VALUE rhub_static OUTPUT_NAME)
MESSAGE (STATUS "This is the rhub_static OUTPUT_NAME: " ${OUTPUT_VALUE})

SET_TARGET_PROPERTIES (rhub_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)
SET_TARGET_PROPERTIES (rhub PROPERTIES CLEAN_DIRECT_OUTPUT 1)

# 按照规则，动态库是应该包含一个版本号的，
# VERSION指代动态库版本，SOVERSION指代API版本。
SET_TARGET_PROPERTIES (rhub PROPERTIES VERSION 1.0 SOVERSION 1)

INSTALL(TARGETS rhub rhub_static 
        LIBRARY DESTINATION librhub/lib
        ARCHIVE DESTINATION librhub/lib)

#INSTALL(FILES *.h DESTINATION include/hello)

INSTALL(DIRECTORY ./src DESTINATION librhub/include FILES_MATCHING PATTERN "*.h")

# 通过:
# cmake -DCMAKE_INSTALL_PREFIX=./ ..
# make
# sudo make install
# 我们就可以将头文件和共享库安装到 ./build/src/lib和 ./build/src/include/hello中了。

