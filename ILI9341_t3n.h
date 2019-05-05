// https://github.com/PaulStoffregen/ILI9341_t3n
// http://forum.pjrc.com/threads/26305-Highly-optimized-ILI9341-(320x240-TFT-color-display)-library
// Warning this is Kurt's hacked up version using my SPIN library to allow different SPI busses
// Also allows use on only one valid CS pin on that buss, which must be used for DC

/***************************************************
  This is our library for the Adafruit  ILI9341 Breakout and Shield
  ----> http://www.adafruit.com/products/1651

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

// <SoftEgg>

//Additional graphics routines by Tim Trzepacz, SoftEgg LLC added December 2015
//(And then accidentally deleted and rewritten March 2016. Oops!)
//Gradient support
//----------------
//		fillRectVGradient	- fills area with vertical gradient
//		fillRectHGradient	- fills area with horizontal gradient
//		fillScreenVGradient - fills screen with vertical gradient
// 	fillScreenHGradient - fills screen with horizontal gradient

//Additional Color Support
//------------------------
//		color565toRGB		- converts 565 format 16 bit color to RGB
//		color565toRGB14		- converts 16 bit 565 format color to 14 bit RGB (2 bits clear for math and sign)
//		RGB14tocolor565		- converts 14 bit RGB back to 16 bit 565 format color

//Low Memory Bitmap Support
//-------------------------
// writeRect8BPP - 	write 8 bit per pixel paletted bitmap
// writeRect4BPP - 	write 4 bit per pixel paletted bitmap
// writeRect2BPP - 	write 2 bit per pixel paletted bitmap
// writeRect1BPP - 	write 1 bit per pixel paletted bitmap

//String Pixel Length support
//---------------------------
//		strPixelLen			- gets pixel length of given ASCII string

// <\SoftEgg>
// Also some of this comes from the DMA version of the library...

/* ILI9341_t3DMA library code is placed under the MIT license
 * Copyright (c) 2016 Frank Bösing
 *
*/

#ifndef _ILI9341_t3NH_
#define _ILI9341_t3NH_
#define  ILI9341_USE_DMAMEM

// Allow us to enable or disable capabilities, particully Frame Buffer and Clipping for speed and size
#ifndef DISABLE_ILI9341_FRAMEBUFFER
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)
#define ENABLE_ILI9341_FRAMEBUFFER
//#define SCREEN_DMA_NUM_SETTINGS (((uint32_t)((2 * ILI9341_TFTHEIGHT * ILI9341_TFTWIDTH) / 65536UL))+1)
#define SCREEN_DMA_NUM_SETTINGS 3 // see if making it a constant value makes difference...
#elif defined(__IMXRT1052__) || defined(__IMXRT1062__)
#define ENABLE_ILI9341_FRAMEBUFFER
//#define SCREEN_DMA_NUM_SETTINGS (((uint32_t)((2 * ILI9341_TFTHEIGHT * ILI9341_TFTWIDTH) / 65536UL))+1)
#define SCREEN_DMA_NUM_SETTINGS 4 // see if making it a constant value makes difference...
#endif
#endif

// Allow way to override using SPI

#ifdef __cplusplus
#include "Arduino.h"
#include <SPI.h>
#include <SPIN.h>
#include <DMAChannel.h>

#endif

#define ILI9341_TFTWIDTH  240
#define ILI9341_TFTHEIGHT 320

#define ILI9341_NOP     0x00
#define ILI9341_SWRESET 0x01
#define ILI9341_RDDID   0x04
#define ILI9341_RDDST   0x09

#define ILI9341_SLPIN   0x10
#define ILI9341_SLPOUT  0x11
#define ILI9341_PTLON   0x12
#define ILI9341_NORON   0x13

#define ILI9341_RDMODE  0x0A
#define ILI9341_RDMADCTL  0x0B
#define ILI9341_RDPIXFMT  0x0C
#define ILI9341_RDIMGFMT  0x0D
#define ILI9341_RDSELFDIAG  0x0F

#define ILI9341_INVOFF  0x20
#define ILI9341_INVON   0x21
#define ILI9341_GAMMASET 0x26
#define ILI9341_DISPOFF 0x28
#define ILI9341_DISPON  0x29

#define ILI9341_CASET   0x2A
#define ILI9341_PASET   0x2B
#define ILI9341_RAMWR   0x2C
#define ILI9341_RAMRD   0x2E

#define ILI9341_PTLAR    0x30
#define ILI9341_MADCTL   0x36
#define ILI9341_VSCRSADD 0x37
#define ILI9341_PIXFMT   0x3A

#define ILI9341_FRMCTR1 0xB1
#define ILI9341_FRMCTR2 0xB2
#define ILI9341_FRMCTR3 0xB3
#define ILI9341_INVCTR  0xB4
#define ILI9341_DFUNCTR 0xB6

#define ILI9341_PWCTR1  0xC0
#define ILI9341_PWCTR2  0xC1
#define ILI9341_PWCTR3  0xC2
#define ILI9341_PWCTR4  0xC3
#define ILI9341_PWCTR5  0xC4
#define ILI9341_VMCTR1  0xC5
#define ILI9341_VMCTR2  0xC7

