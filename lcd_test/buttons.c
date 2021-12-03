/*
 * buttons.c
 *
 *  Created on: 03.12.2021
 *      Author: RBuchert
 */

#include <buttons.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

void button_init() {
	rcc_periph_clock_enable(RCC_GPIOA);
	gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO0);
	gpio_mode_setup(GPIOI, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO9);
}

bool button_get(uint8_t button_id) {
	bool retval = false;
	switch (button_id) {
	case USR_BTN1:
		retval = gpio_get(GPIOA, GPIO0);
		break;
	case USR_BTN2:
		retval = gpio_get(GPIOI, GPIO9);
		break;
	}
	return retval;
}
