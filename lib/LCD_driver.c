//********************************************************************
//
//				LCD_driver.c: Interface for Nokia LCD
//
//********************************************************************
#include "LCD_driver.h"
#include "LPC214x.h"
#include "stdlib.h"
#include "../MP3Dev.h"
#include "rprintf.h"

//********************************************************************
//
//				"Private" Function Definitions
//
//********************************************************************
static void LCDSetPixel(unsigned char color, unsigned char x, unsigned char y);
static unsigned char LCDPrintChar(char txt, char text_color, char *x_pos, char *y_pos, char inverted);

//********************************************************************
//
//				Global Variables for LCD driver
//
//********************************************************************
static char x_offset = 0;
static char y_offset = 0;

#ifdef	EPSON
	char white = 0xFF;//white teXt
	char red = 0xC0;
	char black = 0x00;//black background
#endif
#ifdef	PHILLIPS
	char white = 0x00;
	char red = 0x38;
	char black = 0xFF;
#endif

//This text array defines the pixel values for characters to be sent
//to the LCD.  The array is local to the LCD driver and may not be
//used outside of this file.
static char text_array[475] = {0x00,0x00,0x00,0x00,0x00,/*space*/
                              0x00,0xF6,0xF6,0x00,0x00,/*!*/
                              0x00,0xE0,0x00,0xE0,0x00,/*"*/
                              0x28,0xFE,0x28,0xFE,0x28,/*#*/
                              0x00,0x64,0xD6,0x54,0x08,/*$*/
                              0xC2,0xCC,0x10,0x26,0xC6,/*%*/
                              0x4C,0xB2,0x92,0x6C,0x0A,/*&*/
                              0x00,0x00,0xE0,0x00,0x00,/*'*/
                              0x00,0x38,0x44,0x82,0x00,/*(*/
                              0x00,0x82,0x44,0x38,0x00,/*)*/
                              0x88,0x50,0xF8,0x50,0x88,/***/
                              0x08,0x08,0x3E,0x08,0x08,/*+*/
                              0x00,0x00,0x05,0x06,0x00,/*,*/
                              0x08,0x08,0x08,0x08,0x08,/*-*/
                              0x00,0x00,0x06,0x06,0x00,/*.*/
                              0x02,0x0C,0x10,0x60,0x80,/*/*/
                              0x7C,0x8A,0x92,0xA2,0x7C,/*0*/
                              0x00,0x42,0xFE,0x02,0x00,/*1*/
                              0x42,0x86,0x8A,0x92,0x62,/*2*/
                              0x44,0x82,0x92,0x92,0x6C,/*3*/
                              0x10,0x30,0x50,0xFE,0x10,/*4*/
                              0xE4,0xA2,0xA2,0xA2,0x9C,/*5*/
                              0x3C,0x52,0x92,0x92,0x0C,/*6*/
                              0x80,0x86,0x98,0xE0,0x80,/*7*/
                              0x6C,0x92,0x92,0x92,0x6C,/*8*/
                              0x60,0x92,0x92,0x94,0x78,/*9*/
                              0x00,0x00,0x36,0x36,0x00,/*:*/
                              0x00,0x00,0x35,0x36,0x00,/*;*/
                              0x10,0x28,0x44,0x82,0x00,/*<*/
                              0x28,0x28,0x28,0x28,0x28,/*=*/
                              0x00,0x82,0x44,0x28,0x10,/*>*/
                              0x40,0x80,0x8A,0x90,0x60,/*?*/
                              0x7C,0x82,0xBA,0xBA,0x62,/*@*/
                              0x3E,0x48,0x88,0x48,0x3E,/*A*/
                              0xFE,0x92,0x92,0x92,0x6C,/*B*/
                              0x7C,0x82,0x82,0x82,0x44,/*C*/
                              0xFE,0x82,0x82,0x82,0x7C,/*D*/
                              0xFE,0x92,0x92,0x92,0x82,/*E*/
                              0xFE,0x90,0x90,0x90,0x80,/*F*/
                              0x7C,0x82,0x82,0x8A,0x4E,/*G*/
                              0xFE,0x10,0x10,0x10,0xFE,/*H*/
                              0x82,0x82,0xFE,0x82,0x82,/*I*/
                              0x84,0x82,0xFC,0x80,0x80,/*J*/
                              0xFE,0x10,0x28,0x44,0x82,/*K*/
                              0xFE,0x02,0x02,0x02,0x02,/*L*/
                              0xFE,0x40,0x20,0x40,0xFE,/*M*/
                              0xFE,0x60,0x10,0x0C,0xFE,/*N*/
                              0x7C,0x82,0x82,0x82,0x7C,/*O*/
                              0xFE,0x90,0x90,0x90,0x60,/*P*/
                              0x7C,0x82,0x82,0x86,0x7E,/*Q*/
                              0xFE,0x90,0x98,0x94,0x62,/*R*/
                              0x64,0x92,0x92,0x92,0x4C,/*S*/
                              0x80,0x80,0xFE,0x80,0x80,/*T*/
                              0xFC,0x02,0x02,0x02,0xFC,/*U*/
                              0xF8,0x04,0x02,0x04,0xF8,/*V*/
                              0xFC,0x02,0x0C,0x02,0xFC,/*W*/
                              0xC6,0x28,0x10,0x28,0xC6,/*X*/
                              0xC0,0x20,0x1E,0x20,0xC0,/*Y*/
                              0x86,0x8A,0x92,0xA2,0xC2,/*Z*/
                              0x00,0x00,0xFE,0x82,0x00,/*[*/
                              0x00,0x00,0x00,0x00,0x00,/*this should be / */
                              0x80,0x60,0x10,0x0C,0x02,/*]*/
                              0x20,0x40,0x80,0x40,0x20,/*^*/
                              0x01,0x01,0x01,0x01,0x01,/*_*/
                              0x80,0x40,0x20,0x00,0x00,/*`*/
                              0x04,0x2A,0x2A,0x2A,0x1E,/*a*/
                              0xFE,0x12,0x22,0x22,0x1C,/*b*/
                              0x1C,0x22,0x22,0x22,0x14,/*c*/
                              0x1C,0x22,0x22,0x12,0xFE,/*d*/
                              0x1C,0x2A,0x2A,0x2A,0x18,/*e*/
                              0x10,0x7E,0x90,0x80,0x40,/*f*/
                              0x18,0x25,0x25,0x25,0x1E,/*g*/
                              0xFE,0x10,0x10,0x10,0x0E,/*h*/
                              0x00,0x12,0x5E,0x02,0x00,/*i*/
                              0x02,0x01,0x01,0x11,0x5E,/*j*/
                              0xFE,0x08,0x08,0x14,0x22,/*k*/
                              0x00,0x82,0xFE,0x02,0x00,/*l*/
                              0x3E,0x20,0x1C,0x20,0x1E,/*m*/
                              0x3E,0x20,0x20,0x20,0x1E,/*n*/
                              0x1C,0x22,0x22,0x22,0x1C,/*o*/
                              0x3F,0x24,0x24,0x24,0x18,/*p*/
                              0x18,0x24,0x24,0x3F,0x01,/*q*/
                              0x3E,0x10,0x20,0x20,0x10,/*r*/
                              0x12,0x2A,0x2A,0x2A,0x04,/*s*/
                              0x00,0x10,0x3C,0x12,0x04,/*t*/
                              0x3C,0x02,0x02,0x02,0x3E,/*u*/
                              0x30,0x0C,0x02,0x0C,0x30,/*v*/
                              0x38,0x06,0x18,0x06,0x38,/*w*/
                              0x22,0x14,0x08,0x14,0x22,/*x*/
                              0x38,0x05,0x05,0x05,0x3E,/*y*/
                              0x22,0x26,0x2A,0x32,0x22,/*z*/
                              0x00,0x10,0x6C,0x82,0x82,/*{*/
                              //0x00,0x00,0xFF,0x00,0x00,/*|*/
                              0x04,0x02,0xFF,0x02,0x04,/*|, arrow*/
                              0x82,0x82,0x6C,0x10,0x00,/*}*/
                              0x08,0x10,0x18,0x08,0x10};/*~*/

