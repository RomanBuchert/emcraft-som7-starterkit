
#ifndef STM32F7
#define STM32F7
#endif
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/fsmc.h>
#include <libopencm3/stm32/gpio.h>

#include <buttons.h>
#include <sdram.h>
#include <lcd.h>
void SetPixel(size_t x, size_t y, uint32_t c);

void main() {
	bool DisplayOn = true;
	rcc_clock_setup_hse(&rcc_3v3[RCC_CLOCK_3V3_216MHZ], 12);
	button_init();
	sdram_init();
#if 0
	for(uint32_t* addr = (uint32_t*)0xC0000000; addr < (uint32_t*)0xC4000000; addr++) {
		*addr = 0;
	}
#endif
	for (uint32_t* addr = (uint32_t*)0xC0000000; addr < (uint32_t*)(0xC0000000 + (480*272*4)); addr++) {
		*addr = 0xFF000000;
	}
	lcd_init();

	uint32_t* addr = (uint32_t*) 0xC0000000;
	uint32_t clut[8] = {
			0xFF0000FF,
			0xFF00FF00,
			0xFFFF0000,
			0xFF00FFFF,
			0xFFFF00FF,
			0xFFFFFF00,
			0xFF000000,
			0xFFFFFFFF
	};

	for (size_t x = 0; x < 480; x++) {
		SetPixel(x,0,0xFF0000 | (uint32_t)(255UL*x/480UL)<<24);
		SetPixel(x,1,0xFF0000 | (uint32_t)(255UL*x/480UL)<<24);
		SetPixel(x,2,0xFF0000 | (uint32_t)(255UL*x/480UL)<<24);
		SetPixel(x,271,0xFFFFFF00);
		SetPixel(x,270,0xFFFFFF00);
	}
	for (size_t y = 0; y < 272; y++) {
		SetPixel(0,y, 0xFF00FF00);
		SetPixel(1,y, 0xFF00FF00);
		SetPixel(478,y,0xFFFF00FF);
		SetPixel(479,y,0xFFFF00FF);
	}
	for (size_t i = 0; i < 1000; i++) {
		SetPixel(480*i/1000, 272*i/1000, 0xFF0000FF);
		SetPixel(480*i/1000, 272-(272*i/1000), 0xFF00FFFF);
	}
	for (;;) {
		if (button_get(USR_BTN1)) {
			DisplayOn ^= true;
			while(button_get(USR_BTN1)) {
				;
			}
			lcd_set_backlight(DisplayOn);
		}
	}
}

void SetPixel(size_t x, size_t y, uint32_t c) {
	uint32_t* addr = (uint32_t*)(0xC0000000+(4*(y*480+x)));
	*addr = c;
}
