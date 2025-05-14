#include "UART4_US100.h"
#include "TM4C123GH6PM.h"

void UART4_Init(void)
{
    SYSCTL->RCGCUART |= 0x10;        // Enable UART4 clock
    SYSCTL->RCGCGPIO |= 0x04;        // Enable Port C clock

    GPIOC->AFSEL |= 0x30;            // Enable alternate functions on PC4, PC5
    GPIOC->PCTL &= ~0x00FF0000;      
    GPIOC->PCTL |=  0x00110000;      // PC4 = U4RX, PC5 = U4TX
    GPIOC->DEN  |=  0x30;            // Digital enable PC4, PC5

    UART4->CTL   &= ~0x01;           // Disable UART4
    UART4->IBRD   = 0x0145;          // IBRD = 325 (for 9600 baud @ 50MHz)
    UART4->FBRD   = 0x0021;          // FBRD = 33
    UART4->LCRH   = 0x60;            // 8-bit, no parity, 1 stop bit
    UART4->CTL    = 0x301;           // Enable RXE, TXE, and UART
}

float US100_ReadDistanceCM(void)
{
    uint8_t high, low;
    uint16_t distance_mm;

    while (UART4->FR & 0x20);        // Wait if TXFF full
    UART4->DR = 0x55;                // Trigger measurement

    while (UART4->FR & 0x10);        // Wait for RX data
    high = UART4->DR;

    while (UART4->FR & 0x10);        // Wait for RX data
    low = UART4->DR;

    distance_mm = (high << 8) | low;
    return distance_mm / 10.0f;
}






















