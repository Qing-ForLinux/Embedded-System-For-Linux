#####################################################################  
## file        : test makefile for build current dir .c            ##  
## author      : jernymy                                           ##  
## date-time   : 2/9/2016                                      ##  
#####################################################################  
  
CC      = arm-linux-gcc  
CPP     = arm-linux-g++  
RM      = rm -rf  

## kernel path

KERNEL_PATH = /home/TQ2440/S3C2440/linux/opt/EmbedSky/linux-2.6.30.4

## debug flag  
DBG_ENABLE   = 1  
  
## source file path  
SRC_PATH   :=  $(shell pwd)

## source sub path
SUB_PATH = src

## target exec file name  
TARGET     := main_app  
  
## get all source files  
SRCS         += $(wildcard *.c ./$(SUB_PATH)/*.c)
  
## all .o based on all .c  
OBJS        += $(SRCS:.c=.o)
  
## need libs, add at here  
#LIBS :=  
  
## used headers  file path  
INCLUDE_PATH += ./include $(KERNEL_PATH)

  
## used include librarys file path  
#LIBRARY_PATH := /lib  
  
## debug for debug info, when use gdb to debug
  
  
## get all include path  
CFLAGS  += $(foreach dir, $(INCLUDE_PATH), -I$(dir)) 
  
## get all library path  
#LDFLAGS += $(foreach lib, $(LIBRARY_PATH), -L$(lib))  
  
## get all librarys  
LDFLAGS += $(foreach lib, $(LIBS), -l$(lib))  

  	 
build: 
	$(foreach sub,$(SUB_PATH),make -C $(sub))
	$(CC) -c $(SRCS) $(CFLAGS)  
	$(CC) $(CFLAGS) -lpthread -o $(TARGET) $(OBJS) $(LDFLAGS)  
	$(RM) $(OBJS)
	$(RM) -rf *.o
	chmod 777 main_app
  
clean:  
	$(RM) $(OBJS) $(TARGET) 
	$(RM) -rf *.o

display:
	echo $(OBJS)
	
	
	
