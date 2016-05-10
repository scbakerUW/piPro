#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <assert.h>
#include <pthread.h>
#include "gpsInfoStruct.h"

#define RX_PORT "/dev/ttyAMA0"
#define FALSE 0
#define TRUE 1

struct GPS_DATA *gpsInfo;

char** str_split(const char*, char delimiter);
void* uartReader_thread(void*);
int readLine(char*, int);
int init_tty(void);
void initGpsStruct(void);
struct GPS_DATA *getGpsInfo(void);
void start_gps(void);


void *uartReader_thread(void *arg)
{
	char buffer[512];
    char** splitBuf;
//    char recvBuffer[2];
    int count = 0;
//    char strOutput[128];

	float temp = 0.0;
	float tempLong = 0.0;
	int tempInt = 0;
	int tempIntLong = 0;

	char latitude[24];
	char longitude[24];
	char num_sats[2];

	int fd = (int)arg;

	count = 0;
    splitBuf = malloc(60*sizeof(char *));
	while(1)
	{
		count = readLine(buffer, fd);

		if(strstr(buffer, "$GPGGA") != NULL)
		{
			splitBuf = str_split(buffer, ',');
			strcpy(gpsInfo->rawtime,splitBuf[1]);
			strcpy(latitude,splitBuf[2]);
			strcpy(gpsInfo->latDir,splitBuf[3]);
			strcpy(longitude,splitBuf[4]);
			strcpy(gpsInfo->longDir,splitBuf[5]);
			strcpy(gpsInfo->fix,splitBuf[6]);
			strcpy(num_sats,splitBuf[7]);
			strcpy(gpsInfo->speed,splitBuf[8]);

			gpsInfo->hours = strndup(gpsInfo->rawtime,2);
			gpsInfo->minutes = strndup(gpsInfo->rawtime+2,2);
			gpsInfo->seconds = strndup(gpsInfo->rawtime+4,2);

			temp = atof(latitude)/100;
			tempInt = (int)temp;
			gpsInfo->latitude = (float)(tempInt+(((temp-tempInt)/6)*10));

			tempLong = atof(longitude)/100;
			tempIntLong = (int)tempLong;
			gpsInfo->longitude = (float)(tempIntLong+(((tempLong-tempIntLong)/6)*10));

			gpsInfo->num_sats = (int)(atof(num_sats));

			printf("time:%s:%s:%s UTC %s/%s/%s\n", gpsInfo->hours, gpsInfo->minutes, gpsInfo->seconds, gpsInfo->month, gpsInfo->day, gpsInfo->year);
			printf("latitude:%f%s\n",gpsInfo->latitude,gpsInfo->latDir) ;
			printf("longitude:%f%s\n",gpsInfo->longitude,gpsInfo->longDir);
			printf("fix quality:%s\n",gpsInfo->fix);
			printf("num sats:%d\n",gpsInfo->num_sats);
			printf("speed:%s\n",gpsInfo->speed);
		}
		else if(strstr(buffer, "$GPRMC") != NULL)
		{
			splitBuf = str_split(buffer, ',');
			strcpy(gpsInfo->rawdate,splitBuf[9]);
			gpsInfo->month = strndup(gpsInfo->rawdate,2);
			gpsInfo->day = strndup(gpsInfo->rawdate+2,2);
			gpsInfo->year = strndup(gpsInfo->rawdate+4,2);
		}

	}
	return NULL;
}
char** str_split(const char *str, char delimiter)
{
	int len, i, j;
	char* buf;
	char** ret;

	len = strlen(str);
	buf = malloc(len + 1);
	memcpy(buf, str, len + 1);

	j = 1;
	for(i=0; i < len; ++i)
	{
		if(buf[i] == delimiter)
		{
			j++;
		}
	}
	ret = malloc(sizeof(char*) * (j + 1));
	ret[j] = NULL;

	ret[0] = buf;
	j = 1;
	for(i=0; i < len; ++i)
	{
		if(buf[i] == delimiter)
		{
			buf[i] = '\0';
			ret[j++] = &buf[i + 1];
		}
	}
	return ret;
}

int readLine(char* str, int fd)
{
	int rcount = 0;
	char recvBuffer[2] = "";

	while(strncmp(recvBuffer, "\r", 1))
	{
		rcount += read(fd, recvBuffer, 1);
		if(rcount > 0)
		{
			str[rcount-1] = recvBuffer[0];
		}
	}

	str[rcount] = '\0';

	return rcount;
}

int init_tty()
{
	struct termios serial_old;
	struct termios serial_new;

	int fd = open(RX_PORT, O_RDONLY | O_NOCTTY | O_NDELAY);

    	if (fd == -1) {
        	perror("!");
        	return -1;
    	}
	printf("%s is open!\n",RX_PORT);

	tcgetattr(fd, &serial_old);

   	 // Set up Serial Configuration
    	serial_new.c_iflag = IGNPAR;
    	serial_new.c_oflag = 0;
    	serial_new.c_lflag = 0;
    	serial_new.c_cflag = CRTSCTS | CS8 | CLOCAL | CREAD;;

	tcflush(fd, TCIFLUSH);
    	tcsetattr(fd, TCSANOW, &serial_new); // Apply configuration

	return fd;
}

void initGpsStruct()
{
	gpsInfo = malloc(sizeof(struct GPS_DATA));
	gpsInfo->rawtime = malloc(sizeof(char) * 6);
	gpsInfo->rawdate = malloc(sizeof(char) * 6);
}


struct GPS_DATA *getGpsInfo()
{
	return gpsInfo;
}

void start_gps()
{
	int fd = init_tty();
	initGpsStruct();

	pthread_t tid;
//	pthread_mutex_t mutex;
	int thread_id;

//	pthread_mutex_init(&mutex, NULL);
	thread_id = pthread_create(&tid, NULL, uartReader_thread, fd);
}
