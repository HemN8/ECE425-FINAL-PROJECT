#ifndef SYSTICK_DELAY_H
#define SYSTICK_DELAY_H

#include <stdint.h>

/**
 * @brief Initializes the SysTick timer to generate 1ms delay ticks.
 */
void SysTick_Init(void);

/**
 * @brief Delays the processor for a specified number of milliseconds.
 * @param ms Number of milliseconds to delay
 */
void Delay_ms(uint32_t ms);

#endif








