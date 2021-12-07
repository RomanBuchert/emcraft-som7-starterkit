/*
 * i2c.c
 *
 *  Created on: 07.12.2021
 *      Author: RBuchert
 */

#include "i2c.h"

typedef union {
	uint32_t value;
	struct {
		uint32_t PE				:1;
		uint32_t TXIE			:1;
		uint32_t RXIE			:1;
		uint32_t ADDRIE			:1;
		uint32_t NACKIE			:1;
		uint32_t STOPIE			:1;
		uint32_t TCIE			:1;
		uint32_t ERRIE			:1;
		uint32_t DNF			:4;
		uint32_t ANFOFF			:1;
		uint32_t 				:1;
		uint32_t TXDMAEN		:1;
		uint32_t RXDMAEN		:1;
		uint32_t SBC			:1;
		uint32_t NOSTRETCH		:1;
		uint32_t 				:1;
		uint32_t GCEN			:1;
		uint32_t SMBHEN			:1;
		uint32_t SMBDEN			:1;
		uint32_t ALERTEN		:1;
		uint32_t PECEN			:1;
		uint32_t 				:8;
	};
} i2c_cr1_type;

typedef union {
	uint32_t value;
	struct {
		uint32_t SADD			:10;
		uint32_t RD_WRN			:1;
		uint32_t ADD10			:1;
		uint32_t HEAD10R		:1;
		uint32_t START			:1;
		uint32_t STOP			:1;
		uint32_t NACK			:1;
		uint32_t NBYTES			:8;
		uint32_t RELOAD			:1;
		uint32_t AUTOEND		:1;
		uint32_t PECBYTE		:1;
		uint32_t 				:5;
	};
} i2c_cr2_type;

typedef union {
	uint32_t value;
	struct {
		uint32_t SCLL			:8;
		uint32_t SCLH			:8;
		uint32_t SDADEL			:4;
		uint32_t SCLDEL			:4;
		uint32_t 				:4;
		uint32_t PRESC			:4;
	};
} i2c_timingr_type;

typedef union {
	uint32_t value;
	struct {
		uint32_t TXE			:1;
		uint32_t TXIS			:1;
		uint32_t RXNE			:1;
		uint32_t ADDR			:1;
		uint32_t NACKF			:1;
		uint32_t STOPF			:1;
		uint32_t TC				:1;
		uint32_t TCR			:1;
		uint32_t BERR			:1;
		uint32_t ARLO			:1;
		uint32_t OVR			:1;
		uint32_t PECERR			:1;
		uint32_t TIMEOUT		:1;
		uint32_t ALERT			:1;
		uint32_t				:1;
		uint32_t BUSY			:1;
		uint32_t DIR			:1;
		uint32_t ADDCODE		:7;
		uint32_t 				:8;
	};
}i2c_isr_type;

typedef struct{
	i2c_cr1_type 		cr1;
	i2c_cr2_type		cr2;
	uint32_t			oar1;
	uint32_t			oar2;
	i2c_timingr_type	timingr;
	uint32_t			timeoutr;
	i2c_isr_type		isr;
	uint32_t			icr;
	uint32_t			pecr;
//	uint32_t			rxdr;
//	uint32_t			txdr;
} i2c_registers_type;

volatile i2c_registers_type* i2c1_registers;

void i2c_init_regs() {
	i2c1_registers = (i2c_registers_type*)&MMIO32(I2C1_BASE);
}

void i2c_clear_flag(uint32_t i2c, uint32_t flags) {
	I2C_ICR(i2c) = (flags & I2C_ICR_MASK);
}

uint32_t i2c_get_flag(uint32_t i2c, uint32_t flags) {
	return I2C_ISR(i2c) & (flags & I2C_ISR_MASK);
}

uint8_t i2c_get_bytes_to_transfer(uint32_t i2c) {
	return (I2C_CR2(i2c) & I2C_CR2_NBYTES_MASK) >> I2C_CR2_NBYTES_SHIFT;
}

