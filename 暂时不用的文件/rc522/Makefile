#####################################################################  
## file        : test makefile for build current dir .c            ##  
## author      : jernymy                                           ##  
## date-time   : 2/9/2016                                      ##  
#####################################################################  
  
#添加需要编译的文件

obj-m +=rc522_drv.o
rc522_drv-objs :=rc522_driver.o SPI.o MFRC522.o

KDIR = /home/TQ2440/S3C2440/linux/opt/EmbedSky/linux-2.6.30.4

all:
	make -C $(KDIR) M=$(PWD) modules CROSS_COMPILE=arm-linux- ARCH=arm  
	make clean
	cp *.ko /home/NFS


clean:
	rm -rf *.mod* *.order *.symvers *.o .dht11* .tmp* .*.cmd
	
display:
	#echo $(obj-m)

