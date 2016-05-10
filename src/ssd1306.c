#include "ssd1306.h"
#include "glcdfont.h"

typedef struct _SSD1306 {
	int i2cAddress;
	SSD1306_ROWS rowtype;
	SSD1306_VCCSTATE vccstate;

	unsigned char * bitmap;
	int width;
	int height;
} SSD1306;


void fastI2Cwrite(char* tbuf, uint32_t len) {
	bcm2835_i2c_write(tbuf, len);
}

boolean ssd1306_init()
{
	ssd1306_lcdwidth  = 128;
	ssd1306_lcdheight = 64;
	_i2c_addr = 0x3C;
	cursor_y = cursor_x = 0;
	//textsize = 1;

	// De-Allocate memory for OLED buffer if any
	if (poledbuff)
		free(poledbuff);

	// Allocate memory for OLED buffer
	poledbuff = (uint8_t *) malloc ( (ssd1306_lcdwidth * ssd1306_lcdheight / 8 ));
	if (!poledbuff)
    		return false;

	// Init Raspberry PI GPIO
  	if (!bcm2835_init())
    		return false;

	// default OLED are using internal boost VCC converter
	vcc_type = SSD_Internal_Vcc;
	// Init & Configure Raspberry PI I2C
	if (bcm2835_i2c_begin()==0)
		return false;

	bcm2835_i2c_setSlaveAddress(_i2c_addr) ;

  	// Setup reset pin direction as output
  	bcm2835_gpio_fsel(OLED_I2C_RESET, BCM2835_GPIO_FSEL_OUTP);

	return ( true);
}

void ssd1306_clearDisplay()
{
	memset(poledbuff, 0, (ssd1306_lcdwidth*ssd1306_lcdheight/8));
}


void ssd1306_begin()
{
	uint8_t multiplex;
  	uint8_t chargepump;
  	uint8_t compins;
  	uint8_t contrast;
  	uint8_t precharge;

	// Setup reset pin direction (used by both SPI and I2C)
  	bcm2835_gpio_fsel(OLED_I2C_RESET, BCM2835_GPIO_FSEL_OUTP);
  	bcm2835_gpio_write(OLED_I2C_RESET, HIGH);

  	// VDD (3.3V) goes high at start, lets just chill for a ms
  	usleep(1000);

	// bring reset low
  	bcm2835_gpio_write(OLED_I2C_RESET, LOW);

  	// wait 10ms
  	usleep(10000);

	// bring out of reset
  	bcm2835_gpio_write(OLED_I2C_RESET, HIGH);

	ssd1306_command(SSD_Display_Off);                    // 0xAE
	ssd1306_command2(SSD1306_SETDISPLAYCLOCKDIV, 0x80);      // 0xD5 + the suggested ratio 0x80
	ssd1306_command2(SSD1306_SETMULTIPLEX, 0x3F);
	ssd1306_command2(SSD1306_SETDISPLAYOFFSET, 0x00);        // 0xD3 + no offset
	ssd1306_command(SSD1306_SETSTARTLINE | 0x0);            // line #0
	ssd1306_command2(SSD1306_CHARGEPUMP, 0x14);
	ssd1306_command2(SSD1306_MEMORYMODE, 0x00);              // 0x20 0x0 act like ks0108
	ssd1306_command(SSD1306_SEGREMAP | 0x1);
	ssd1306_command(SSD1306_COMSCANDEC);
	ssd1306_command2(SSD1306_SETCOMPINS, 0x12);  // 0xDA
	ssd1306_command2(SSD_Set_ContrastLevel, 0xCF);
	ssd1306_command2(SSD1306_SETPRECHARGE, 0xF1); // 0xd9
	ssd1306_command2(SSD1306_SETVCOMDETECT, 0x40);  // 0xDB
	ssd1306_command(SSD1306_DISPLAYALLON_RESUME);    // 0xA4
	ssd1306_command(SSD1306_NORMALDISPLAY);         // 0xA6

	// Reset to default value in case of
	// no reset pin available on OLED
	ssd1306_command3( 0x21, 0, 127 );
	ssd1306_command3( 0x22, 0,   7 );
	//stopscroll();

	// Empty uninitialized buffer
	ssd1306_clearDisplay();
	ssd1306_command(SSD_Display_On);

}

void ssd1306_command(uint8_t c)
{
	char buff[2] ;

	// Clear D/C to switch to command mode
	buff[0] = SSD_Command_Mode ;
	buff[1] = c;

	// Write Data on I2C
	fastI2Cwrite(buff, sizeof(buff));

}

void ssd1306_command2(uint8_t c0, uint8_t c1)
{
	char buff[3];
	buff[1] = c0;
	buff[2] = c1;

	// Clear D/C to switch to command mode
	buff[0] = SSD_Command_Mode ;

	// Write Data on I2C
	fastI2Cwrite(buff, 3);
}

void ssd1306_command3(uint8_t c0, uint8_t c1, uint8_t c2)
{
	char buff[4] ;

	buff[1] = c0;
	buff[2] = c1;
	buff[3] = c2;

	// Clear D/C to switch to command mode
	buff[0] = SSD_Command_Mode;

	// Write Data on I2C
	fastI2Cwrite(buff, sizeof(buff));

}


