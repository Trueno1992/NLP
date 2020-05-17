TARGET     := ./bin/Main
CC         := gcc
CXX        := g++
CFLAGS     := -lpthread -g -Wall

INCLUDE    := ./utils
LOCAL_DIR  := ./utils .
LIB        := ./lib
LOCAL_SRC  := $(foreach dir, $(LOCAL_DIR), $(wildcard $(dir)/*.cpp $(dir)/*.c $(dir)/*.cc))
LOCAL_INC  := $(foreach dir, $(INCLUDE), $(wildcard $(dir)/*.h))

LOCAL_OBJS = $(patsubst %.cpp,%.o,$(LOCAL_SRC))

$(LOCAL_OBJS):%.o:%.cpp
	@echo $?

$(TARGET): $(LOCAL_OBJS)
	$(CXX) $(CFLAGS) $(LOCAL_OBJS) -o $@


help:
	@echo "local_src =" $(LOCAL_SRC)
	@echo "local_objs=" $(LOCAL_OBJS)
