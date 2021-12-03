/*
 * lcd.h
 *
 *  Created on: 02.12.2021
 *      Author: RBuchert
 */

#ifndef LCD_TEST_LCD_H_
#define LCD_TEST_LCD_H_

struct lcd_tft_info {
	uint32_t pixelclock_hz;
	uint32_t hori_disp_area;
	uint32_t hori_sync;
	uint32_t hori_back_porch;
	uint32_t hori_front_porch;
	uint32_t vert_disp_area;
	uint32_t vert_sync;
	uint32_t vert_back_porch;
	uint32_t vert_front_porch;
};

void lcd_init();
void lcd_set_backlight(uint8_t value);

#endif /* LCD_TEST_LCD_H_ */
