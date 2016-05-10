#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "bcm2835.h"


#define BLACK 0
#define WHITE 1

#define SSD_Display_Off				0xAE
#define SSD_Display_On				0xAF

#define SSD_Command_Mode			0x00 	/* C0 and DC bit are 0 				 */
#define SSD_Data_Mode					0x40	/* C0 bit is 0 and DC bit is 1 */

#define SSD_Set_ContrastLevel	0x81

#define SSD1306_SETCONTRAST 0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF

#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS 0xDA

#define SSD1306_SETVCOMDETECT 0xDB

#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9

#define SSD1306_SETMULTIPLEX 0xA8

#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10

#define SSD1306_SETSTARTLINE 0x40

#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLADDRESS 0x21
#define SSD1306_PAGEADDRESS 0x22

#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8

#define SSD1306_SEGREMAP 0xA0

#define SSD1306_CHARGEPUMP 0x8D
/*
 * opaque handle
 * details defined in implementation file
 */
typedef struct _SSD1306 SSD1306;
int16_t  cursor_x, cursor_y;

/*
 * library works for 32 and 64 row displays
 */
typedef enum _SSD1306_ROWS {
	SSD1306_ROWS_NONE = 0,
	SSD1306_ROWS_32 = 32,
	SSD1306_ROWS_64 = 64
} SSD1306_ROWS;

typedef enum _SSD1306_VCCSTATE {
	SSD1306_VCCSATE_NONE = 0,
	SSD1306_VCCSATE_EXTERNAL = 1,
	SSD1306_VCCSATE_SWITCHCAP = 2
} SSD1306_VCCSTATE;

#define OLED_I2C_RESET RPI_V2_GPIO_P1_22 /* GPIO 25 pin 12  */
#define SSD_Internal_Vcc			0x02

typedef enum { false, true } boolean;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef short int16_t;

#define _BV(bit) (1 << (bit))
#define swap(a, b) { int16_t t = a; a = b; b = t; }

uint8_t *poledbuff;	// Pointer to OLED data buffer in memory
int8_t _i2c_addr, dc, rst, cs;
int16_t ssd1306_lcdwidth, ssd1306_lcdheight;
uint8_t vcc_type;
int16_t  WIDTH, HEIGHT;   // this is the 'raw' display w/h - never changes
int16_t  _width, _height; // dependent on rotation
int16_t  cursor_x, cursor_y;
uint16_t textcolor, textbgcolor;
uint8_t  textsize;
uint8_t  rotation;
boolean  wrap;


boolean ssd1306_init(void);
void ssd1306_clearDisplay(void);
void ssd1306_begin(void);
void ssd1306_command(uint8_t c);
void ssd1306_command2(uint8_t c0, uint8_t c1);
void ssd1306_command3(uint8_t c0, uint8_t c1, uint8_t c2);
void ssd1306_reset(void);
void ssd1306_delay(unsigned int ms);

void ssd1306_drawPixel(int16_t x, int16_t y, uint16_t color);
void ssd1306_print(const char * string, uint16_t tcolor, uint16_t bcolor, uint8_t tsize);
size_t ssd1306_write(uint8_t c, uint16_t tcolor, uint16_t bcolor, uint8_t tsize);
void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size);
void ssd1306_display(void);
void ssd1306_setCursor(int16_t x, int16_t y);

void ssd1306_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);


