#ifndef UART0_H
#define UART0_H

#include <stdint.h>

void UART0_Init(void);
void UART0_Output_Char(char data);
void UART0_Output_String(const char *str);
void UART0_Output_Unsigned_Decimal(uint32_t number);
void UART0_Output_Unsigned_Hexadecimal(uint32_t number);
void UART0_Output_Newline(void);

#endif
