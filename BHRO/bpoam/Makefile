PWD=`pwd`

WORK_ROOT = $(PWD)

BUILD_DIR = $(WORK_ROOT)/../build
INCLUDE_DIR = $(BUILD_DIR)/include
LIB_DIR = $(BUILD_DIR)/lib

TARGET = libbpoam

#CC = $(CC)
#AR = $(AR)

CFLAGS = -Wall 
USER_LINK_LIB = -L./ -lipc -lmem 
USER_LINK_INC = -I$(INCLUDE_DIR) -I./
CC_ARG = $(CFLAGS) $(USER_LINK_INC) $(USER_LINK_LIB)
LINK_ARG = $(CFLAGS) $(USER_LINK_INC) $(USER_LINK_LIB)

SOURCE = $(wildcard *.c)
OBJECTS = $(SOURCE:.c=.o)

all: $(TARGET).a
	@mv $(TARGET).a $(LIB_DIR)
	@cp libipc.a $(LIB_DIR)
	@cp libmem.a $(LIB_DIR)
	@cp -rf ./*.h $(INCLUDE_DIR)

$(TARGET).a: $(OBJECTS)
	$(AR) cqs $@ $^
	chmod a+rx $(TARGET).a

%.o: %.c
	$(CC) -c $(CFLAGS) $(USER_LINK_INC) $< -o $@ $(USER_LINK_LIB)

.PHONY:clean
clean:
	rm -rf *.o

