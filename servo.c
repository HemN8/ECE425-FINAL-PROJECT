#include "servo.h"
#include "TM4C123GH6PM.h"

#define PWM_PERIOD_TICKS         0xF424   // 62500: 20ms period @ 3.125 MHz (50Hz)
#define MIN_PULSE_WIDTH_TICKS    0x061B   // 1563: ~0.5ms (0°)
#define MAX_PULSE_WIDTH_TICKS    0x1E85   // 7813: ~2.5ms (180°)

void Servo_Init(void)
{
    SYSCTL->RCGCPWM |= 0x01;              // Enable clock to PWM0
    SYSCTL->RCGCGPIO |= 0x02;             // Enable clock to Port B

    while ((SYSCTL->PRPWM & 0x01) == 0);  // Wait for PWM0 ready

    GPIOB->AFSEL |= 0x80;                 // Enable alternate function on PB7
    GPIOB->PCTL &= ~0xF0000000;
    GPIOB->PCTL |=  0x40000000;           // Configure PB7 as M0PWM1
    GPIOB->DEN  |= 0x80;                  // Digital enable PB7
    GPIOB->DIR  |= 0x80;                  // Set PB7 as output

    SYSCTL->RCC |= 0x00100000;            // Use PWM divider
    SYSCTL->RCC &= ~0x000E0000;           // Clear divider bits
    SYSCTL->RCC |=  0x00060000;           // Set divider to /16 ? 3.125 MHz

    PWM0->_0_CTL   = 0x00;                // Disable PWM generator 0
    PWM0->_0_GENB  = 0x0000080C;          // Set PB7 high on LOAD, low on CMPB
    PWM0->_0_LOAD  = PWM_PERIOD_TICKS - 1;
    PWM0->_0_CMPB  = PWM_PERIOD_TICKS - MIN_PULSE_WIDTH_TICKS;
    PWM0->_0_CTL   |= 0x01;               // Enable generator
    PWM0->ENABLE   |= 0x02;               // Enable M0PWM1 on PB7
}

void Servo_SetPulseWidthTicks(uint32_t pulse_ticks)
{
    PWM0->_0_CMPB = PWM_PERIOD_TICKS - pulse_ticks;
}

uint32_t AngleToPulseTicks(uint32_t angle)
{
    if (angle > 180) angle = 180;
    return MIN_PULSE_WIDTH_TICKS +
        ((angle * (MAX_PULSE_WIDTH_TICKS - MIN_PULSE_WIDTH_TICKS)) / 180);
}
