#include <getopt.h>
#include <pthread.h>
#include <semaphore.h>

#include "ssd1306.h"
#include "gps_uart.h"
#include "ui.h"
#include "gpsInfoStruct.h"


#define PRG_NAME        "piPro"
#define PRG_VERSION     "1.0"

#define XPOS 0
#define YPOS 1
#define DELTAY 2

struct GPS_DATA *gpsInfo;

void drawSatUnknown()
{
	ssd1306_setCursor(24,8);
	ssd1306_print("X", WHITE, BLACK, 1);
	ssd1306_display();
}

void drawSatBars(int num_bars)
{
	int x,y;

	x = 24;
	y = 15;
	ssd1306_setCursor(x,y);
	for(int i=1; i <= 4; i++)
	{
		if(i <= num_bars)
		{
			ssd1306_drawLine(x, y, x, y-(i*4), WHITE);
			ssd1306_drawLine(x+1, y, x+1, y-(i*4), WHITE);
			x+=4;
		}
		else
		{
			ssd1306_drawLine(x, y, x, y-(i*4), BLACK);
			ssd1306_drawLine(x+1, y, x+1, y-(i*4), BLACK);
			x+=4;
		}
	}
    ssd1306_display();
}

/*


void drawSatUnknown()
{
	display.setTextSize(1);
	display.setTextColor(WHITE, BLACK);
	display.setCursor(24,8);
	display.printf("X");
	display.display();
	display.setTextSize(1);
}

void drawVideoFormat(int selection)
{
	switch(selection){
		case 1:
		{
			//display.fillRect(24,54,80,12,BLACK);
			//display.display();
			display.setTextSize(1);
			display.setTextColor(WHITE, BLACK);
			display.setCursor(24,54);
			display.print("1080p/30fps");
			display.display();
			display.setTextSize(1);
			break;
		}
		case 2:
		{
			//display.fillRect(24,54,80,12,BLACK);
			//display.display();
			display.setTextSize(1);
			display.setTextColor(WHITE, BLACK);
			display.setCursor(24,54);
			display.print(" 720p/60fps");
			display.display();
			display.setTextSize(1);
			break;
		}
		case 3:
		{
			//display.fillRect(24,54,80,12,BLACK);
			//display.display();
			display.setTextSize(1);
			display.setTextColor(WHITE, BLACK);
			display.setCursor(24,54);
			display.print(" 720p/30fps");
			display.display();
			display.setTextSize(1);
			break;
		}
		default:
		{
			//display.fillRect(24,54,80,12,BLACK);
			//display.display();
			display.setTextSize(1);
			display.setTextColor(WHITE, BLACK);
			display.setCursor(24,54);
			display.print("1080p/30fps");
			display.display();
			display.setTextSize(1);
			break;
		}
	}


}

void displayMemory(int mem)
{
	//display.fillRect(0,100,27,12,BLACK);
	//display.display();
	display.setTextSize(1);
	display.setTextColor(WHITE, BLACK);
	display.setCursor(100,8);
	display.printf("%02d%%",mem);
	display.display();
	display.setTextSize(1);
}

void *timer_thread(void *arg)
{
	int i=0;
	int sec, min;

	while(1)
	{
	//		display.fillRect(80,28,100,24,BLACK);
	//		display.display();
			sec = i % 60;
			min = i/60 % 59;
			display.setTextSize(2);
			display.setTextColor(WHITE, BLACK);
			display.setCursor(24,28);
			display.printf("00:%02d:%02d",min, sec);
			display.display();
			display.setTextSize(1);
			sleep(1);
			i++;
	}
	return NULL;
}
*/

void *gps_thread(void *arg)
{
	char strOutput[128];

	while(1)
	{
		gpsInfo = getGpsInfo();

		if(gpsInfo->num_sats == 0)
			drawSatUnknown();
		else if(gpsInfo->num_sats >= 11)
			drawSatBars(4);
		else if(gpsInfo->num_sats >= 7)
			drawSatBars(3);
		else if(gpsInfo->num_sats >= 3)
			drawSatBars(2);
		else if(gpsInfo->num_sats >= 1)
			drawSatBars(1);


		ssd1306_setCursor(0,16);
		sprintf(strOutput, "%s:%s:%s\n", gpsInfo->hours, gpsInfo->minutes, gpsInfo->seconds);
		ssd1306_print(strOutput,WHITE,BLACK,2);
		ssd1306_display();
	}
	return NULL;
}

int main(int argc, char **argv)
{
//	int i=0;
//	int sec, min;
	char strOutput[128];

	pthread_t pth;

	ssd1306_init();
	ssd1306_begin();
	ssd1306_clearDisplay();
	ssd1306_display();
	start_GPIO_UI();
	start_gps();

	// text display tests
	sleep(5);
	ssd1306_setCursor(0,8);
	ssd1306_print("GPS",WHITE,BLACK,1);
	ssd1306_display();
	pthread_create(&pth,NULL,gps_thread, NULL);
	pthread_join(pth,NULL);
//	while(1)
//	{
//
//	}
//	display.fillRect(100,32,110,32+7,1);
//	display.display();
//	sleep(2);
//	display.drawLine(100, 32, 100, 32+7, 0);
//	displayMemory(80);
	//drawSatBar(20,16,12);
	//drawSatBar(24,16,6);
	//drawSatBar(28,16,0);
//	drawSatUnknown();
//	display.print("\n");
//	drawVideoFormat(1);
//	sleep(2);
//	drawVideoFormat(2);
	//sleep(2);
	//drawVideoFormat(3);
	//display.setTextColor(WHITE);
	//display.printf("0x%8X\n", 0xDEADBEEF);
	//display.display();
//	sleep(2);
	//for(i=0; i<120; i++)
	/* Create worker thread */
//	pthread_create(&pth,NULL,timer_thread, NULL);
//	while(1)
//	{
//		display.fillRect(80,28,100,24,BLACK);
//		display.display();
//		sec = i % 60;
//		min = i/60 % 59;
//		display.setTextSize(2);
//		display.setTextColor(WHITE, BLACK);
//		display.setCursor(24,28);
//		display.printf("00:%02d:%02d",min, sec);
//		display.display();
//		display.setTextSize(1);
//		sleep(1);
//		i++;
//	}


//  	display.close();
}

