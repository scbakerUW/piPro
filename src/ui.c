#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>
#include <string.h>

#define PIN_IN RPI_GPIO_P1_16
#define BUF_LEN 5
void *readGPIO(void *arg)
{
	struct pollfd pfd;
	int fd, gpio;
   	char buf[BUF_LEN];
	int oldValue, newValue;

   	gpio = 23;
	
	if ((fd = open("/sys/class/gpio/gpio23/value", O_RDONLY)) < 0)
   	{
      		fprintf(stderr, "Failed, gpio %d not exported.\n", gpio);
      		exit(1);
   	}

   	pfd.fd = fd;

   	pfd.events = POLLPRI;

	read(fd, buf, BUF_LEN-1);

	oldValue = atoi(buf);
	printf("value is %d\n",oldValue);
	while(1)
	{
		memset(buf, 0x00, BUF_LEN);
		lseek(fd, 0, SEEK_SET);
		poll(&pfd, 1, -1);
		read(fd, buf, BUF_LEN-1);
		newValue = atoi(buf);
//		printf("newValue is %d\n",newValue);
		if(oldValue != newValue && oldValue < newValue)
		{ 	
			printf("rising edge on pin 16 %s\n",buf);
			usleep(300);
		}
		oldValue = newValue;
	}

	return NULL;
}

int start_GPIO_UI()
{
	pthread_t pth;

	pthread_create(&pth,NULL,readGPIO,NULL);

	return 1;
}
