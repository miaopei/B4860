PWD=`pwd`

WORK_ROOT = $(PWD)

BUILD_DIR = $(WORK_ROOT)/../build
INCLUDE_DIR = $(BUILD_DIR)/include
LIB_DIR = $(BUILD_DIR)/lib

TARGET = libbbu

CC = g++
SOLINK.cc = $(CC) -shared -o

CFLAGS = -Wall -std=c++11 -fPIC
#USER_LINK_LIB = -lpthread -L$(LIB_DIR) -luv -Wl,-rpath,$(LIB_DIR)
USER_LINK_LIB = -lpthread -L$(LIB_DIR) -luv
USER_LINK_INC = -I$(INCLUDE_DIR) -I./include
CC_ARG = $(CFLAGS) $(USER_LINK_INC) $(USER_LINK_LIB)
LINK_ARG = $(CFLAGS) $(USER_LINK_INC) $(USER_LINK_LIB)

SOURCE = $(wildcard *.cpp)
OBJECTS = $(SOURCE:.cpp=.o)

all: $(TARGET).so
	@echo "[build libbbu ...]"
	@mv $(TARGET).so $(LIB_DIR)
	@cp -rf include/* $(INCLUDE_DIR)

$(TARGET).so: $(OBJECTS)
	$(SOLINK.cc) $(TARGET).so $(OBJECTS) $(LINK_ARG)
	chmod a+rx $(TARGET).so

%.o: %.cpp
	$(CC) -c $(CFLAGS) $(USER_LINK_INC) $< -o $@ $(USER_LINK_LIB)

.PHONY:clean
clean:
	rm -rf *.o

