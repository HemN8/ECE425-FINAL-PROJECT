/**
 * @file    SysTick_Delay.c
 * @brief   Millisecond delay implementation using SysTick timer on TM4C123GH6PM
 * @author  Hem Nagoo
  */

#include "TM4C123GH6PM.h"
#include "SysTick_Delay.h"

void SysTick_Init(void)
{
    SysTick->LOAD = 50000 - 1;   // 1 ms delay (50 MHz system clock)
    SysTick->VAL = 0;
    SysTick->CTRL = 0x05;        // Enable SysTick, no interrupt, use system clock
}

void Delay_ms(uint32_t ms)
{
    for (uint32_t i = 0; i < ms; i++)
    {
        while ((SysTick->CTRL & 0x10000) == 0); // Wait for COUNT flag
    }
}
