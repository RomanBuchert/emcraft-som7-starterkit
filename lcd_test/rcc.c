/*
 * rcc.c
 *
 *  Created on: 03.12.2021
 *      Author: RBuchert
 */

#include <rcc.h>
#include <libopencm3/stm32/rcc.h>

uint32_t sai_p_freq;
uint32_t sai_q_freq;
uint32_t sai_ltdc_freq;

void rcc_sai_on() {
	RCC_CR |= RCC_CR_PLLSAION;
	while (!(RCC_CR & RCC_CR_PLLSAIRDY));
}

void rcc_sai_off() {
	RCC_CR &= ~RCC_CR_PLLSAION;
	while ((RCC_CR & RCC_CR_PLLSAIRDY));
}

void rcc_set_saip_pll(struct rcc_sai_pll* pll_config) {
	uint32_t rcc_pllsaicfgr = 0;
	uint32_t rcc_dckcfgr1 = RCC_DCKCFGR1;
	uint32_t pll_freq;

	pll_config->plln &= RCC_PLLSAICFGR_PLLSAIN_MASK;
	pll_config->pllp &= RCC_PLLSAICFGR_PLLSAIP_MASK;
	pll_config->pllq &= RCC_PLLSAICFGR_PLLSAIQ_MASK;
	pll_config->pllr &= RCC_PLLSAICFGR_PLLSAIR_MASK;

	rcc_pllsaicfgr |= pll_config->plln << RCC_PLLSAICFGR_PLLSAIN_SHIFT;
	rcc_pllsaicfgr |= pll_config->pllp << RCC_PLLSAICFGR_PLLSAIP_SHIFT;
	rcc_pllsaicfgr |= pll_config->pllq << RCC_PLLSAICFGR_PLLSAIQ_SHIFT;
	rcc_pllsaicfgr |= pll_config->pllr << RCC_PLLSAICFGR_PLLSAIR_SHIFT;

	pll_config->plldivq &= RCC_DCKCFGR1_PLLSAIDIVQ_MASK;
	pll_config->plldivr &= RCC_DCKCFGR1_PLLSAIDIVR_MASK;

	rcc_dckcfgr1 &= ~(RCC_DCKCFGR1_PLLSAIDIVQ_MASK << RCC_DCKCFGR1_PLLSAIDIVQ_SHIFT);
	rcc_dckcfgr1 &= ~(RCC_DCKCFGR1_PLLSAIDIVR_MASK << RCC_DCKCFGR1_PLLSAIDIVR_SHIFT);
	rcc_dckcfgr1 |= pll_config->plldivq << RCC_DCKCFGR1_PLLSAIDIVQ_SHIFT;
	rcc_dckcfgr1 |= pll_config->plldivr << RCC_DCKCFGR1_PLLSAIDIVR_SHIFT;

	rcc_sai_off();
	RCC_PLLSAICFGR = rcc_pllsaicfgr;
	RCC_DCKCFGR1 = rcc_dckcfgr1;
	rcc_sai_on();
	pll_freq = pll_config->vco_mhz * pll_config->plln;
	sai_p_freq = pll_freq / pll_config->pllp;
	sai_q_freq = (pll_freq / pll_config->pllq) >> (pll_config->plldivq);
	sai_ltdc_freq = (pll_freq / pll_config->pllr) >> (1 + pll_config->plldivr);
}
