PWD=`pwd`

WORK_ROOT = $(PWD)

BUILD_DIR = $(WORK_ROOT)/../build
INCLUDE_DIR = $(BUILD_DIR)/include
LIB_DIR = $(BUILD_DIR)/lib
BIN_DIR = $(BUILD_DIR)/bin

TARGET = test

CC = g++
CFLAGS = -Wall -std=c++11 
USER_LINK_INC = -I$(INCLUDE_DIR) -I./include
USER_LINK_LIB = -L$(LIB_DIR) -ljson -Wl,-rpath,$(LIB_DIR)
#USER_LINK_LIB = -L$(LIB_DIR) -pthread -luv -lbbu -Wl,-rpath,$(LIB_DIR)
#USER_LINK_LIB = -pthread -L$(LIB_DIR) -luv -L$(LIB_DIR) -lbbu 

SOURCE = $(wildcard *.cpp)
OBJECTS = $(SOURCE:.cpp=.o)

all: before $(TARGET)

before:
	@cp -d ./libs/* $(LIB_DIR)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(USER_LINK_INC) $^ -o $@ $(USER_LINK_LIB) 

%.o: %.cpp
	$(CC) -c $(CFLAGS) $(USER_LINK_INC) $< -o $@ $(USER_LINK_LIB) 

.PHONY:clean
clean:
	@rm -f $(OBJECTS)
	@rm -f $(TARGET)

