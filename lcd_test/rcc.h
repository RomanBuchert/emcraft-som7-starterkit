/*
 * rcc.h
 *
 *  Created on: 03.12.2021
 *      Author: RBuchert
 */

#ifndef LCD_TEST_RCC_H_
#define LCD_TEST_RCC_H_

#include <stdint.h>

struct rcc_sai_pll{
	uint32_t vco_mhz;
	uint16_t plln;
	uint8_t pllp;
	uint8_t pllq;
	uint8_t pllr;
	uint8_t plldivq;
	uint8_t plldivr;
};

extern uint32_t sai_p_freq;
extern uint32_t sai_q_freq;
extern uint32_t sai_ltdc_freq;

void rcc_sai_on();
void rcc_sai_off();
void rcc_set_saip_pll(struct rcc_sai_pll* pll_config);

#endif /* LCD_TEST_RCC_H_ */
