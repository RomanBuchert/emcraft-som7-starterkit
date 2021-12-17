/*
 * sdram.c
 *
 *  Created on: 02.12.2021
 *      Author: RBuchert
 */
#ifndef STM32F7
#define STM32F7
#endif

#include <stdint.h>
#include "sdram.h"
#include <libopencm3/cm3/mpu.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/fsmc.h>

#define NS2CLK(ns) (_ns2clk(ns))
#define max(X, Y) ( (X) > (Y) ? (X) : (Y) )

/**
  * @brief  FMC SDRAM Mode definition register defines
  */
#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200)

#define SDRAM_TRRD	NS2CLK(12)
#define SDRAM_TRCD	NS2CLK(18)
#define SDRAM_TRP	NS2CLK(18)
#define SDRAM_TRAS	NS2CLK(42)
#define SDRAM_TRC	NS2CLK(60)
#define SDRAM_TRFC	NS2CLK(60)
#define SDRAM_TCDL	(1 - 1)
#define SDRAM_TRDL	NS2CLK(12)
#define SDRAM_TBDL	(1 - 1)
#define SDRAM_TREF	(NS2CLK(64000000 / 8192) - 20)
#define SDRAM_TCCD	(1 - 1)

#define SDRAM_TXSR	SDRAM_TRFC	/* Row cycle time after precharge */
#define SDRAM_TMRD	(3 - 1)	/* Page 10, Mode Register Set */

/* Last data in to row precharge, need also comply ineq on page 1648 */
#define SDRAM_TWR	(max(						\
	(int)max((int)SDRAM_TRDL, (int)(SDRAM_TRAS - SDRAM_TRCD)), \
	(int)(SDRAM_TRC - SDRAM_TRCD - SDRAM_TRP)	\
	))

static void sdram_config_gpio(uint32_t gpio, uint16_t gpios);
static void sdram_init_gpio();
static void sdram_init_sdram();
static uint32_t _ns2clk(uint32_t ns);

void sdram_init() {
	sdram_init_gpio();
	rcc_periph_clock_enable(RCC_FMC);
	sdram_init_sdram();
}

static void sdram_config_gpio(uint32_t gpio, uint16_t gpios) {
	gpio_mode_setup(gpio, GPIO_MODE_AF, GPIO_PUPD_NONE, gpios);
	gpio_set_output_options(gpio, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, gpios);
	gpio_set_af(gpio, GPIO_AF12, gpios);
}

static void sdram_init_gpio() {
	rcc_periph_clock_enable(RCC_GPIOC);
	rcc_periph_clock_enable(RCC_GPIOD);
	rcc_periph_clock_enable(RCC_GPIOE);
	rcc_periph_clock_enable(RCC_GPIOF);
	rcc_periph_clock_enable(RCC_GPIOG);
	rcc_periph_clock_enable(RCC_GPIOH);

	// Port C
	sdram_config_gpio(GPIOC,
			GPIO2 | GPIO3
	);
	// Port D
	sdram_config_gpio(GPIOD,
		    GPIO0 | GPIO1 | GPIO4 | GPIO5 | GPIO8 |
		    GPIO9 | GPIO10 | GPIO11 | GPIO12 | GPIO13 |
		    GPIO14 | GPIO15
	);
	// Port E
	sdram_config_gpio(GPIOE,
		    GPIO0 | GPIO1 | GPIO3 | GPIO4 | GPIO5 |
		    GPIO6 | GPIO7 | GPIO8 | GPIO9 | GPIO10 |
		    GPIO11 | GPIO12 | GPIO13 | GPIO14 | GPIO15
	);
	// Port F
	sdram_config_gpio(GPIOF,
		    GPIO0 | GPIO1 | GPIO2 | GPIO3 | GPIO4 |
		    GPIO5 | GPIO11 | GPIO12 | GPIO13 | GPIO14 |
		    GPIO15
	);
	// Port G
	sdram_config_gpio(GPIOG,
		    GPIO0 | GPIO1 | GPIO2 | GPIO4 | GPIO5 |
		    GPIO8 | GPIO15
	);
	// Port H
	sdram_config_gpio(GPIOH,
			GPIO5
	);

}

static void sdram_init_sdram() {
	uint32_t timing;
	uint32_t tmpreg;

	struct sdram_timing t = {
		.trcd =	1 + SDRAM_TRCD,		/* RCD Delay */
		.trp =	1 + SDRAM_TRP,		/* RP Delay */
		.twr = 	1 + SDRAM_TWR,		/* Write Recovery Time */
		.trc = 	1 + SDRAM_TRC,		/* Row Cycle Delay */
		.tras = 1 + SDRAM_TRAS,		/* Self Refresh TIme */
		.txsr = 1 + SDRAM_TXSR,		/* Exit Self Refresh Time */
		.tmrd = 1 + SDRAM_TMRD,		/* Load to Active delay */
	};

	// Configure SDCR - register
	tmpreg =  FMC_SDCR_RPIPE_NONE;
	tmpreg |= FMC_SDCR_SDCLK_2HCLK;
	tmpreg |= FMC_SDCR_CAS_3CYC;
	tmpreg |= FMC_SDCR_NB4;
	tmpreg |= FMC_SDCR_MWID_16b;
	tmpreg |= FMC_SDCR_NR_13;
	tmpreg |= FMC_SDCR_NC_10;
	FMC_SDCR1 = tmpreg;

	// Configure the SDTR1 timing register
	timing = sdram_timing(&t);
	FMC_SDTR1 = timing;

	// Configure the controller
	sdram_command(SDRAM_BANK1, SDRAM_CLK_CONF, 1, 0);
	for(volatile int i = 2000; i; i--) {
		;
	}
	sdram_command(SDRAM_BANK1, SDRAM_PALL, 1, 0);
	sdram_command(SDRAM_BANK1, SDRAM_AUTO_REFRESH, 8, 0);
	tmpreg = 	SDRAM_MODE_BURST_LENGTH_1			|
				SDRAM_MODE_BURST_TYPE_SEQUENTIAL	|
				SDRAM_MODE_CAS_LATENCY_3			|
				SDRAM_MODE_OPERATING_MODE_STANDARD	|
				SDRAM_MODE_WRITEBURST_MODE_SINGLE;
	sdram_command(SDRAM_BANK1, SDRAM_LOAD_MODE, 1, tmpreg);
	sdram_command(SDRAM_BANK1, SDRAM_NORMAL, 1, 0);
	FMC_SDRTR = (SDRAM_TREF / 2);
}

static uint32_t _ns2clk(uint32_t ns) {
	uint32_t freq = rcc_ahb_frequency / 2;
	uint32_t tmp = freq / 1000000;
	return (tmp * ns) / 1000;
}
