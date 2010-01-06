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
    
    bootARM();           // ARM boot sequence
    vs1002Config();                 //Configure MP3 I/O
    vs1002Reset();                  //Reset MP3 Player
    vs1002Init();

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
 *      the value of 1000000 is derived from the clock speed of the ARM:
 *      60 MHz = 1 instruction / 1e-8 seconds; so 1e6 nops will take one
 *      millisecond to execute.
 */

void delay_ms(int count) {
    int i = 0;
    count *= 1000000;
    for (i; i < count; i++) { asm("nop"); }
    return;
}


// a quick reset function

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
    delay_ms(30);       // wait for power to stabilize


    // set up MP3 decoder
    
}