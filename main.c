/**
 * @file    main.c
 * @brief   Object Locking Radar System using TM4C123GH6PM, UART4-US100, Servo, and LCD
 * @author  Hem
 */

#include "TM4C123GH6PM.h"
#include "SysTick_Delay.h"
#include "UART4_US100.h"
#include "EduBase_LCD.h"
#include "servo.h"

// Constants for servo sweep range (1ms to 2ms PWM at 50Hz)
#define MIN_PULSE_WIDTH_TICKS    0x061B  // 1563 ticks (~1ms)
#define MAX_PULSE_WIDTH_TICKS    0x1A85  // 7813 ticks (~2ms)

// Object detection thresholds in cm
#define LOCK_DISTANCE_CM         30
#define UNLOCK_DISTANCE_CM       35

// Timing intervals in ms
#define SENSOR_UPDATE_INTERVAL   50
#define SERVO_UPDATE_INTERVAL    5

// Step size in ticks for sweeping motion
#define SWEEP_STEP_TICKS         0x00C8  // 200 ticks

volatile uint32_t milliseconds = 0;

// SysTick interrupt handler: increment time
void SysTick_Handler(void)
{
    milliseconds++;
}

// Enable PWM clock for servo control
void PWM_Clock_Init(void)
{
    SYSCTL->RCC |= (1U << 20);         // Use PWM divider
    SYSCTL->RCC &= ~0x000E0000;        // Clear PWM divisor bits
    SYSCTL->RCC |= (0x3U << 17);       // Set divisor to /8 (0b011 => divide by 8)
}

// Initialize RED LED (PF1)
void LED_Init(void)
{
    SYSCTL->RCGCGPIO |= 0x20;          // Enable clock for Port F (bit 5)
    while ((SYSCTL->PRGPIO & 0x20) == 0); // Wait until Port F is ready

    GPIOF->DIR  |= 0x02;               // Set PF1 (bit 1) as output
    GPIOF->DEN  |= 0x02;               // Digital enable PF1
    GPIOF->DATA &= ~0x02;              // Turn off RED LED initially
}

int main(void)
{
    SysTick_Init();                    // Configure SysTick 1ms timer
    SysTick->LOAD = 0xC34F;            // 50000-1 = 0xC34F, for 1ms interrupt (50 MHz)
    SysTick->CTRL = 0x07;              // Enable SysTick with interrupt

    PWM_Clock_Init();
    Servo_Init();
    UART4_Init();
    EduBase_LCD_Init();
    LED_Init();

    uint32_t pulse_width = MIN_PULSE_WIDTH_TICKS;
    uint8_t direction = 1;             // 1 = increasing pulse, 0 = decreasing
    uint8_t locked = 0;                // 0 = not locked, 1 = target locked
    float distance = 0;

    uint32_t last_servo_update = 0;
    uint32_t last_sensor_update = 0;

    while (1)
    {
        uint32_t now = milliseconds;

        // Servo sweep if not locked onto a target
        if (!locked && (now - last_servo_update >= SERVO_UPDATE_INTERVAL))
        {
            Servo_SetPulseWidthTicks(pulse_width);  // Move servo to current angle

            if (direction)
            {
                pulse_width += SWEEP_STEP_TICKS;
                if (pulse_width >= MAX_PULSE_WIDTH_TICKS)
                {
                    pulse_width = MAX_PULSE_WIDTH_TICKS;
                    direction = 0;
                }
            }
            else
            {
                pulse_width -= SWEEP_STEP_TICKS;
                if (pulse_width <= MIN_PULSE_WIDTH_TICKS)
                {
                    pulse_width = MIN_PULSE_WIDTH_TICKS;
                    direction = 1;
                }
            }

            last_servo_update = now;
        }

        // Read distance from US-100 and update display
        if ((now - last_sensor_update) >= SENSOR_UPDATE_INTERVAL)
        {
            distance = US100_ReadDistanceCM();
            last_sensor_update = now;

            EduBase_LCD_Clear_Display();

            // Lock condition: distance within LOCK threshold
            if (!locked && distance > 0 && distance < LOCK_DISTANCE_CM)
            {
                locked = 1;
                GPIOF->DATA |= 0x02;   // RED LED ON (PF1)

                EduBase_LCD_Set_Cursor(0, 0);
                EduBase_LCD_Display_String("Target locked");

                EduBase_LCD_Set_Cursor(1, 0);
                EduBase_LCD_Display_String("Distance: ");
                EduBase_LCD_Display_Distance((uint32_t)distance);

                Servo_SetPulseWidthTicks(pulse_width); // Hold servo at current angle
            }
            // Unlock condition: distance exceeds threshold
            else if (locked && distance >= UNLOCK_DISTANCE_CM)
            {
                locked = 0;
                GPIOF->DATA &= ~0x02;  // RED LED OFF (PF1)

                EduBase_LCD_Set_Cursor(0, 0);
                EduBase_LCD_Display_String("Finding enemy");
            }
            // Remain locked: keep servo position
            else if (locked)
            {
                EduBase_LCD_Set_Cursor(0, 0);
                EduBase_LCD_Display_String("Target locked");

                EduBase_LCD_Set_Cursor(1, 0);
                EduBase_LCD_Display_String("Distance: ");
                EduBase_LCD_Display_Distance((uint32_t)distance);

                Servo_SetPulseWidthTicks(pulse_width); // Maintain lock
            }
            // No target detected
            else
            {
                EduBase_LCD_Set_Cursor(0, 0);
                EduBase_LCD_Display_String("Finding enemy");
            }
        }
    }
}