//The spark logo array defines the SparkFun logo that is displayed during startup on the splash screen.  The array may
//be changed to display a different image at startup or removed altogether.  The array is local to the LCD driver and
//can not be used outside of this file.
static char logo_spark[1120] =	{
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x78,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xe0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xe0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xfb,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7f,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0c,0x3f,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1c,0x3f,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3c,0x7f,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3f,0xff,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3f,0xff,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3f,0xff,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3f,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3f,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3f,0xfc,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3f,0xe0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3e,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x38,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x0f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x0e,0x20,0x1f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x1e,0x00,0x3f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x1e,0x00,0x3c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x1e,0x00,0x3c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x0f,0xe0,0x9f,0x01,0xfc,0x09,0x9e,0x1e,0x7f,0x70,0x73,0x9f,0x00,0x00,0x00,0x00,
0x3f,0xf1,0xff,0x87,0xfe,0x3f,0xde,0x3d,0xff,0x78,0xf3,0xff,0x80,0x00,0x00,0x00,
0x3c,0xf9,0xff,0xc7,0xdf,0x3f,0xde,0x79,0xff,0x78,0xf3,0xff,0xc0,0x00,0x00,0x00,
0x78,0x79,0xc3,0xcf,0x0f,0x3f,0x1c,0xf0,0x3c,0x78,0xf3,0xe3,0xc0,0x00,0x00,0x00,
0x7c,0x01,0xc1,0xe0,0x0f,0x3e,0x1f,0xe0,0x3c,0x78,0xf3,0xc3,0xc0,0x00,0x00,0x00,
0x3f,0xc1,0x81,0xe0,0x3f,0x3c,0x1f,0xe0,0x3c,0x78,0xf3,0xc1,0xc0,0x00,0x00,0x00,
0x1f,0xf1,0x81,0xe3,0xff,0x3c,0x1f,0xe0,0x3c,0x78,0xf3,0xc1,0xc0,0x00,0x00,0x00,
0x07,0xf9,0x81,0xe7,0xef,0x3c,0x1f,0xf0,0x3c,0x78,0xf3,0xc1,0xc0,0x00,0x00,0x00,
0x00,0xf9,0x81,0xef,0x07,0x3c,0x1e,0xf8,0x3c,0x78,0xf3,0xc1,0xc0,0x00,0x00,0x00,
0x78,0x79,0xc1,0xef,0x0f,0x3c,0x1e,0x78,0x3c,0x78,0xf3,0xc1,0xc0,0x00,0x00,0x00,
0x78,0x79,0xe3,0xcf,0x0f,0x3c,0x1e,0x3c,0x3c,0x7c,0xf3,0xc1,0xc0,0x00,0x00,0x00,
0x3f,0xf9,0xff,0xcf,0xff,0x3c,0x1e,0x3e,0x3c,0x7f,0xf3,0xc1,0xcf,0x00,0x00,0x00,
0x1f,0xf1,0xff,0x87,0xff,0x3c,0x1e,0x1e,0x3c,0x3f,0xf3,0xc1,0xc7,0x00,0x00,0x00,
0x07,0xc1,0x9e,0x03,0xe0,0x00,0x00,0x02,0x00,0x0e,0x20,0x00,0x00,0x00,0x00,0x00,
0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x03,0x80,0x00,0x00,0x00,0xc0,0x00,0x00,0x18,0x00,0x00,0x08,0x08,0x00,0x00,
0x00,0x01,0x87,0xc3,0x03,0xe0,0xe1,0xf0,0xf8,0x3e,0x33,0x08,0x3e,0x1e,0x00,0x00,
0x00,0x01,0x86,0x03,0x03,0x01,0xb0,0xe0,0xdc,0x66,0x3b,0x08,0x66,0x32,0x00,0x00,
0x00,0x00,0x87,0xc3,0x03,0xe1,0x80,0x40,0xd8,0x63,0x3b,0x08,0x60,0x3c,0x00,0x00,
0x00,0x00,0x87,0x83,0x03,0xc1,0x80,0x40,0xf8,0x63,0x3f,0x08,0x60,0x0e,0x00,0x00,
0x00,0x00,0x06,0x03,0x03,0x01,0xb0,0x40,0xd8,0x66,0x37,0x08,0x66,0x32,0x00,0x00,
0x00,0x00,0x07,0xc3,0xe3,0xe0,0xe0,0x40,0xc8,0x3e,0x33,0x08,0x3e,0x3e,0x00,0x00,
0x00,0x00,0x07,0xc3,0xe3,0xe0,0xe0,0x40,0x88,0x3c,0x33,0x08,0x3c,0x1e,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,};


