#ifndef EDUBASE_LCD_H
#define EDUBASE_LCD_H

#include <stdint.h>

void EduBase_LCD_Init(void);
void EduBase_LCD_Clear_Display(void);
void EduBase_LCD_Set_Cursor(uint8_t row, uint8_t col);
void EduBase_LCD_Display_String(const char *str);
void EduBase_LCD_Display_Distance(uint32_t distance);

#endif // EDUBASE_LCD_H