#define ILI9341_RDID1   0xDA
#define ILI9341_RDID2   0xDB
#define ILI9341_RDID3   0xDC
#define ILI9341_RDID4   0xDD

#define ILI9341_GMCTRP1 0xE0
#define ILI9341_GMCTRN1 0xE1
/*
#define ILI9341_PWCTR6  0xFC

*/

// Color definitions
#define ILI9341_BLACK       0x0000      /*   0,   0,   0 */
#define ILI9341_NAVY        0x000F      /*   0,   0, 128 */
#define ILI9341_DARKGREEN   0x03E0      /*   0, 128,   0 */
#define ILI9341_DARKCYAN    0x03EF      /*   0, 128, 128 */
#define ILI9341_MAROON      0x7800      /* 128,   0,   0 */
#define ILI9341_PURPLE      0x780F      /* 128,   0, 128 */
#define ILI9341_OLIVE       0x7BE0      /* 128, 128,   0 */
#define ILI9341_LIGHTGREY   0xC618      /* 192, 192, 192 */
#define ILI9341_DARKGREY    0x7BEF      /* 128, 128, 128 */
#define ILI9341_BLUE        0x001F      /*   0,   0, 255 */
#define ILI9341_GREEN       0x07E0      /*   0, 255,   0 */
#define ILI9341_CYAN        0x07FF      /*   0, 255, 255 */
#define ILI9341_RED         0xF800      /* 255,   0,   0 */
#define ILI9341_MAGENTA     0xF81F      /* 255,   0, 255 */
#define ILI9341_YELLOW      0xFFE0      /* 255, 255,   0 */
#define ILI9341_WHITE       0xFFFF      /* 255, 255, 255 */
#define ILI9341_ORANGE      0xFD20      /* 255, 165,   0 */
#define ILI9341_GREENYELLOW 0xAFE5      /* 173, 255,  47 */
#define ILI9341_PINK        0xF81F

#define CL(_r,_g,_b) ((((_r)&0xF8)<<8)|(((_g)&0xFC)<<3)|((_b)>>3))

#define sint16_t int16_t

typedef struct {
	const unsigned char *index;
	const unsigned char *unicode;
	const unsigned char *data;
	unsigned char version;
	unsigned char reserved;
	unsigned char index1_first;
	unsigned char index1_last;
	unsigned char index2_first;
	unsigned char index2_last;
	unsigned char bits_index;
	unsigned char bits_width;
	unsigned char bits_height;
	unsigned char bits_xoffset;
	unsigned char bits_yoffset;
	unsigned char bits_delta;
	unsigned char line_space;
	unsigned char cap_height;
} ILI9341_t3_font_t;

#define ILI9341_DMA_INIT	0x01 	// We have init the Dma settings
#define ILI9341_DMA_CONT	0x02 	// continuous mode
#define ILI9341_DMA_ACTIVE  0x80    // Is currently active

//These enumerate the text plotting alignment (reference datum point)
#define TL_DATUM 0 // Top left (default)
#define TC_DATUM 1 // Top centre
#define TR_DATUM 2 // Top right
#define ML_DATUM 3 // Middle left
#define CL_DATUM 3 // Centre left, same as above
#define MC_DATUM 4 // Middle centre
#define CC_DATUM 4 // Centre centre, same as above
#define MR_DATUM 5 // Middle right
#define CR_DATUM 5 // Centre right, same as above
#define BL_DATUM 6 // Bottom left
#define BC_DATUM 7 // Bottom centre
#define BR_DATUM 8 // Bottom right
//#define L_BASELINE  9 // Left character baseline (Line the 'A' character would sit on)
//#define C_BASELINE 10 // Centre character baseline
//#define R_BASELINE 11 // Right character baseline


#ifdef __cplusplus
// At all other speeds, ILI9241_KINETISK__pspi->beginTransaction() will use the fastest available clock
#ifdef KINETISK
#define ILI9341_SPICLOCK 30000000
#define ILI9341_SPICLOCK_READ 2000000
#elif defined(__IMXRT1052__) || defined(__IMXRT1062__)  // Teensy 4.x
#define ILI9341_SPICLOCK 144000000u
#define ILI9341_SPICLOCK_READ 2000000
#else
#define ILI9341_SPICLOCK 30000000
#define ILI9341_SPICLOCK_READ 2000000
#endif

extern void (*handleDmaComplete)();

class ILI9341_t3n : public Print
{
  public:
	ILI9341_t3n(uint8_t _CS, uint8_t _DC, uint8_t _RST = 255, 
		uint8_t _MOSI=11, uint8_t _SCLK=13, uint8_t _MISO=12, 
		SPINClass *pspin=(SPINClass*)(&SPIN));
	void begin(void);
  	void sleep(bool enable);		
	void pushColor(uint16_t color);
	void fillScreen(uint16_t color);
	void drawPixel(int16_t x, int16_t y, uint16_t color);
	void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
	void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
	void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
			
	void fillRectHGradient(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color1, uint16_t color2);
	void fillRectVGradient(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color1, uint16_t color2);
	void fillScreenVGradient(uint16_t color1, uint16_t color2);
	void fillScreenHGradient(uint16_t color1, uint16_t color2);