//Usage: LCDClear(black);
//Inputs: char color: 8-bit color to be sent to the screen.
//Outputs: None
//Description: This function will clear the screen with "color" by writing the
//			   color to each location in the RAM of the LCD.
void LCDClear(char color)
{
	#ifdef EPSON
		LCDCommand(PASET);
		LCDData(0);
		LCDData(131);
	
		LCDCommand(CASET);
		LCDData(0);
		LCDData(131);

		LCDCommand(RAMWR);
	#endif
	#ifdef	PHILLIPS
		LCDCommand(PASETP);
		LCDData(0);
		LCDData(131);
	
		LCDCommand(CASETP);
		LCDData(0);
		LCDData(131);

		LCDCommand(RAMWRP);
	#endif
	
	for(int i=0; i < 30000; i++)
	{
		LCDData(color);
	}
	
	x_offset = 0;
	y_offset = 0;
}

//Usage: LCDCommand(RAMWR);
//Inputs: char data - character command to be sent to the LCD
//Outputs: None
//Description: Sends a 9 bit command over SPI to the LCD with
//			   the first bit HIGH indicating "command" mode.
void LCDCommand(unsigned char data)
{

	IODIR0 |= (LCD_DIO | LCD_SCK | LCD_CS | LCD_RES);		//Assign LCD pins as Outputs
	
	IOCLR0 = LCD_CS;      // enable chip, p0.20 goes low
    IOCLR0 = LCD_DIO;     // output low on data out (9th bit low = command), p0.19

    IOCLR0 = LCD_SCK;      // send clock pulse
    IOSET0 = LCD_SCK;

    for (char j = 0; j < 8; j++)
    {
        if ((data & 0x80) == 0x80) IOSET0 = LCD_DIO;
        else IOCLR0 = LCD_DIO;

        IOCLR0 = LCD_SCK;   // send clock pulse
        IOSET0 = LCD_SCK;

        data <<= 1;
    }

    IOSET0 = LCD_CS;    		// disable

}

