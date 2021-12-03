/*
 * buttons.h
 *
 *  Created on: 03.12.2021
 *      Author: RBuchert
 */

#ifndef LCD_TEST_BUTTONS_H_
#define LCD_TEST_BUTTONS_H_

#define USR_BTN1	1
#define USR_BTN2	2

#include <stdbool.h>
#include <stdint.h>

void button_init();
bool button_get(uint8_t button_id);

#endif /* LCD_TEST_BUTTONS_H_ */