	void setRotation(uint8_t r);
	void setScroll(uint16_t offset);
	void invertDisplay(boolean i);
	void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
	// Pass 8-bit (each) R,G,B, get back 16-bit packed color
	static uint16_t color565(uint8_t r, uint8_t g, uint8_t b) {
		return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
	}

	//color565toRGB		- converts 565 format 16 bit color to RGB
	static void color565toRGB(uint16_t color, uint8_t &r, uint8_t &g, uint8_t &b) {
		r = (color>>8)&0x00F8;
		g = (color>>3)&0x00FC;
		b = (color<<3)&0x00F8;
	}
	
	//color565toRGB14		- converts 16 bit 565 format color to 14 bit RGB (2 bits clear for math and sign)
	//returns 00rrrrr000000000,00gggggg00000000,00bbbbb000000000
	//thus not overloading sign, and allowing up to double for additions for fixed point delta
	static void color565toRGB14(uint16_t color, int16_t &r, int16_t &g, int16_t &b) {
		r = (color>>2)&0x3E00;
		g = (color<<3)&0x3F00;
		b = (color<<9)&0x3E00;
	}
	
	//RGB14tocolor565		- converts 14 bit RGB back to 16 bit 565 format color
	static uint16_t RGB14tocolor565(int16_t r, int16_t g, int16_t b)
	{
		return (((r & 0x3E00) << 2) | ((g & 0x3F00) >>3) | ((b & 0x3E00) >> 9));
	}
	
	//uint8_t readdata(void);
	uint8_t readcommand8(uint8_t reg, uint8_t index = 0);

	// Added functions to read pixel data...
	uint16_t readPixel(int16_t x, int16_t y);
	void readRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t *pcolors);
	void writeRect(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t *pcolors);

	// writeRect8BPP - 	write 8 bit per pixel paletted bitmap
	//					bitmap data in array at pixels, one byte per pixel
	//					color palette data in array at palette
	void writeRect8BPP(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t *pixels, const uint16_t * palette );

	// writeRect4BPP - 	write 4 bit per pixel paletted bitmap
	//					bitmap data in array at pixels, 4 bits per pixel
	//					color palette data in array at palette
	//					width must be at least 2 pixels
	void writeRect4BPP(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t *pixels, const uint16_t * palette );
	
	// writeRect2BPP - 	write 2 bit per pixel paletted bitmap
	//					bitmap data in array at pixels, 4 bits per pixel
	//					color palette data in array at palette
	//					width must be at least 4 pixels
	void writeRect2BPP(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t *pixels, const uint16_t * palette );
	
	// writeRect1BPP - 	write 1 bit per pixel paletted bitmap
	//					bitmap data in array at pixels, 4 bits per pixel
	//					color palette data in array at palette
	//					width must be at least 8 pixels
	void writeRect1BPP(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t *pixels, const uint16_t * palette );

	// writeRectNBPP - 	write N(1, 2, 4, 8) bit per pixel paletted bitmap
	//					bitmap data in array at pixels
	//  Currently writeRect1BPP, writeRect2BPP, writeRect4BPP use this to do all of the work. 
	// 
	void writeRectNBPP(int16_t x, int16_t y, int16_t w, int16_t h,  uint8_t bits_per_pixel, 
		const uint8_t *pixels, const uint16_t * palette );

	// from Adafruit_GFX.h
	void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
	void drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color);
	void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
	void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color);
	void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
	void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
	void drawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color);
	void fillRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color);
	void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);
	void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size);
	void setCursor(int16_t x, int16_t y);
    void getCursor(int16_t *x, int16_t *y);
	void setTextColor(uint16_t c);
	void setTextColor(uint16_t c, uint16_t bg);
	void setTextSize(uint8_t s);
	uint8_t getTextSize();
	void setTextWrap(boolean w);
	boolean getTextWrap();

	// setOrigin sets an offset in display pixels where drawing to (0,0) will appear
	// for example: setOrigin(10,10); drawPixel(5,5); will cause a pixel to be drawn at hardware pixel (15,15)
	void setOrigin(int16_t x = 0, int16_t y = 0) { 
		_originx = x; _originy = y; 
		//if (Serial) Serial.printf("Set Origin %d %d\n", x, y);
		updateDisplayClip();
	}
	void getOrigin(int16_t* x, int16_t* y) { *x = _originx; *y = _originy; }

	// setClipRect() sets a clipping rectangle (relative to any set origin) for drawing to be limited to.
	// Drawing is also restricted to the bounds of the display

	void setClipRect(int16_t x1, int16_t y1, int16_t w, int16_t h) 
		{ _clipx1 = x1; _clipy1 = y1; _clipx2 = x1+w; _clipy2 = y1+h; 
			//if (Serial) Serial.printf("Set clip Rect %d %d %d %d\n", x1, y1, w, h);
			updateDisplayClip();
		}
	void setClipRect() {
			 _clipx1 = 0; _clipy1 = 0; _clipx2 = _width; _clipy2 = _height; 
			//if (Serial) Serial.printf("clear clip Rect\n");
			 updateDisplayClip(); 
		}

	virtual size_t write(uint8_t);
	int16_t width(void)  { return _width; }
	int16_t height(void) { return _height; }
	uint8_t getRotation(void);
	void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
	void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
	int16_t getCursorX(void) const { return cursor_x; }
	int16_t getCursorY(void) const { return cursor_y; }
	void setFont(const ILI9341_t3_font_t &f);
	void setFont() { font = NULL; }
	void setFontAdafruit(void) { font = NULL; }
	void drawFontChar(unsigned int c);
	int16_t strPixelLen(char * str);
	
	// added support for drawing strings/numbers/floats with centering
	// modified from tft_ili9341_ESP github library
	// Handle numbers
	int16_t  drawNumber(long long_num,int poX, int poY);
	int16_t  drawFloat(float floatNumber,int decimal,int poX, int poY);   
	// Handle char arrays
	int16_t drawString(const String& string, int poX, int poY);
	int16_t drawString1(char string[], int16_t len, int poX, int poY);

	void setTextDatum(uint8_t datum);
	
	// added support for scrolling text area
	// https://github.com/vitormhenrique/ILI9341_t3
	// Discussion regarding this optimized version:
    //http://forum.pjrc.com/threads/26305-Highly-optimized-ILI9341-%28320x240-TFT-color-display%29-library
	//	
	void setScrollTextArea(int16_t x, int16_t y, int16_t w, int16_t h);
	void setScrollBackgroundColor(uint16_t color);
	void enableScroll(void);
	void disableScroll(void);
	void scrollTextArea(uint8_t scrollSize);
	void resetScrollBackgroundColor(uint16_t color);

	// added support to use optional Frame buffer
	void	setFrameBuffer(uint16_t *frame_buffer);
	uint8_t useFrameBuffer(boolean b);		// use the frame buffer?  First call will allocate
	void	freeFrameBuffer(void);			// explicit call to release the buffer
	void	updateScreen(void);				// call to say update the screen now. 
	bool	updateScreenAsync(bool update_cont = false);	// call to say update the screen optinoally turn into continuous mode. 
	void	waitUpdateAsyncComplete(void);
	void	endUpdateAsync();			 // Turn of the continueous mode fla
	bool	drawImageAsync(int x1, int y1, int x2, int y2, uint16_t *buf);	// call to say update the screen void	waitUpdateAsyncComplete(void);
    inline void setHandleDmaComplete(void (*fptr)()) {
        handleDmaComplete = fptr;
    };
	void	dumpDMASettings();
	#ifdef ENABLE_ILI9341_FRAMEBUFFER
	uint16_t *getFrameBuffer() {return _pfbtft;}
	uint32_t frameCount() {return _dma_frame_count; }
	boolean	asyncUpdateActive(void)  {return (_dma_state & ILI9341_DMA_ACTIVE);}
	void	initDMASettings(void);
	#else
	uint32_t frameCount() {return 0; }
	uint16_t *getFrameBuffer() {return NULL;}
	boolean	asyncUpdateActive(void)  {return false;}
	#endif
 protected:
 	SPINClass *_pspin;
