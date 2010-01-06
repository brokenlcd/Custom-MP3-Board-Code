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


// time to start from scratch

/*
first function referenced in the now broken build is delay_ms
so the first function I will define is delay_ms.
the clock is at 12 MHz, multiplied by 5 to get 60 MHz for the
microprocessor speed. This means an instruction may be executed
every 1x1e-8 seconds. To get a millisecond delay, we multiply by
1000. So we get an instruction every 1x1e-6 milliseconds. This
means that to delay for a millisecond, we need to execute 1000000
nops in that time.
*/

/*
 * function: void delay_ms(int count)
 *      input: the number of milliseconds to delay for
 *      output: no output but the system will pause for n milliseconds
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

/****************************************
 *          INTERNAL VARS               *
 ****************************************/

/****************************************
 *          EXTERNAL VARS               *
 ****************************************/

