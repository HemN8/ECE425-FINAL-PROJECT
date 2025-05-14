#ifndef SERVO_H
#define SERVO_H

#include <stdint.h>

void Servo_Init(void);
void Servo_SetPulseWidthTicks(uint32_t pulse_ticks);
uint32_t AngleToPulseTicks(uint32_t angle);  // Add this if tracking with angles

#endif