#if defined(KINETISK)
 	KINETISK_SPI_t *_pkinetisk_spi;
#elif defined(__IMXRT1052__) || defined(__IMXRT1062__)  // Teensy 4.x
 	IMXRT_LPSPI_t *_pimxrt_spi;

#elif defined(KINETISL)
 	KINETISL_SPI_t *_pkinetisl_spi;
#endif

	int16_t _width, _height; // Display w/h as modified by current rotation
	int16_t  cursor_x, cursor_y;

	int16_t  _clipx1, _clipy1, _clipx2, _clipy2;
	int16_t  _originx, _originy;
	int16_t  _displayclipx1, _displayclipy1, _displayclipx2, _displayclipy2;
	bool _invisible = false; 
	bool _standard = true; // no bounding rectangle or origin set. 

	inline void updateDisplayClip() {
		_displayclipx1 = max(0,min(_clipx1+_originx,width()));
		_displayclipx2 = max(0,min(_clipx2+_originx,width()));

		_displayclipy1 = max(0,min(_clipy1+_originy,height()));
		_displayclipy2 = max(0,min(_clipy2+_originy,height()));
		_invisible = (_displayclipx1 == _displayclipx2 || _displayclipy1 == _displayclipy2);
		_standard =  (_displayclipx1 == 0) && (_displayclipx2 == _width) && (_displayclipy1 == 0) && (_displayclipy2 == _height);
		if (Serial) {
			//Serial.printf("UDC (%d %d)-(%d %d) %d %d\n", _displayclipx1, _displayclipy1, _displayclipx2, 
			//	_displayclipy2, _invisible, _standard);

		}
	}
	
	int16_t scroll_x, scroll_y, scroll_width, scroll_height;
	boolean scrollEnable,isWritingScrollArea; // If set, 'wrap' text at right edge of display

	uint16_t textcolor, textbgcolor, scrollbgcolor;
	uint32_t textcolorPrexpanded, textbgcolorPrexpanded;
	uint8_t textsize, rotation, textdatum;
	boolean wrap; // If set, 'wrap' text at right edge of display
	const ILI9341_t3_font_t *font;
	// Anti-aliased font support
	uint8_t fontbpp = 1;
	uint8_t fontbppindex = 0;
	uint8_t fontbppmask = 1;
	uint8_t fontppb = 8;
	uint8_t* fontalphalut;
	float fontalphamx = 1;
	
	uint32_t padX;

  	uint8_t  _rst;
  	uint8_t _cs, _dc;
	uint8_t pcs_data, pcs_command;
	uint8_t _miso, _mosi, _sclk;
	// add support to allow only one hardware CS (used for dc)