//Usage: LCDData(RAMWR);
//Inputs: char data - character data to be sent to the LCD
//Outputs: None
//Description: Sends a 9 bit command over SPI to the LCD with
//			   the first bit LOW indicating "data" mode.
void LCDData(unsigned char data)
{

	IODIR0 |= (LCD_DIO | LCD_SCK | LCD_CS | LCD_RES);		//Assign LCD pins as Outputs
	
	IOCLR0 = LCD_SCK;      // send clock pulse
	IOCLR0 = LCD_CS;      // enable chip, p0.20 goes low    
	//#ifdef	PHILLIPS
		//for (char i = 0; i < 1; i++) asm volatile ("nop");
	//#endif
	IOSET0 = LCD_DIO;    // output high on data out (9th bit high = data), p0.19
    IOCLR0 = LCD_SCK;    // send clock pulse
	//#ifdef	PHILLIPS
		//for (char i = 0; i < 1; i++) asm volatile ("nop");
	//#endif
    IOSET0 = LCD_SCK;
	for (char j = 0; j < 8; j++)
    {
        if ((data & 0x80) == 0x80) IOSET0 = LCD_DIO;
        else IOCLR0 = LCD_DIO;
        IOCLR0 = LCD_SCK;   // send clock pulse
		//#ifdef	PHILLIPS
			//for (char i = 0; i < delay; i++) asm volatile ("nop");
		//#endif
        IOSET0 = LCD_SCK;
		//#ifdef	PHILLIPS
			//for (char i = 0; i < delay; i++) asm volatile ("nop");
		//#endif
        data <<= 1;
    }

    IOSET0 = LCD_CS;     		// disable
}

