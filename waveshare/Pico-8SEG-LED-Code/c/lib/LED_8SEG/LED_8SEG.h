#ifndef __LED_8SEG_H
#define __LED_8SEG_H

#include "DEV_Config.h"
#include <stdlib.h>

#define KILOBIT   0XFE //KiloBit
#define HUNDREDS  0XFD //hundreds
#define TENS      0XFB //tens
#define UNITS     0XF7 //units
#define Dot       0x80 //decimal separator


void LED_8SEG_SendCommand(UBYTE Reg,UBYTE Seg);
void LED_8SEG_stopwatch();


#endif
