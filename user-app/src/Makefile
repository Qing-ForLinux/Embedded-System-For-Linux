#####################################################################  
## file        : test makefile for build current dir .c            ##  
## author      : jernymy                                           ##  
## date-time   : 2/9/2016                                      ##  
#####################################################################  
  
CC      = arm-linux-gcc  
CPP     = arm-linux-g++  
RM      = rm -rf  

## debug flag  
DBG_ENABLE   = 1  
  
## source file path  
#SRC_PATH   :=  /home/TQ2440/S3C2440/TQ2440_APP/my-object 
  
## target exec file name  
#TARGET     := main_app  
  
## get all source files  
SRCS         += $(wildcard *.c)
  
## all .o based on all .c  
OBJS        := $(SRCS:.c=.o)  
  
## need libs, add at here  
#LIBS :=  
  
## used headers  file path  
INCLUDE_PATH := ../include 
  
## used include librarys file path  
#LIBRARY_PATH := /lib  
  
## debug for debug info, when use gdb to debug  
ifeq (1, ${DBG_ENABLE})   
	CFLAGS += -D_DEBUG -O0 -g -DDEBUG=1  
endif  
  
## get all include path  
CFLAGS  += $(foreach dir, $(INCLUDE_PATH), -I$(dir))  
  
## get all library path  
#LDFLAGS += $(foreach lib, $(LIBRARY_PATH), -L$(lib))  
  
## get all librarys  
LDFLAGS += $(foreach lib, $(LIBS), -l$(lib))  
  
all:build 
 
#只编译成.o文件
build:
	$(RM) $(OBJS)
	$(CC) -c $(SRCS) $(CFLAGS)   
  
clean:  
	$(RM) $(OBJS) $(TARGET) 
	