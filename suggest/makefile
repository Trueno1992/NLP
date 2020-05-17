TARGET           := ./bin/Main
CC               := gcc
CXX              := g++
CFLAGS           := -lpthread -g -Wall
LFLAGS           := -L./lib 

# 存放.h   和 .hpp 的所有目录（空格分隔）
INC_DIR          :=./utils .
# 存放.cpp 和 .c   文件的所有目录（空格分隔）
SRC_DIR          :=./utils . 

LOCAL_SRC        := $(foreach dir, $(SRC_DIR), $(wildcard $(dir)/*.cpp $(dir)/*.c $(dir)/*.cc))
LOCAL_INC        := $(addprefix -I, $(INC_DIR))

LOCAL_OBJS        = $(patsubst %.cpp,%.o, $(patsubst %.c,%.cpp,$(patsubst %.cc,%.cpp,$(LOCAL_SRC))))
LOCAL_DEPENDS    := $(LOCAL_SRC:%.cpp=%.d)
LOCAL_DEPENDS    := $(foreach file, $(LOCAL_DEPENDS), $(dir $(file)).$(notdir $(file)))
#LOCAL_DEPENDS    := $(foreach obj, $(LOCAL_OBJS), $(dir $(obj)).$(basename $(notdir $(obj))).d)

$(TARGET): $(LOCAL_OBJS)
	$(CXX) $(CFLAGS) $(LOCAL_OBJS) -o $@ $(LOCAL_INC)

$(LOCAL_OBJS):%.o:%.cpp
	$(CXX) $(CFLAGS) -c $< -o $@ $(LOCAL_INC)

-include $(LOCAL_DEPENDS)
$(LOCAL_DEPENDS):.%.d:%.cpp
	@set -e; rm -f $@;\
	$(CXX) -MM $(CFLAGS) $< > $@.$$$$;\
	sed 's,\($*\)\.o[:]*,\1.o $@:,g' < $@.$$$$ > $@;\
	rm $@.$$$$

clean:
	rm -f $(TARGET); rm -f $(LOCAL_OBJS); rm -f $(LOCAL_DEPENDS)