//Usage: LCDInit();
//Inputs: None
//Outputs: None
//Description:  Initializes the LCD regardless of if the controlller is an EPSON or PHILLIPS.
void LCDInit(void)
{
    int j;

	delay_ms(200);
	
    // reset display
	IODIR0 |= (LCD_DIO | LCD_SCK | LCD_CS | LCD_RES);		//Assign LCD pins as Outputs
		
    IOCLR0 = (LCD_SCK | LCD_DIO);							//output_low (SPI_CLK);//output_low (SPI_DO);
    IOSET0 = LCD_CS;				//output_high (LCD_CS);
    for (j = 0; j < 16; j++)asm volatile ("nop");
    IOCLR0 = LCD_RES;				//output_low (LCD_RESET);
    for (j = 0; j < 300000; j++)asm volatile ("nop");
    IOSET0 = LCD_RES;				//output_high (LCD_RESET);
    IOSET0 = (LCD_SCK | LCD_DIO);
    for (j = 0; j < 300000; j++)asm volatile ("nop");	//delay_ms(100);
	
    LCDCommand(DISCTL);  	// display control(EPSON)
    LCDData(0x0C);   		// 12 = 1100 - CL dividing ratio [don't divide] switching period 8H (default)
	LCDData(0x20);    
	LCDData(0x02);
	
	LCDData(0x01);
	
    LCDCommand(COMSCN);  	// common scanning direction(EPSON)
    LCDData(0x01);
    
    LCDCommand(OSCON);  	// internal oscialltor ON(EPSON)
	
    LCDCommand(SLPOUT);  	// sleep out(EPSON)
	LCDCommand(SLEEPOUT);	//sleep out(PHILLIPS)
    
    LCDCommand(PWRCTR); 	// power ctrl(EPSON)
    LCDData(0x0F);    		//everything on, no external reference resistors
    LCDCommand(BSTRON);		//Booset On(PHILLIPS)
	
	LCDCommand(DISINV);  	// invert display mode(EPSON)
	LCDCommand(INVON);		// invert display mode(PHILLIPS)
    
    LCDCommand(DATCTL);  	// data control(EPSON)
    LCDData(0x03);			//correct for normal sin7
	LCDData(0x00);   		// normal RGB arrangement
	//LCDData(0x01);		// 8-bit Grayscale
	LCDData(0x02);			// 16-bit Grayscale Type A
	
	LCDCommand(MADCTL);		//Memory Access Control(PHILLIPS)
	LCDData(0xC8);
	
	LCDCommand(COLMOD);		//Set Color Mode(PHILLIPS)
	LCDData(0x02);	
	
    LCDCommand(VOLCTR);  	// electronic volume, this is the contrast/brightness(EPSON)
    //LCDData(0x18);   		// volume (contrast) setting - fine tuning, original
	LCDData(0x24);   		// volume (contrast) setting - fine tuning, original
    LCDData(0x03);   		// internal resistor ratio - coarse adjustment
	LCDCommand(SETCON);		//Set Contrast(PHILLIPS)
	LCDData(0x30);	
	
    /*
    LCDCommand(RGBSET8);   // setup 8-bit color lookup table  [RRRGGGBB](EPSON)
    //RED
    LCDData(0);
    LCDData(2);
    LCDData(4);
    LCDData(6);
    LCDData(8);
    LCDData(10);
    LCDData(12);
    LCDData(15);
    // GREEN
    LCDData(0);
    LCDData(2);
    LCDData(4);
    LCDData(6);
    LCDData(8);
    LCDData(10);
    LCDData(12);
    LCDData(15);
    //BLUE
    LCDData(0);
    LCDData(4);
    LCDData(9);
    LCDData(15);
	*/
    
    LCDCommand(NOP);  	// nop(EPSON)
	LCDCommand(NOPP);		// nop(PHILLIPS)

    LCDCommand(DISON);   	// display on(EPSON)
	LCDCommand(DISPON);	// display on(PHILLIPS)
}

