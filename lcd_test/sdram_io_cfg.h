/*
 * sdram_io_cfg.h
 *
 *  Created on: 02.12.2021
 *      Author: RBuchert
 */

#ifndef LCD_TEST_SDRAM_IO_CFG_H_
#define LCD_TEST_SDRAM_IO_CFG_H_

#include <libopencm3/stm32/gpio.h>

/**
 * Pin definitions for SDRAM
 */
#define SDRAM_PIN_D0
#define SDRAM_PIN_D1
#define SDRAM_PIN_D2
#define SDRAM_PIN_D3
#define SDRAM_PIN_D4
#define SDRAM_PIN_D5
#define SDRAM_PIN_D6
#define SDRAM_PIN_D7
#define SDRAM_PIN_D8
#define SDRAM_PIN_D9
#define SDRAM_PIN_D10
#define SDRAM_PIN_D11
#define SDRAM_PIN_D12
#define SDRAM_PIN_D13
#define SDRAM_PIN_D14
#define SDRAM_PIN_D15
#define SDRAM_PIN_D16
#define SDRAM_PIN_D17
#define SDRAM_PIN_D18
#define SDRAM_PIN_D19
#define SDRAM_PIN_D20
#define SDRAM_PIN_D21
#define SDRAM_PIN_D22
#define SDRAM_PIN_D23
#define SDRAM_PIN_D24
#define SDRAM_PIN_D25
#define SDRAM_PIN_D26
#define SDRAM_PIN_D27
#define SDRAM_PIN_D28
#define SDRAM_PIN_D29
#define SDRAM_PIN_D30
#define SDRAM_PIN_D31

#define SDRAM_PIN_A0
#define SDRAM_PIN_A1
#define SDRAM_PIN_A2
#define SDRAM_PIN_A3
#define SDRAM_PIN_A4
#define SDRAM_PIN_A5
#define SDRAM_PIN_A6
#define SDRAM_PIN_A7
#define SDRAM_PIN_A8
#define SDRAM_PIN_A9
#define SDRAM_PIN_A10
#define SDRAM_PIN_A11
#define SDRAM_PIN_A12

#define SDRAM_PIN_BA0
#define SDRAM_PIN_BA1

#define SDRAM_PIN_NBL0
#define SDRAM_PIN_NBL1
#define SDRAM_PIN_NBL2
#define SDRAM_PIN_NBL3

#define SDRAM_PIN_SDCLK
#define SDRAM_PIN_SDNWE
#define SDRAM_PIN_SDNRAS
#define SDRAM_PIN_SDNCAS
#define SDRAM_PIN_SDCKE0	GPIO3
#define SDRAM_PIN_SDCKE1
#define SDRAM_PIN_SDNE0		GPIO2
#define SDRAM_PIN_SDNE1

/**
 * Port definitions for SDRAM
 */
#define SDRAM_PORT_D0
#define SDRAM_PORT_D1
#define SDRAM_PORT_D2
#define SDRAM_PORT_D3
#define SDRAM_PORT_D4
#define SDRAM_PORT_D5
#define SDRAM_PORT_D6
#define SDRAM_PORT_D7
#define SDRAM_PORT_D8
#define SDRAM_PORT_D9
#define SDRAM_PORT_D10
#define SDRAM_PORT_D11
#define SDRAM_PORT_D12
#define SDRAM_PORT_D13
#define SDRAM_PORT_D14
#define SDRAM_PORT_D15
#define SDRAM_PORT_D16
#define SDRAM_PORT_D17
#define SDRAM_PORT_D18
#define SDRAM_PORT_D19
#define SDRAM_PORT_D20
#define SDRAM_PORT_D21
#define SDRAM_PORT_D22
#define SDRAM_PORT_D23
#define SDRAM_PORT_D24
#define SDRAM_PORT_D25
#define SDRAM_PORT_D26
#define SDRAM_PORT_D27
#define SDRAM_PORT_D28
#define SDRAM_PORT_D29
#define SDRAM_PORT_D30
#define SDRAM_PORT_D31

#define SDRAM_PORT_A0
#define SDRAM_PORT_A1
#define SDRAM_PORT_A2
#define SDRAM_PORT_A3
#define SDRAM_PORT_A4
#define SDRAM_PORT_A5
#define SDRAM_PORT_A6
#define SDRAM_PORT_A7
#define SDRAM_PORT_A8
#define SDRAM_PORT_A9
#define SDRAM_PORT_A10
#define SDRAM_PORT_A11
#define SDRAM_PORT_A12

#define SDRAM_PORT_BA0
#define SDRAM_PORT_BA1

#define SDRAM_PORT_NBL0
#define SDRAM_PORT_NBL1
#define SDRAM_PORT_NBL2
#define SDRAM_PORT_NBL3

#define SDRAM_PORT_SDCLK
#define SDRAM_PORT_SDNWE
#define SDRAM_PORT_SDNRAS
#define SDRAM_PORT_SDNCAS
#define SDRAM_PORT_SDCKE0
#define SDRAM_PORT_SDCKE1
#define SDRAM_PORT_SDNE0	GPIOC
#define SDRAM_PORT_SDNE1

#endif /* LCD_TEST_SDRAM_IO_CFG_H_ */
