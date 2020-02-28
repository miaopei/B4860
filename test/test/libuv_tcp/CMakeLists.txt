CMAKE_MINIMUM_REQUIRED(VERSION 3.5.1)
PROJECT(uvcpp-server)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -Wextra -pedantic -pthread -std=c++11 -Wl,--no-as-needed")
SET(CMAKE_BUILD_TYPE Debug)

INCLUDE_DIRECTORIES(
    .
    ${PROJECT_SOURCE_DIR}
)

# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_SRCS 变量
# aux_source_directory(. DIR_SRCS)
AUX_SOURCE_DIRECTORY(. DIR_SRCS)

# 指定生成目标
ADD_EXECUTABLE(${PROJECT_NAME} ${DIR_SRCS})

# ADD_EXECUTABLE(${PROJECT_NAME} main.cpp Server.cpp TcpConnection.cpp)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} uv)