//Usage: LCDPrintChar('1', white,&x_offset, &y_offset, inverted)
//Inputs: char txt - character to be printed
//		  char text_color - desired color
//	 	  char *x_pos - pointer to the row position of the character
//		  char *y_pos - pointer to the column position of the character
//		  char inverted - indicates if the character should be positioned normally, or oriented to the right side of the screen
//Outputs: char *x_pos - Now holds the next available row position for a character on the screen
//		   char *y_pos - Now holds the next available column position for a character on the screen
//Description: Prints a character in the desired color at the given x and y position.  If the character is to be inverted,
//			   then the right side of the LCD becomes the "top" so the text will be viewed at a different orientation.
//**NOTE** Because this function is static, it is essentially a "private" function
//         and can only be used within this file!
static unsigned char LCDPrintChar(char txt, char text_color, char *x_pos, char *y_pos, char inverted)
{
    short text_array_offset = (txt - 32)*5, j;
    char k, temp;
	
    
    if (txt == 10)
    {
        y_pos += 8;
        return(0);
    }

    else if (txt == 13)
    {
        x_pos = 0;
        return(0);
    }

	#ifdef PHILLIPS
    for (j = text_array_offset; j < text_array_offset+5; j++)
    {
        temp = text_array[j];

        for (k = 0; k < 8; k++)
        {
			if(inverted==0){
				if ((temp & 0x80) == 0x80) LCDSetPixel(text_color, *y_pos + k, *x_pos + j + 2 - text_array_offset);
				temp <<= 1;
			}
			else if(inverted==1){

				if ((temp & 0x01) == 0x01) LCDSetPixel(text_color, *y_pos + j + 2 - text_array_offset, *x_pos + k);

				temp >>= 1;	
			}
        }
    }
	#endif
	#ifdef EPSON
    for (j = text_array_offset+4; j >= text_array_offset; j--)
    {
        temp = text_array[j];

        for (k = 8; k > 0; k--)
        {
			if(inverted==0){
				if ((temp & 0x80) == 0x80) LCDSetPixel(text_color, *y_pos + k, *x_pos - j - 2 + text_array_offset);
				temp <<= 1;
			}
			else if(inverted==1){

				if ((temp & 0x01) == 0x01) LCDSetPixel(text_color, *y_pos - j - 2 + text_array_offset, *x_pos + k);

				temp >>= 1;	
			}
        }
    }
	#endif
	#ifdef PHILLIPS
	if(inverted==0){

		if ((*x_pos + 12) > 132) *x_pos = 0, *y_pos += 8;
		else *x_pos += 6;

	}
	else if(inverted==1){

		if ((*y_pos + 12) > 132) *y_pos = 0, *x_pos += 8;
		else *y_pos += 6;	

	}

    if ((*y_pos + 8) > 132)
    {
        //clear_screen();
        return 1;
    }
	#endif
	#ifdef EPSON
	if(inverted==0){

		if ((*x_pos - 12) > 132) *x_pos = 0, *y_pos -= 8;
		else *x_pos -= 6;

	}
	else if(inverted==1){

		if ((*y_pos - 12) > 132) *y_pos = 0, *x_pos -= 8;
		else *y_pos -= 6;	

	}

    if ((*y_pos + 8) > 132)
    {
        //clear_screen();
        return 1;
    }
	#endif
    else return 0;

}

//Usage: LCDPrintLogo();
//Inputs: None
//Outputs: None
//Description: Prints the logo_spark array to the LCD.
void LCDPrintLogo(void)
{
    int x = 4, y = 25, logo_ix = 0, z;
    char logo;
    
    for (logo_ix = 0; logo_ix < 1120; logo_ix++)
    {
        logo = logo_spark[logo_ix];
		for (z = 0; z < 8; z++)
        {
			#ifdef PHILLIPS
            if ((logo & 0x80) == 0x80) LCDSetPixel(red, y, x);
			#endif
			#ifdef EPSON
            if ((logo & 0x80) == 0x80) LCDSetPixel(red, 132-y, 132-x);			
			#endif
            x++;
            if (x == 132)
            {
                x = 4;
                y++;
            }
            
            logo <<= 1;
        }
    }

}

