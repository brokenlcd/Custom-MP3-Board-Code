//*******************************************************
//					MP3 Development Platform
//*******************************************************
//#include <stdio.h>
#include "../lib/LPC214x.h"
#include "setup.h"
#include "MP3Dev.h"
#include "../lib/serial.h"
#include "../lib/rprintf.h"

//*******************************************************
//				Memory Management Libraries
//*******************************************************
#include "../lib/rootdir.h"
#include "../lib/sd_raw.h"
#include "../lib/fat16.h"

//*******************************************************
//				USB Libraries
//*******************************************************
#include "../Bootloader/LPCUSB/main_msc.h"

//*******************************************************
//					External Component Libs
//*******************************************************
#include "../lib/LCD_driver.h"
#include "../lib/MMA7260.h"
#include "../lib/vs1002.h"
#include "../lib/NS73.h"

//*******************************************************
//					Interrupt Functions
//*******************************************************
static void timer0ISR(void);
static void timer1ISR(void); 



/****************************************
 *          INTERNAL VARS               *
 ****************************************/
char green = 0x1C;          // 8 bit color

/****************************************
 *          EXTERNAL VARS               *
 ****************************************/
extern char black;          // from LCD_driver


int main(void) {
    /*****************
     * boot sequence *
     *****************/
    
    bootARM();           	// ARM boot sequence
    vs1002Mute();           // mute volume during start up

    // start of code
    ledRedOn();         // red LED indicates powering up
    LCDInit();          // initialize LCD
    LCDClear(black);   

    return 0;
}


// time to start from scratch

/*
 * function: void delay_ms(int count)
 *      input: the number of milliseconds to delay for
 *      output: no output but the system will pause for n milliseconds
 *
 *      the value of count is derived from the clock speed of the ARM:
 *      60 MHz = 1 instruction / 1.7e-8 seconds; so 1e6 nops will take one
 *      millisecond to execute.
 */

void delay_ms(int count) {
    int i = 0;
    count *= 1.7E6;
    for (i; i < count; i++) { asm("nop"); }
    return;
}



// reset by triggering a fault in the watchdog
void reset(void) {
    // by triggering a fault in the watchdog, the MCU will be reset
    WDMOD |= 3;
    WDFEED = 0xAA;
    WDFEED = 0x55;
    WDFEED = 0xAA;
    WDFEED = 0x00;
}


// intialization functions
void bootARM(void) {
    // had to steal some code from the original SparkFun code because I don't
    // understand it yet.
    rprintf_devopen(putc_serial0);  // Init rprintf
    delay_ms(30);                   // wait for power to stabilize
    
    // bring up SD / FAT *
    if (! (sd_raw_init()) ) { rprintf("SD init error!\n"); }
    if ( openroot() ) { rprintf("openroot error!\n"); }
    
    // set up I/O pins *
    
	//Setup the MP3 I/O Lines
	IODIR0 |= MP3_XCS;
	IODIR0 &= ~MP3_DREQ;
	PINSEL1 |= 0x00000C00;	        // Set the MP3_DREQ Pin to be a capture pin
	IODIR1 |= MP3_XDCS | MP3_GPIO0 | MP3_XRES;	
	
	//Setupt the FM Trans. Lines
	IODIR1 |= FM_LA; 				// FM trans outputs (Leave SPI pins 
	IODIR1 |= FM_CS;                //    unconfigured for now)
	
	//Setup the SD Card I/O Lines
	IODIR0 |= SD_CS;				// SD card outputs
	
	//Setup the Accelerometer I/O Lines
	IODIR0 |= (GS1 | GS2);			// accelerometer outputs
	
	// ensure ADC pins have ADC Functions selected
	PINSEL0 |= (MMA_X_PINSEL | MMA_Y_PINSEL | MMA_Z_PINSEL);		
	IOCLR0 = (GS1 | GS2);		    // initialize acceleration to 1.5 G mode
	
	//Setup the LCD I/O Lines
	IODIR0 |= (LCD_RES | LCD_CS);   // LCD Outputs
	
	//Setup the LED Lines										
	IODIR0 |= (LED_BLU | LED_RED | LED_GRN);
	ledBlueOff();
	ledRedOff();
	ledGrnOff();
	
	//Setup the Buttons
	IODIR1 &= (~SW_UP & ~SW_DWN & ~SW_MID);		//Button Inputs

	IODIR0 &= ~(1<<23);							//Set the Vbus line as an input

    //Setupt the Interrupts
	VPBDIV = 1;									// Set PCLK equal to the system clock	
	VICIntSelect = ~0x30; 						// Timer 0 AND TIMER 1 interrupt is an IRQ interrupt
    VICIntEnable = 0x10; 						// Enable Timer 0 interrupts 
                                                // (don't start sending song data with Timer 1)
    VICVectCntl0= 0x25; 						// Use slot 0 for timer 1 interrupt
    VICVectAddr0 = (unsigned int)timer1ISR; 	// Set the address of ISR for slot 1		
    VICVectCntl1 = 0x24; 						// Use slot 1 for timer 0 interrupt
    VICVectAddr1 = (unsigned int)timer0ISR; 	// Set the address of ISR for slot 1
	
	//Configure Timer0
	T0PR = 1500;								// divide clock(60MHz) by 1500 for 40kHz PS
	T0TCR |=0X01;							    // enable the clock
	T0CTCR=0;									// timer node
	T0MCR=0x0003;								// interrupt and reset timer on match
	T0MR0=1000;									// interrupt on 40Hz
	
	//Configure Timer1
	T1PR = 200;									// divide clock by 300 for 40kHz PS
	T1TCR |=0X01;								// enable the clock
	T1CTCR=0;									// timer mode
	T1CCR=0x0A00;								// capture and interrupt on the 
	                                            //    rising edge of DREQ
	
	//Setup the SPI Port
    S0SPCCR = 64;              					// SCK = 1 MHz, counter > 8 and even
    S0SPCR  = 0x20;                				// Master, no interrupt enable, 8 bits	
    
    
    
    // set up MP3 decoder
    vs1002Config();	// set up ARM <-> VS1002 communication
    vs1002Reset();	// hardware reset
    vs1002Init();	// initialization
    
    return;
    
}

void timer0ISR(void) { return; }
void timer1ISR(void) { return; } 
