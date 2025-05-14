#include "UART0.h"
#include "TM4C123GH6PM.h"

#define UART0_IBRD 27
#define UART0_FBRD 8

void UART0_Init(void)
{
    SYSCTL->RCGCUART |= 0x01;
    SYSCTL->RCGCGPIO |= 0x01;

    UART0->CTL &= ~0x01;
    UART0->IBRD = UART0_IBRD;
    UART0->FBRD = UART0_FBRD;
    UART0->LCRH = 0x70; // 8-bit, no parity, 1 stop, FIFO
    UART0->CTL = 0x301;

    GPIOA->AFSEL |= 0x03;
    GPIOA->PCTL &= ~0xFF;
    GPIOA->PCTL |= 0x11;
    GPIOA->DEN |= 0x03;
}

void UART0_Output_Char(char data)
{
    while (UART0->FR & 0x20);
    UART0->DR = data;
}

void UART0_Output_String(const char *str)
{
    while (*str) UART0_Output_Char(*str++);
}

void UART0_Output_Unsigned_Decimal(uint32_t number)
{
    if (number >= 10)
        UART0_Output_Unsigned_Decimal(number / 10);
    UART0_Output_Char((number % 10) + '0');
}

void UART0_Output_Unsigned_Hexadecimal(uint32_t number)
{
    if (number >= 0x10)
        UART0_Output_Unsigned_Hexadecimal(number / 0x10);

    uint8_t digit = number % 0x10;
    if (digit < 10)
        UART0_Output_Char(digit + '0');
    else
        UART0_Output_Char((digit - 10) + 'A');
}

void UART0_Output_Newline(void)
{
    UART0_Output_Char('\r');
    UART0_Output_Char('\n');
}
