#include "LED_8SEG.h"

UBYTE SEG8Code[] ={
        0x3F, // 0
        0x06, // 1
        0x5B, // 2
        0x4F, // 3
        0x66, // 4
        0x6D, // 5
        0x7D, // 6
        0x07, // 7
        0x7F, // 8
        0x6F, // 9
        0x77, // A
        0x7C, // b
        0x39, // C
        0x5E, // d
        0x79, // E
        0x71, // F
    };

/******************************************************************************
function:	Send Command
parameter: 
    Num: bit select
    Seg：segment select
Info:The data transfer
******************************************************************************/

void LED_8SEG_SendCommand(UBYTE Num, UBYTE Seg) 
{
    DEV_Digital_Write(LED_8SEG_RCLK_PIN, 1);
    DEV_SPI_WriteByte(Num);
    DEV_SPI_WriteByte(Seg);
    DEV_Digital_Write(LED_8SEG_RCLK_PIN, 0);
    DEV_Delay_ms(2);
    DEV_Digital_Write(LED_8SEG_RCLK_PIN, 1);
}

/******************************************************************************
function:	Stop watch
parameter: 
Info:99 seconds watch
******************************************************************************/

void LED_8SEG_stopwatch()
{
    for (int o = 0; o < 99999; o++)
    {
        DEV_Delay_us(500);
        LED_8SEG_SendCommand(UNITS, SEG8Code[o%10]);
        DEV_Delay_us(500);
        LED_8SEG_SendCommand(TENS, SEG8Code[(o%100)/10]);
        DEV_Delay_us(500);
        LED_8SEG_SendCommand(HUNDREDS, SEG8Code[(o%1000)/100] | Dot);
        DEV_Delay_us(500);
        LED_8SEG_SendCommand(KILOBIT, SEG8Code[(o%10000)/1000]);
    }
}