void ssd1306_reset()
{

}
/*
void ssd1306_delay(unsigned int ms)
{
	struct timespec sleeper, dummy ;

	sleeper.tv_sec  = (time_t)(ms / 1000) ;
	sleeper.tv_nsec = (long)(ms % 1000) * 1000000 ;

	nanosleep (&sleeper, &dummy) ;
}
*/
void ssd1306_drawPixel(int16_t x, int16_t y, uint16_t color)
{
	uint8_t * p = poledbuff ;

  	if ((x < 0) || (x >= ssd1306_lcdwidth) || (y < 0) || (y >= ssd1306_lcdheight))
    	return;

	// check rotation, move pixel around if necessary
	/*
	switch (getRotation())
	{
		case 1:
			swap(x, y);
			x = WIDTH - x - 1;
		break;

		case 2:
			x = WIDTH - x - 1;
			y = HEIGHT - y - 1;
		break;

		case 3:
			swap(x, y);
			y = HEIGHT - y - 1;
		break;
	}
	*/

	// Get where to do the change in the buffer
	p = poledbuff + (x + (y/8)*ssd1306_lcdwidth );

	// x is which column
	if (color == WHITE)
		*p |=  _BV((y%8));
	else
		*p &= ~_BV((y%8));
}

void ssd1306_setCursor(int16_t x, int16_t y)
{
  	cursor_x = x;
  	cursor_y = y;
}


void ssd1306_print(const char * string, uint16_t tcolor, uint16_t bcolor, uint8_t tsize)
{
	const char * p = string;
	int n = strlen(string);

	while (*p != 0 && n-->0)
	{
		ssd1306_write( (uint8_t) *p++, tcolor, bcolor, tsize);
	}

}

size_t ssd1306_write(uint8_t c, uint16_t tcolor, uint16_t bcolor, uint8_t tsize)
{
  	if (c == '\n')
	{
    		cursor_y += tsize*8;
    		cursor_x = 0;
  	}
	else if (c == '\r')
	{
    		// skip em
  	}
	else
	{
    		drawChar(cursor_x, cursor_y, c, tcolor, bcolor, tsize);
    		cursor_x += tsize*6;

		if (wrap && (cursor_x > (ssd1306_lcdwidth - tsize*6)))
		{
      			cursor_y += tsize*8;
      			cursor_x = 0;
    		}
  	}
  	return 1;

}

void ssd1306_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
  int16_t steep = abs(y1 - y0) > abs(x1 - x0);

	if (steep)
	{
    swap(x0, y0);
    swap(x1, y1);
  }

  if (x0 > x1)
	{
    swap(x0, x1);
    swap(y0, y1);
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1)
	{
    ystep = 1;
  }
	else
	{
    ystep = -1;
  }

  for (; x0<=x1; x0++)
	{
    if (steep)
		{
      ssd1306_drawPixel(y0, x0, color);
    }
		else
		{
      ssd1306_drawPixel(x0, y0, color);
		}
    err -= dy;

		if (err < 0)
		{
      y0 += ystep;
      err += dx;
    }
  }
}

void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
  // stupidest version - update in subclasses if desired!
  ssd1306_drawLine(x, y, x, y+h-1, color);
}

void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
  // stupidest version - update in subclasses if desired!
  for (int16_t i=x; i<x+w; i++)
	{
    drawFastVLine(i, y, h, color);
  }
}

void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size)
{
	if((x >= ssd1306_lcdwidth)            || // Clip right
     	   (y >= ssd1306_lcdheight)           || // Clip bottom
           ((x + 5 * size - 1) < 0) || // Clip left
           ((y + 8 * size - 1) < 0))   // Clip top
        	return;

  	for (int8_t i=0; i<6; i++ )
	{
    		uint8_t line;
    		if (i == 5)
      			line = 0x0;
    		else
      			line = font[(c*5)+i];
    			for (int8_t j = 0; j<8; j++)
			{
      				if (line & 0x1)
				{
        				if (size == 1) // default size
          					ssd1306_drawPixel(x+i, y+j, color);
        				else
					{  // big size
          					fillRect(x+(i*size), y+(j*size), size, size, color);
					}
      			}
				else if (bg != color)
				{
        				if (size == 1) // default size
          					ssd1306_drawPixel(x+i, y+j, bg);
        				else
					{  // big size
          					fillRect(x+i*size, y+j*size, size, size, bg);
        				}
      				}

      				line >>= 1;
    			}
  	}
}

void ssd1306_display(void)
{
	ssd1306_command(SSD1306_SETLOWCOLUMN  | 0x0); // low col = 0
  	ssd1306_command(SSD1306_SETHIGHCOLUMN | 0x0); // hi col = 0
  	ssd1306_command(SSD1306_SETSTARTLINE  | 0x0); // line #0

	uint16_t i=0 ;

	// pointer to OLED data buffer
	uint8_t * p = poledbuff;

	char buff[17] ;
	uint8_t x ;

	// Setup D/C to switch to data mode
	buff[0] = SSD_Data_Mode;

	// loop trough all OLED buffer and
    // send a bunch of 16 data byte in one xmission
    for ( i=0; i<(ssd1306_lcdwidth*ssd1306_lcdheight/8); i+=16 )
	{
    	for (x=1; x<=16; x++)
			buff[x] = *p++;
		fastI2Cwrite(buff,  17);
	}
}

