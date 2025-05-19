#ifndef LCD1602_H
#define LCD1602_H

void lcd_init();
void lcd_clear();
void lcd_set_cursor(int col, int row);
void lcd_print(const char *str);

#endif

