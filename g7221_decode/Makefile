#!bash

include function
DIRS := $(shell find . -maxdepth 2 -type d)
INC	= -I./common/
SRC = $(foreach dir,$(DIRS),$(wildcard $(dir)/*.c))
CPPSRC += $(foreach dir,$(DIRS),$(wildcard $(dir)/*.cpp))


CROSS = arm-hisiv300-linux-
CC 		= $(CROSS)gcc
CPP		= $(CROSS)g++
LD		= $(CROSS)ld
AR		= $(CROSS)ar
RM		= rm -f

COBJS		= $(SRC:%.c=%.o)
CPPOBJS		= $(CPPSRC:%.cpp=%.o)
CFLAGS	+= $(INC)


AFLAGS	+= -rv
LDFLAGS	+= -lpthread  #-lstdc++

EXEC	= test
LIBBLURE= libg7221.a
TARGET  = $(LIBBLURE)
all: $(TARGET)

$(LIBBLURE): $(COBJS) $(CPPOBJS)
	@$(RM) $@;
	@$(AR) $(AFLAGS) $@ $^
$(EXEC): $(COBJS) $(CPPOBJS) $(LIBS)
	@$(CC) -o $@ $^ $(LDFLAGS)
.c.o:
	@$(CC) -c $(CFLAGS)   $< -o $@
.cpp.o:
	@$(CPP) -c $(CFLAGS)   $< -o $@


clean:
	@rm -f $(TARGET) $(COBJS) $(CPPOBJS);
	@rm -rf $(OBJSTMP);
