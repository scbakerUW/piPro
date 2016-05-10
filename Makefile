CC = gcc

# The recommended compiler flags for the Raspberry Pi
#CCFLAGS=-Ofast -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s
CCFLAGS= -Wall -g -O3 -fPIC -lm

# define all programs
TARGET = piPro

SRCS += \
	src/bcm2835.c \
	src/ssd1306.c \
	src/gps_uart.c \
	src/ui.c \
	src/pi_pro_main.c

OBJS += \
	$(SRCS:.c=.o)

all: ${TARGET}

clean:
	rm -rf $(TARGET) *.o

%.o:%.c
	gcc -std=gnu99 $(CCFLAGS) -c $< -o $@

${TARGET}: ${OBJS}
	gcc -o $@ $(OBJS) -pthread 