#if defined(__IMXRT1052__) || defined(__IMXRT1062__)  // Teensy 4.x
    uint32_t _cspinmask;
    volatile uint32_t *_csport;
    uint32_t _spi_tcr_current;
    uint32_t _dcpinmask;
    volatile uint32_t *_dcport;
#else
    uint8_t _cspinmask;
    volatile uint8_t *_csport;
#endif
#ifdef KINETISL
    volatile uint8_t  *_dcport;
    uint8_t _dcpinmask;
#endif
#ifdef ENABLE_ILI9341_FRAMEBUFFER
    // Add support for optional frame buffer
    uint16_t	*_pfbtft;						// Optional Frame buffer 
    uint8_t		_use_fbtft;						// Are we in frame buffer mode?
    uint16_t	*_we_allocated_buffer;			// We allocated the buffer; 
    // Add DMA support. 
	static  ILI9341_t3n 		*_dmaActiveDisplay;  // Use pointer to this as a way to get back to object...
	static volatile uint8_t  	_dma_state;  		// DMA status
	static volatile uint32_t	_dma_frame_count;	// Can return a frame count...
	#if defined(__MK66FX1M0__) 
	// T3.6 use Scatter/gather with chain to do transfer
	static DMASetting 	_dmasettings[4];
	static DMAChannel  	_dmatx;
	#elif defined(__IMXRT1052__) || defined(__IMXRT1062__)  // Teensy 4.x
	// Going to try it similar to T4.
	static DMASetting 	_dmasettings[4];
	static DMAChannel  	_dmatx;
	uint32_t 			_spi_fcr_save;		// save away previous FCR register value
	#else
	// T3.5 - had issues scatter/gather so do just use channels/interrupts
	// and update and continue
	static DMAChannel  	_dmatx;
	static DMAChannel  	_dmarx;
	static uint16_t 	_dma_count_remaining;
	static uint16_t		_dma_write_size_words;
	#endif	
	static void dmaInterrupt(void);
	void process_dma_interrupt(void);
