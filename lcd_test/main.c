
#ifndef STM32F7
#define STM32F7
#endif
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/fsmc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/vector.h>
#include <i2c.h>
#include <buttons.h>
#include <sdram.h>
#include <gt911.h>
#include <lcd.h>
#include <systimer.h>
#include <uart.h>

static const void* heap_end = (void*)0xC4000000;
static void* heap_start = NULL;
static void* prev_heap_end = NULL;
static void* curr_heap_end = NULL;

void* _sbrk(int incr) {
	if (heap_start == NULL) {
		heap_start = (void*)(heap_end - (1024*1024));
	}
	if (curr_heap_end == NULL) {
		curr_heap_end = heap_start;
	}
	prev_heap_end = curr_heap_end;
	if ((curr_heap_end + incr) < heap_end) {
		curr_heap_end += incr;
	}
	else {
		return (void*)-1;
	}
	return (void*)prev_heap_end;
}

int _write(int fd, char* ptr, int len) {
	return uart_write(fd, ptr, len);
}

int _read(int fd, char* ptr, int len) {
	return uart_read(fd, ptr, len);
}

void SetPixel(size_t x, size_t y, uint32_t c);

void int2led(void) {
	if (gpio_get(GPIOI, GPIO10)) {
		gpio_set(GPIOD, GPIO3);
	}
	else {
		gpio_clear(GPIOD, GPIO3);
	}
}

void main() {
	int number;
	bool DisplayOn = true;
	rcc_clock_setup_hse(&rcc_3v3[RCC_CLOCK_3V3_216MHZ], 12);
	rcc_periph_clock_enable(RCC_GPIOD);
	gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO3);
	gpio_set_output_options(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, GPIO3);
	gpio_clear(GPIOD, GPIO3);
	sdram_init();
	systimer_init();
	uart_init(&uart_usart1_8n1_115200);
	i2c_init_regs();
	button_init();
	/*
	while(!button_get(USR_BTN1)) {
		;
	}
	 */
	lcd_init();
	lcd_set_backlight(0);
	gt911_init();
#if 0
	uint8_t data;
	gt911_read(0x804d, &data, 1);
	data &= ~0x08;
	gt911_write(0x804d, &data, 1);
	gt911_read(0x804d, &data, 1);
	gt911_update_config();
#endif
	gt911_debug();
#if 0
	for(uint32_t* addr = (uint32_t*)0xC0000000; addr < (uint32_t*)0xC4000000; addr++) {
		*addr = 0;
	}
	for (uint32_t* addr = (uint32_t*)0xC0000000; addr < (uint32_t*)(0xC0000000 + (480*272*4)); addr++) {
		*addr = 0xFF000000;
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
#endif
	char tmp = 0;

	uint16_t gt911_info_data[22];
	volatile int retval;
	uint64_t cntr = 0;
	retval = gt911_write(0x8040, &tmp, 1);
	if (retval != 0) {
		gt911_init();
	}

	for (;;) {
		while (gpio_get(GPIOI, GPIO10) == 0) {
			;
		}
		cntr++;
		retval = gt911_read(0x814E, gt911_info_data, 42);
		if (retval != 0) {
			gt911_init();
		}
		else {
			if (gt911_info_data[0] & 0x80) {
				retval = gt911_write(0x814E, &tmp, 0);
			}
			if (retval != 0) {
				gt911_init();
			}
		}
		while(gpio_get(GPIOI, GPIO10));
		systimer_delay_ms(100);
	}
}

void SetPixel(size_t x, size_t y, uint32_t c) {
	uint32_t* addr = (uint32_t*)(0xC0000000+(4*(y*480+x)));
	*addr = c;
}
