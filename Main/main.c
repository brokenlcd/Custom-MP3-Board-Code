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


// and now I start from scratch.