/*
 * i2c.h
 *
 *  Created on: 07.12.2021
 *      Author: RBuchert
 */

#ifndef LCD_TEST_I2C_H_
#define LCD_TEST_I2C_H_

#include <libopencm3/stm32/i2c.h>


#define I2C_ICR_MASK	(I2C_ICR_ADDRCF | I2C_ICR_NACKCF | I2C_ICR_STOPCF | I2C_ICR_BERRCF   | \
						 I2C_ICR_ARLOCF | I2C_ICR_OVRCF  | I2C_ICR_PECCF  | I2C_ICR_TIMOUTCF | \
						 I2C_ICR_ALERTCF)

#define I2C_ISR_DIR		(0x1 << 16)

#define I2C_ISR_MASK	(I2C_ISR_TXE     | I2C_ISR_TXIS  | I2C_ISR_RXNE | I2C_ISR_ADDR   | \
						 I2C_ISR_NACKF   | I2C_ISR_STOPF | I2C_ISR_TC   | I2C_ISR_TCR    | \
						 I2C_ISR_BERR    | I2C_ISR_ARLO  | I2C_ISR_OVR  | I2C_ISR_PECERR | \
						 I2C_ISR_TIMEOUT | I2C_ISR_ALERT | I2C_ISR_BUSY | I2C_ISR_DIR)

void i2c_init_regs();
void i2c_clear_flag(uint32_t i2c, uint32_t flags);

uint32_t i2c_get_flag(uint32_t i2c, uint32_t flags);

uint8_t i2c_get_bytes_to_transfer(uint32_t i2c);
#endif /* LCD_TEST_I2C_H_ */