#endif

	void setAddr(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
	  __attribute__((always_inline)) {
		writecommand_cont(ILI9341_CASET); // Column addr set
		writedata16_cont(x0);   // XSTART
		writedata16_cont(x1);   // XEND
		writecommand_cont(ILI9341_PASET); // Row addr set
		writedata16_cont(y0);   // YSTART
		writedata16_cont(y1);   // YEND
	}
//. From Onewire utility files
#if defined(__IMXRT1052__) || defined(__IMXRT1062__)  // Teensy 4.x

	void DIRECT_WRITE_LOW(volatile uint32_t * base, uint32_t mask)  __attribute__((always_inline)) {
		*(base+34) = mask;
	}
	void DIRECT_WRITE_HIGH(volatile uint32_t * base, uint32_t mask)  __attribute__((always_inline)) {
		*(base+33) = mask;
	}
#endif

	void beginSPITransaction(uint32_t clock = ILI9341_SPICLOCK) __attribute__((always_inline)) {
		_pspin->beginTransaction(SPISettings(clock, MSBFIRST, SPI_MODE0));
		if (_csport) {
#if defined(__IMXRT1052__) || defined(__IMXRT1062__)  // Teensy 4.x 
			DIRECT_WRITE_LOW(_csport, _cspinmask);
#else
			*_csport  &= ~_cspinmask;
#endif
		}
	}
	void endSPITransaction() __attribute__((always_inline)) {
		if (_csport) {
#if defined(__IMXRT1052__) || defined(__IMXRT1062__)  // Teensy 4.x 
			DIRECT_WRITE_HIGH(_csport, _cspinmask);
#else
			*_csport |= _cspinmask;
#endif
		}
		_pspin->endTransaction();
	}
#if defined(KINETISK)	
	void writecommand_cont(uint8_t c) __attribute__((always_inline)) {
		_pkinetisk_spi->PUSHR = c | (pcs_command << 16) | SPI_PUSHR_CTAS(0) | SPI_PUSHR_CONT;
		_pspin->waitFifoNotFull();
	}
	void writedata8_cont(uint8_t c) __attribute__((always_inline)) {
		_pkinetisk_spi->PUSHR = c | (pcs_data << 16) | SPI_PUSHR_CTAS(0) | SPI_PUSHR_CONT;
		_pspin->waitFifoNotFull();
	}
	void writedata16_cont(uint16_t d) __attribute__((always_inline)) {
		_pkinetisk_spi->PUSHR = d | (pcs_data << 16) | SPI_PUSHR_CTAS(1) | SPI_PUSHR_CONT;
		_pspin->waitFifoNotFull();
	}
	void writecommand_last(uint8_t c) __attribute__((always_inline)) {
		uint32_t mcr = _pkinetisk_spi->MCR;
		_pkinetisk_spi->PUSHR = c | (pcs_command << 16) | SPI_PUSHR_CTAS(0) | SPI_PUSHR_EOQ;
		_pspin->waitTransmitComplete(mcr);
	}
	void writedata8_last(uint8_t c) __attribute__((always_inline)) {
		uint32_t mcr = _pkinetisk_spi->MCR;
		_pkinetisk_spi->PUSHR = c | (pcs_data << 16) | SPI_PUSHR_CTAS(0) | SPI_PUSHR_EOQ;
		_pspin->waitTransmitComplete(mcr);
	}
	void writedata16_last(uint16_t d) __attribute__((always_inline)) {
		uint32_t mcr = _pkinetisk_spi->MCR;
		_pkinetisk_spi->PUSHR = d | (pcs_data << 16) | SPI_PUSHR_CTAS(1) | SPI_PUSHR_EOQ;
		_pspin->waitTransmitComplete(mcr);
	}
#elif defined(__IMXRT1052__) || defined(__IMXRT1062__)  // Teensy 4.x 
	#define TCR_MASK  (LPSPI_TCR_PCS(3) | LPSPI_TCR_FRAMESZ(31) | LPSPI_TCR_CONT | LPSPI_TCR_RXMSK )
	void maybeUpdateTCR(uint32_t requested_tcr_state) /*__attribute__((always_inline)) */ {
		if ((_spi_tcr_current & TCR_MASK) != requested_tcr_state) {
			bool dc_state_change = (_spi_tcr_current & LPSPI_TCR_PCS(3)) != (requested_tcr_state & LPSPI_TCR_PCS(3));
			_spi_tcr_current = (_spi_tcr_current & ~TCR_MASK) | requested_tcr_state ;
			// only output when Transfer queue is empty.
			if (!dc_state_change || !_dcpinmask) {
				while ((_pimxrt_spi->FSR & 0x1f) )	;
				_pimxrt_spi->TCR = _spi_tcr_current;	// update the TCR

			} else {
				_pspin->waitTransmitComplete();
				if (requested_tcr_state & LPSPI_TCR_PCS(3)) DIRECT_WRITE_HIGH(_dcport, _dcpinmask);
				else DIRECT_WRITE_LOW(_dcport, _dcpinmask);
				_pimxrt_spi->TCR = _spi_tcr_current & ~(LPSPI_TCR_PCS(3) | LPSPI_TCR_CONT);	// go ahead and update TCR anyway?  

			}
		}
	}

	// BUGBUG:: currently assumming we only have CS_0 as valid CS
	void writecommand_cont(uint8_t c) __attribute__((always_inline)) {
		maybeUpdateTCR(LPSPI_TCR_PCS(0) | LPSPI_TCR_FRAMESZ(7) /*| LPSPI_TCR_CONT*/);
		_pimxrt_spi->TDR = c;
		_pspin->pending_rx_count++;	//
		_pspin->waitFifoNotFull();
	}
	void writedata8_cont(uint8_t c) __attribute__((always_inline)) {
		maybeUpdateTCR(LPSPI_TCR_PCS(1) | LPSPI_TCR_FRAMESZ(7) | LPSPI_TCR_CONT);
		_pimxrt_spi->TDR = c;
		_pspin->pending_rx_count++;	//
		_pspin->waitFifoNotFull();
	}
	void writedata16_cont(uint16_t d) __attribute__((always_inline)) {
		maybeUpdateTCR(LPSPI_TCR_PCS(1) | LPSPI_TCR_FRAMESZ(15) | LPSPI_TCR_CONT);
		_pimxrt_spi->TDR = d;
		_pspin->pending_rx_count++;	//
		_pspin->waitFifoNotFull();
	}
	void writecommand_last(uint8_t c) __attribute__((always_inline)) {
		maybeUpdateTCR(LPSPI_TCR_PCS(0) | LPSPI_TCR_FRAMESZ(7));
		_pimxrt_spi->TDR = c;
//		_pimxrt_spi->SR = LPSPI_SR_WCF | LPSPI_SR_FCF | LPSPI_SR_TCF;
		_pspin->pending_rx_count++;	//
		_pspin->waitTransmitComplete();
	}
	void writedata8_last(uint8_t c) __attribute__((always_inline)) {
		maybeUpdateTCR(LPSPI_TCR_PCS(1) | LPSPI_TCR_FRAMESZ(7));
		_pimxrt_spi->TDR = c;
//		_pimxrt_spi->SR = LPSPI_SR_WCF | LPSPI_SR_FCF | LPSPI_SR_TCF;
		_pspin->pending_rx_count++;	//
		_pspin->waitTransmitComplete();
	}
	void writedata16_last(uint16_t d) __attribute__((always_inline)) {
		maybeUpdateTCR(LPSPI_TCR_PCS(1) | LPSPI_TCR_FRAMESZ(15));
		_pimxrt_spi->TDR = d;
//		_pimxrt_spi->SR = LPSPI_SR_WCF | LPSPI_SR_FCF | LPSPI_SR_TCF;
		_pspin->pending_rx_count++;	//
		_pspin->waitTransmitComplete();
	}

#elif defined (KINETISL)
// Lets see how hard to make it work OK with T-LC
	uint8_t _dcpinAsserted;
	uint8_t _data_sent_not_completed;
	void waitTransmitComplete()  {
		while (_data_sent_not_completed) {
			uint16_t timeout_count = 0xff; // hopefully enough 
			while (!(_pkinetisl_spi->S & SPI_S_SPRF) && timeout_count--) ; // wait 
			uint8_t d __attribute__((unused));
			d = _pkinetisl_spi->DL;
			d = _pkinetisl_spi->DH;
			_data_sent_not_completed--; // We hopefully received our data...
		}
	}
	uint16_t waitTransmitCompleteReturnLast()  {
		uint16_t d = 0;
		while (_data_sent_not_completed) {
			uint16_t timeout_count = 0xff; // hopefully enough 
			while (!(_pkinetisl_spi->S & SPI_S_SPRF) && timeout_count--) ; // wait 
			d = (_pkinetisl_spi->DH << 8) | _pkinetisl_spi->DL;
			_data_sent_not_completed--; // We hopefully received our data...
		}
		return d;
	}

	void setCommandMode() __attribute__((always_inline)) {
		if (!_dcpinAsserted) {
			waitTransmitComplete();
			*_dcport  &= ~_dcpinmask;
			_dcpinAsserted = 1;
		}
	}

	void setDataMode() __attribute__((always_inline)) {
		if (_dcpinAsserted) {
			waitTransmitComplete();
			*_dcport  |= _dcpinmask;
			_dcpinAsserted = 0;
		}
	}

	void outputToSPI(uint8_t c) {
		if (_pkinetisl_spi->C2 & SPI_C2_SPIMODE) {
			// Wait to change modes until any pending output has been done.
			waitTransmitComplete();
			_pkinetisl_spi->C2 = 0; // make sure 8 bit mode.
		}
		while (!(_pkinetisl_spi->S & SPI_S_SPTEF)) ; // wait if output buffer busy.
		// Clear out buffer if there is something there...
		if  ((_pkinetisl_spi->S & SPI_S_SPRF)) {
			uint8_t d __attribute__((unused));
			d = _pkinetisl_spi->DL;
			_data_sent_not_completed--;
		} 
		_pkinetisl_spi->DL = c; // output byte
		_data_sent_not_completed++; // let system know we sent something	
	}

	void outputToSPI16(uint16_t data)  {
		if (!(_pkinetisl_spi->C2 & SPI_C2_SPIMODE)) {
			// Wait to change modes until any pending output has been done.
			waitTransmitComplete();
			_pkinetisl_spi->C2 = SPI_C2_SPIMODE; // make sure 8 bit mode.
		}
		uint8_t s;
		do {
			s = _pkinetisl_spi->S;
			 // wait if output buffer busy.
			// Clear out buffer if there is something there...
			if  ((s & SPI_S_SPRF)) {
				uint8_t d __attribute__((unused));
				d = _pkinetisl_spi->DL;
				d = _pkinetisl_spi->DH;
				_data_sent_not_completed--; 	// let system know we sent something	
			}

		} while (!(s & SPI_S_SPTEF) || (s & SPI_S_SPRF));

		_pkinetisl_spi->DL = data; 		// output low byte
		_pkinetisl_spi->DH = data >> 8; // output high byte
		_data_sent_not_completed++; 	// let system know we sent something	
	}

	void writecommand_cont(uint8_t c)  {
		setCommandMode();
		outputToSPI(c);
	}
	void writedata8_cont(uint8_t c) {
		setDataMode();
		outputToSPI(c);
	}

	void writedata16_cont(uint16_t c)  {
		setDataMode();
		outputToSPI16(c);
	}

	void writecommand_last(uint8_t c)  {
		setCommandMode();
		outputToSPI(c);
		waitTransmitComplete();
	}
	void writedata8_last(uint8_t c)  {
		setDataMode();
		outputToSPI(c);
		waitTransmitComplete();
	}
	void writedata16_last(uint16_t c) {
		setDataMode();
		outputToSPI16(c);
		waitTransmitComplete();
	}

#endif


	void HLine(int16_t x, int16_t y, int16_t w, uint16_t color)
	  __attribute__((always_inline)) {
		#ifdef ENABLE_ILI9341_FRAMEBUFFER
	  	if (_use_fbtft) {
	  		drawFastHLine(x, y, w, color);
	  		return;
	  	}
	  	#endif
	    x+=_originx;
	    y+=_originy;

	    // Rectangular clipping
	    if((y < _displayclipy1) || (x >= _displayclipx2) || (y >= _displayclipy2)) return;
	    if(x<_displayclipx1) { w = w - (_displayclipx1 - x); x = _displayclipx1; }
	    if((x+w-1) >= _displayclipx2)  w = _displayclipx2-x;
	    if (w<1) return;

		setAddr(x, y, x+w-1, y);
		writecommand_cont(ILI9341_RAMWR);
		do { writedata16_cont(color); } while (--w > 0);
	}
	void VLine(int16_t x, int16_t y, int16_t h, uint16_t color)
	  __attribute__((always_inline)) {
		#ifdef ENABLE_ILI9341_FRAMEBUFFER
	  	if (_use_fbtft) {
	  		drawFastVLine(x, y, h, color);
	  		return;
	  	}
	  	#endif
		x+=_originx;
	    y+=_originy;

	    // Rectangular clipping
	    if((x < _displayclipx1) || (x >= _displayclipx2) || (y >= _displayclipy2)) return;
	    if(y < _displayclipy1) { h = h - (_displayclipy1 - y); y = _displayclipy1;}
	    if((y+h-1) >= _displayclipy2) h = _displayclipy2-y;
	    if(h<1) return;

		setAddr(x, y, x, y+h-1);
		writecommand_cont(ILI9341_RAMWR);
		do { writedata16_cont(color); } while (--h > 0);
	}
	/**
	 * Found in a pull request for the Adafruit framebuffer library. Clever!
	 * https://github.com/tricorderproject/arducordermini/pull/1/files#diff-d22a481ade4dbb4e41acc4d7c77f683d
	 * Converts  0000000000000000rrrrrggggggbbbbb
	 *     into  00000gggggg00000rrrrr000000bbbbb
	 * with mask 00000111111000001111100000011111
	 * This is useful because it makes space for a parallel fixed-point multiply
	 * This implements the linear interpolation formula: result = bg * (1.0 - alpha) + fg * alpha
	 * This can be factorized into: result = bg + (fg - bg) * alpha
	 * alpha is in Q1.5 format, so 0.0 is represented by 0, and 1.0 is represented by 32
	 * @param	fg		Color to draw in RGB565 (16bit)
	 * @param	bg		Color to draw over in RGB565 (16bit)
	 * @param	alpha	Alpha in range 0-255
	 **/
	uint16_t alphaBlendRGB565( uint32_t fg, uint32_t bg, uint8_t alpha )
	 __attribute__((always_inline)) {
	 	alpha = ( alpha + 4 ) >> 3; // from 0-255 to 0-31
		bg = (bg | (bg << 16)) & 0b00000111111000001111100000011111;
		fg = (fg | (fg << 16)) & 0b00000111111000001111100000011111;
		uint32_t result = ((((fg - bg) * alpha) >> 5) + bg) & 0b00000111111000001111100000011111;
		return (uint16_t)((result >> 16) | result); // contract result
	}
	/**
	 * Same as above, but fg and bg are premultiplied, and alpah is already in range 0-31
	 */
	uint16_t alphaBlendRGB565Premultiplied( uint32_t fg, uint32_t bg, uint8_t alpha )
	 __attribute__((always_inline)) {
		uint32_t result = ((((fg - bg) * alpha) >> 5) + bg) & 0b00000111111000001111100000011111;
		return (uint16_t)((result >> 16) | result); // contract result
	}
	void Pixel(int16_t x, int16_t y, uint16_t color)
	  __attribute__((always_inline)) {
	    x+=_originx;
	    y+=_originy;

	  	if((x < _displayclipx1) ||(x >= _displayclipx2) || (y < _displayclipy1) || (y >= _displayclipy2)) return;

		#ifdef ENABLE_ILI9341_FRAMEBUFFER
	  	if (_use_fbtft) {
	  		_pfbtft[y*_width + x] = color;
	  		return;
	  	}
	  	#endif
		setAddr(x, y, x, y);
		writecommand_cont(ILI9341_RAMWR);
		writedata16_cont(color);
	}
	void drawFontBits(bool opaque, uint32_t bits, uint32_t numbits, int32_t x, int32_t y, uint32_t repeat);
	void drawFontPixel( uint8_t alpha, uint32_t x, uint32_t y );
	uint32_t fetchpixel(const uint8_t *p, uint32_t index, uint32_t x);

};

