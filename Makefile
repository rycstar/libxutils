CC=$(CROSS_COMPILE)gcc
CUR_DIR=$(shell pwd)
CFLAGS:=-Wall -O3
SRC_PATH:=$(CUR_DIR)/src

SRCS_C:=$(foreach dir, $(SRC_PATH), $(wildcard $(dir)/*.c))
OBJS_C:=$(patsubst %.c, %.o, $(SRCS_C))

OUTPUT_LIB=libxutils.so

INC_DIR=-I$(CUR_DIR)/include

all:$(OUTPUT_LIB)

$(OBJS_C):%.o : %.c
	$(CC) $(CFLAGS) -fpic -c $< -o $@ $(INC_DIR)

$(OUTPUT_LIB) : $(OBJS_C)
	$(CC) -shared -o $@ $(OBJS_C)

clean:
	rm $(OBJS_C) $(OUTPUT_LIB)