//Usage: LCDPrintString("Hello %d", 5, black, 0, 0, 1);
//Inputs: const char *txt - string of characters to be printed
//		  int variable - if a variable is to be printed in the string it should be placed here, else put a 0
//		  text_color - desired color of the string
//		  char row - desired row for the string
//		  char column - desired column for the string
//		  char inverterted - indication of desired orientation
//Outputs: None
//Description: Prints a string, including variables, in the desired color at the given row and column.  If inverterted,
//			   the right side of the LCD will become the "top."
void LCDPrintString(const char *txt, int variable, char text_color, char row, char column, char inverted)
{
#ifdef PHILLIPS
	if(inverted==0){
		x_offset=column*6;
		y_offset=row*8;
	}
	else if(inverted==1){
		x_offset=(15-row)*8+3;
		y_offset=(column*6);
	}
#endif
#ifdef EPSON
	if(inverted==0){
		x_offset=column*6+21*6;
		y_offset=15*8-row*8;
	}
	else if(inverted==1){
		x_offset=row*8+3;
		y_offset=column*6+21*6;
	}
#endif
    int	  temp;
    short i, k, l, m;
    short temp2;
    
    short decimal_output[9];
    
    for(i = 0 ; i < 22; i++)		//Only print the length of the screen
    {
        //delay_ms(3);
        
        k = txt[i];

        if (k == '\0') break;
        
        else if (k == '%') //Print var
        {
            i++;
            k = txt[i];

            if (k == '\0') break;
        	
            else if (k == '\\') //Print special characters
            {
                i++;
                k = txt[i];
                
                LCDPrintChar(k, white, &x_offset, &y_offset, inverted);
                

            } //End Special Characters
            else if (k == 'b') //Print Binary
            {
                for( m = 0 ; m < 32 ; m++ )
                {
                    if ((variable & 0x80000000) == 0x80000000) LCDPrintChar('1', white,&x_offset, &y_offset, inverted);
                    if ((variable & 0x80000000) == 0) LCDPrintChar('0', white,&x_offset, &y_offset, inverted);
                    if (((m+1) % 4) == 0) LCDPrintChar(' ', white,&x_offset, &y_offset, inverted);
                    
                    variable = variable << 1;
                }
            } //End Binary               
            else if (k == 'd') //Print Decimal
            {
                //Print negative sign and take 2's compliment
                
                if(variable < 0)
                {
                    LCDPrintChar('-', text_color,&x_offset, &y_offset, inverted);
                    variable *= -1;
                }
                
                
                if (variable == 0)
                    LCDPrintChar('0', text_color,&x_offset, &y_offset, inverted);
                else
                {
                    //Divide number by a series of 10s
                    for(m = 9 ; variable > 0 ; m--)
                    {
                        temp = variable % 10;
                        decimal_output[m] = temp+48;
                        variable = variable / 10;               
                    }
                
                    for(m++ ; m < 10 ; m++)
                    {
                        LCDPrintChar(decimal_output[m], text_color,&x_offset, &y_offset, inverted);
                    }
                }
    
            } //End Decimal
            else if (k == 'h') //Print Hex
            {
                //New trick 3-15-04
                LCDPrintChar('0', white,&x_offset, &y_offset, inverted);
                LCDPrintChar('x', white,&x_offset, &y_offset, inverted);
                
                for (m = 0; m < 8; m++)
                {
                	temp = variable & 0xF0000000;
                	temp2 = temp >>= 28;
                	//LCDPrintChar(bin2Hex(temp2));
                	variable <<= 4;
                }
            } //End Hex
            else if (k == 'f') //Print Float
            {
                LCDPrintChar('!', white,&x_offset, &y_offset, inverted);
            } //End Float
            else if (k == 'u') //Print Direct Character
            {
                //All ascii characters below 20 are special and screwy characters
                //if(my_byte > 20) 
                    LCDPrintChar(variable, text_color,&x_offset, &y_offset, inverted);
            } //End Direct
                        
        } //End Special Chars           
        
        else if (k == '/')
        {
            l = txt[i+1];
            if (l == 'r') LCDPrintChar(13, white,&x_offset, &y_offset, inverted);
            else if (l == 'n') LCDPrintChar(10, white,&x_offset, &y_offset, inverted);
            i += 1;
        }

        else LCDPrintChar(k, text_color,&x_offset, &y_offset, inverted);
    }    
}

