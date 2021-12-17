/*
 * systimer.h
 *
 *  Created on: 07.12.2021
 *      Author: RBuchert
 */

#ifndef LCD_TEST_SYSTIMER_H_
#define LCD_TEST_SYSTIMER_H_

#include <stdint.h>

void systimer_init();
void systimer_delay_ms(uint32_t ms);
uint32_t systimer_get_ms();
int systimer_add_timeout(uint32_t* var, uint32_t timeout);
int systimer_remove_timeout(uint32_t* var);
#endif /* LCD_TEST_SYSTIMER_H_ */
