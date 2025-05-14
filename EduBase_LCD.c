#include "EduBase_LCD.h"
#include "TM4C123GH6PM.h"
#include "SysTick_Delay.h"
#include <stdio.h>

#define RS  PE0   // PE0 for Register Select
#define E   PC6   // PC6 for Enable
#define D4  PA2
#define D5  PA3
#define D6  PA4
#define D7  PA5

void LCD_Command(uint8_t cmd);
void LCD_Data(uint8_t data);
void LCD_Pulse_Enable(void);
void LCD_Write_4Bits(uint8_t nibble);

void EduBase_LCD_Init(void)
{
    SYSCTL->RCGCGPIO |= 0x15;          // Enable clock to Port A (0x01), C (0x04), E (0x10)

    GPIOA->DIR |=  0x3C;               // PA2-5 outputs
    GPIOA->DEN |=  0x3C;

    GPIOC->DIR |=  0x40;               // PC6 output
    GPIOC->DEN |=  0x40;

    GPIOE->DIR |=  0x01;               // PE0 output
    GPIOE->DEN |=  0x01;

    Delay_ms(50);

    LCD_Command(0x33);
    LCD_Command(0x32);
    LCD_Command(0x28); // 4-bit, 2-line, 5x8 dots
    LCD_Command(0x0C); // Display ON, Cursor OFF
    LCD_Command(0x06); // Entry mode
    LCD_Command(0x01); // Clear
    Delay_ms(2);
}

void EduBase_LCD_Clear_Display(void)
{
    LCD_Command(0x01);
    Delay_ms(2);
}

void EduBase_LCD_Set_Cursor(uint8_t row, uint8_t col)
{
    uint8_t address = (row == 0) ? (0x80 + col) : (0xC0 + col);
    LCD_Command(address);
}

void EduBase_LCD_Display_String(const char *str)
{
    while (*str)
        LCD_Data(*str++);
}

void EduBase_LCD_Display_Distance(uint32_t distance)
{
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%u cm", distance);
    EduBase_LCD_Display_String(buffer);
}

// Low-level helpers
void LCD_Command(uint8_t cmd)
{
    GPIOE->DATA &= ~0x01;         // RS = 0
    LCD_Write_4Bits(cmd >> 4);
    LCD_Write_4Bits(cmd & 0x0F);
    Delay_ms(2);
}

void LCD_Data(uint8_t data)
{
    GPIOE->DATA |= 0x01;          // RS = 1
    LCD_Write_4Bits(data >> 4);
    LCD_Write_4Bits(data & 0x0F);
    Delay_ms(2);
}

void LCD_Write_4Bits(uint8_t nibble)
{
    GPIOA->DATA &= ~0x3C;
    GPIOA->DATA |= (nibble << 2) & 0x3C;
    LCD_Pulse_Enable();
}

void LCD_Pulse_Enable(void)
{
    GPIOC->DATA &= ~0x40;
    Delay_ms(1);
    GPIOC->DATA |=  0x40;
    Delay_ms(1);
    GPIOC->DATA &= ~0x40;
    Delay_ms(1);
}