//Usage: LCDSetPixel(white, 0, 0);
//Inputs: unsigned char color - desired color of the pixel
//		  unsigned char x - Page address of pixel to be colored
//		  unsigned char y - column address of pixel to be colored
//Outputs: None
//Description: Sets the starting page(row) and column (x & y) coordinates in ram,
//  		   then writes the colour to display memory.  The ending x & y are left
//  		   maxed out so one can continue sending colour data bytes to the 'open'
//  		   RAMWR command to fill further memory.  issuing any red command
//  		   finishes RAMWR.
//**NOTE** Because this function is static, it is essentially a "private" function
//         and can only be used within this file!
static void LCDSetPixel(unsigned char color, unsigned char x, unsigned char y)
{
	unsigned int temp_register;
	
	temp_register = VICIntEnable;
	VICIntEnClr = 0x20;
	#ifdef EPSON
		LCDCommand(PASET);   // page start/end ram
		LCDData(x);
		LCDData(ENDPAGE);
  
		LCDCommand(CASET);   // column start/end ram
		LCDData(y);
		LCDData(ENDCOL);
  
		LCDCommand(RAMWR);    // write
		LCDData(color);
		LCDData(NOP);
		LCDData(NOP);
	#endif
	#ifdef	PHILLIPS
		LCDCommand(PASETP);   // page start/end ram
		LCDData(x);
		LCDData(ENDPAGE);
  
		LCDCommand(CASETP);   // column start/end ram
		LCDData(y);
		LCDData(ENDCOL);
  
		LCDCommand(RAMWRP);    // write
		LCDData(color);
	#endif
	VICIntEnable = temp_register;
}

//Usage: LCDSetRowColor(0,0,white, 0);
//Inputs: unsigned char row - row to be set
//		  unsigned char column - this indicates where to start coloring the row
//		  char color - desired color to change the row to
//		  char inverted - indicates if row goes from left to right, or top to bottom
//Outputs: None
//Description:  Changes the color or a row starting at the position indicated in column.
void LCDSetRowColor(unsigned char row, unsigned char column, char color, char inverted){
#ifdef PHILLIPS
	if(inverted==ORIENTUP){
		row *= 8;
		column *=6;
	}
	else if(inverted==ORIENTLEFT){
		row=(15-row)*8+3;
		column = (column*6);
	}

	for (int j = column; j < ROW_LENGTH; j++)
	{
		for (int k = 0; k < 8; k++)
		{
			if(inverted==ORIENTUP){
				LCDSetPixel(color, row + k, j);
			}
			else if(inverted==ORIENTLEFT){
				LCDSetPixel(color, j, row+k);
			}
		}
	}
#endif
#ifdef EPSON
	if(inverted==ORIENTUP){
		row = 15*8-row*8;
		column *=6;
	}
	else if(inverted==ORIENTLEFT){
		row=row*8+3;
		column = column*6;
	}

	for (int j = column; j < ROW_LENGTH; j++)
	{
		for (int k = 0; k < 8; k++)
		{
			if(inverted==ORIENTUP){
				LCDSetPixel(color, row + k, j);
			}
			else if(inverted==ORIENTLEFT){
				LCDSetPixel(color, j, row+k);
			}
		}
	}
#endif
}

//Usage: LCDContrast(0x03);
//Inputs: char setting - character representing desired contrast
//Outputs: None
//Description: Sets the LCD contrast to "setting"
void LCDContrast(char setting){
	#ifdef	EPSON
		LCDCommand(VOLCTR);	// electronic volume, this is the contrast/brightness
		LCDData(0x18);		// volume (contrast) setting - fine tuning, original
		LCDData(setting);	// internal resistor ratio - coarse adjustment
	#endif
	#ifdef	PHILLIPS
		LCDCommand(SETCON);	//Sets contrast for phillips screen
		LCDCommand(setting);
	#endif
}