#ifndef swap
#define swap(a, b) { typeof(a) t = a; a = b; b = t; }
#endif

// To avoid conflict when also using Adafruit_GFX or any Adafruit library
// which depends on Adafruit_GFX, #include the Adafruit library *BEFORE*
// you #include ILI9341_t3.h.
#ifndef _ADAFRUIT_GFX_H
class Adafruit_GFX_Button {
public:
	Adafruit_GFX_Button(void) { _gfx = NULL; }
	void initButton(ILI9341_t3n *gfx, int16_t x, int16_t y,
		uint8_t w, uint8_t h,
		uint16_t outline, uint16_t fill, uint16_t textcolor,
		const char *label, uint8_t textsize);
	void drawButton(bool inverted = false);
	bool contains(int16_t x, int16_t y);
	void press(boolean p) {
		laststate = currstate;
		currstate = p;
	}
	bool isPressed() { return currstate; }
	bool justPressed() { return (currstate && !laststate); }
	bool justReleased() { return (!currstate && laststate); }
private:
	ILI9341_t3n *_gfx;
	int16_t _x, _y;
	uint16_t _w, _h;
	uint8_t _textsize;
	uint16_t _outlinecolor, _fillcolor, _textcolor;
	char _label[10];
	boolean currstate, laststate;
};
#endif

#endif // __cplusplus


#endif
