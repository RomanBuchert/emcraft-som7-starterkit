
/*
 * gt911.c
 *
 *  Created on: 06.12.2021
 *      Author: RBuchert
 */
#ifndef STM32F7
#define STM32F7
#endif

#include <gt911.h>
#include <stdlib.h>
#include <string.h>
#include <machine/endian.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <i2c.h>
#include <systimer.h>

static void gt911_init_gpio();
static void gt911_init_i2c();

static uint8_t gt911_i2c_addr = GD911_I2C_7BIT_ADDR1;
static uint32_t gt911_i2c_device;

static struct gt911_info_struct gt911_info;
uint8_t gt911_data[192];
uint8_t gt911_info_data[312];

void gt911_init() {
	gt911_init_gpio();
	gt911_init_i2c();
	gt911_reset(false);
#if 0
	//gt911_change_addr(GD911_I2C_7BIT_ADDR2);
	gt911_read(0x8140, &gt911_info, sizeof(struct gt911_info_struct));
	uint16_t resolution;
	resolution = 480;
	gt911_write(0x0848, &resolution, 2);
	gt911_read(0x0848, &resolution, 2);
	resolution = 272;
	gt911_write(0x084A, &resolution, 2);
	gt911_read(0x084A, &resolution, 2);
	gt911_read(0x8140, &gt911_info, sizeof(struct gt911_info_struct));
	gt911_info.ProductID = __ntohl(gt911_info.ProductID);
	gt911_info.FwVersion = __ntohs(gt911_info.FwVersion);
	gt911_info.XResolution = __ntohs(gt911_info.XResolution);
	gt911_info.YResolution = __ntohs(gt911_info.YResolution);
	gt911_read(0x8040, gt911_data, 192);
	gt911_read(0x814E, gt911_info_data, 42);
#endif
}

void gt911_reset(bool state) {
	if (state) {
		gpio_clear(GPIOH, GPIO8);
		systimer_delay_ms(100);
	}
	else {
		//reconfigure INT to output to set i2c address.
		gpio_mode_setup(GPIOI, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO10);
		gpio_set_output_options(GPIOI, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, GPIO10);
		if (gt911_i2c_addr == GD911_I2C_7BIT_ADDR2) {
			gpio_set(GPIOI, GPIO10);
		}
		else {
			gpio_clear(GPIOI, GPIO10);
		}
		systimer_delay_ms(10);
		gpio_set(GPIOH, GPIO8);
		systimer_delay_ms(50);
		gpio_mode_setup(GPIOI, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO10);
		systimer_delay_ms(100);
		//i2c_set_7bit_address(gt911_i2c_device, gt911_i2c_addr);
	}
}

void gt911_change_addr(uint8_t new_addr) {
	if ((new_addr == GD911_I2C_7BIT_ADDR1) || (new_addr == GD911_I2C_7BIT_ADDR2)) {
		if (new_addr != gt911_i2c_addr) {
			gt911_i2c_addr = new_addr;
			gt911_reset(true);
			systimer_delay_ms(1);
			gt911_reset(false);
		}
	}
}
void gt911_init_gpio() {
	// nRst on PH8
	rcc_periph_clock_enable(RCC_GPIOH);
	gpio_mode_setup(GPIOH, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO8);
	gpio_set_output_options(GPIOH, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, GPIO8);
	gt911_reset(true);

	// Touch interrupt on PI10
	rcc_periph_clock_enable(RCC_GPIOI);
	gpio_mode_setup(GPIOI, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO10);
	//gpio_set_af(GPIOI, GPIO_AF15, GPIO10);
}

void gt911_init_i2c() {
	// SDA on PB7 / SCL on PB8
	gt911_i2c_device = I2C1;
	rcc_periph_clock_enable(RCC_GPIOB);
	gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO7 | GPIO8);
	gpio_set_output_options(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_100MHZ, GPIO7 | GPIO8);
	gpio_set_af(GPIOB, GPIO_AF4, GPIO7 | GPIO8);
	gpio_set(GPIOB, GPIO7 | GPIO8);

	rcc_periph_clock_enable(RCC_I2C1);
	i2c_reset(gt911_i2c_device);
	i2c_peripheral_disable(gt911_i2c_device);
	i2c_enable_analog_filter(gt911_i2c_device);
	i2c_set_digital_filter(gt911_i2c_device, 0);
	i2c_set_speed(gt911_i2c_device, i2c_speed_sm_100k, rcc_apb1_frequency / 1000000);
	i2c_disable_stretching(gt911_i2c_device);
	i2c_set_7bit_addr_mode(gt911_i2c_device);
	//i2c_set_7bit_address(gt911_i2c_device, gt911_i2c_addr);
	i2c_peripheral_enable(gt911_i2c_device);

}

int gt911_set_reg(uint16_t reg) {
}

int gt911_read(uint16_t reg, void* data, size_t size) {

	uint16_t tmpreg = __htons(reg);
	uint64_t timeout;
	int retval = -2;
	char* cdata = (char*) &tmpreg;
	size_t retries = 3;
	// Prepare communication to send the register address
	i2c_clear_flag(gt911_i2c_device, I2C_ICR_MASK);
	i2c_set_7bit_address(gt911_i2c_device, gt911_i2c_addr);
	i2c_set_7bit_addr_mode(gt911_i2c_device);
	i2c_set_write_transfer_dir(gt911_i2c_device);
	i2c_set_bytes_to_transfer(gt911_i2c_device, 2);
	i2c_enable_autoend(gt911_i2c_device);
	i2c_disable_stretching(gt911_i2c_device);
	//Preload first byte to send
	i2c_send_data(gt911_i2c_device, *cdata++);
	//start sending the register address
	i2c_send_start(gt911_i2c_device);
	systimer_add_timeout(&timeout, 100);
	//wait for TXIS or NACKF
	while(timeout) {
		if (i2c_get_flag(gt911_i2c_device, I2C_ISR_TXIS)) {
			retval = 0;
			break;
		}
		if (i2c_get_flag(gt911_i2c_device, I2C_ISR_NACKF)) {
			retval = -1;
			break;
		}
	}
	// Address successfully send. Now send register
	if (retval == 0) {
		size_t data2send = i2c_get_bytes_to_transfer(gt911_i2c_device) - 1;
		while (data2send--) {
			i2c_send_data(gt911_i2c_device, *cdata);
			timeout = 250;
			retval = -2;			// set retval to timeout error
			while (timeout) {
				if (i2c_get_flag(gt911_i2c_device, I2C_ISR_TXIS)) {
					retval = 0;
					break;
				}
				if (i2c_get_flag(gt911_i2c_device, I2C_ISR_NACKF)) {
					retval = -1;
				}
				if (i2c_get_flag(gt911_i2c_device, I2C_ISR_TC | I2C_ISR_STOPF)) {
					data2send = 0;
					retval = 0;
					break;
				}
			}
			// Error occurred, stop sending data.
			if (retval != 0) {
				break;
			}
			cdata++;
		};
	}

	// Register successfully send
	if (retval == 0) {
		// reconfigure to get data
		cdata = (char*) data;
		size_t data2receive = size;
		i2c_set_read_transfer_dir(gt911_i2c_device);
		i2c_set_bytes_to_transfer(gt911_i2c_device, size);
		i2c_send_start(gt911_i2c_device);
		// it's important, to enable autoend AFTERWARDS to do a proper repeated start
		i2c_enable_autoend(gt911_i2c_device);
		while (size--) {
			//wait for data
			retval = -2;
			timeout = 250;
			while(timeout) {
				if (i2c_get_flag(gt911_i2c_device, I2C_ISR_RXNE)) {
					retval = 0;
					break;
				}
			}
			// Error occurred, stop receiving data
			if (retval != 0) {
				break;
			}
			*(cdata++) = i2c_get_data(gt911_i2c_device);
		}
	}
#if 0
	if (retval != 0) {
		i2c_send_stop(gt911_i2c_device);
	}
#endif
	return retval;
}

int gt911_write(uint16_t reg, void* data, size_t size) {
	uint16_t tmpreg = __htons(reg);
	uint64_t timeout;
	int retval = -2;
	char* cdata = calloc(size + 2, 1);
	char* cdata_org = cdata;
	if (cdata == NULL) {
		return -3;
	}
	memcpy(cdata, &tmpreg, 2);
	memcpy(&cdata[2], data, size);

	size_t retries = 3;
	do {
		// Prepare communication to send the register address
		i2c_clear_flag(gt911_i2c_device, I2C_ICR_MASK);
		i2c_set_7bit_address(gt911_i2c_device, gt911_i2c_addr);
		i2c_set_7bit_addr_mode(gt911_i2c_device);
		i2c_set_write_transfer_dir(gt911_i2c_device);
		i2c_set_bytes_to_transfer(gt911_i2c_device, 2 + size);
		i2c_enable_autoend(gt911_i2c_device);
		i2c_disable_stretching(gt911_i2c_device);
		//Preload first byte to send
		i2c_send_data(gt911_i2c_device, *cdata++);
		//start sending the register address
		i2c_send_start(gt911_i2c_device);
		systimer_add_timeout(&timeout, 100);
		//wait for TXIS or NACKF
		while(timeout) {
			if (i2c_get_flag(gt911_i2c_device, I2C_ISR_TXIS)) {
				retval = 0;
				break;
			}
			if (i2c_get_flag(gt911_i2c_device, I2C_ISR_NACKF)) {
				retval = -1;
				break;
			}
		}
		if (retval == 0) {
			break;
		}
		else {
			//i2c_send_stop(gt911_i2c_device);
			gt911_reset(true);
			gt911_reset(false);
		}

	} while(retries--);
	// Address successfully send. Now send register
	if (retval == 0) {
		size_t data2send = i2c_get_bytes_to_transfer(gt911_i2c_device) - 1;
		while (data2send--) {
			i2c_send_data(gt911_i2c_device, *cdata);
			timeout = 250;
			retval = -2;			// set retval to timeout error
			while (timeout) {
				if (i2c_get_flag(gt911_i2c_device, I2C_ISR_TXIS)) {
					retval = 0;
					break;
				}
				if (i2c_get_flag(gt911_i2c_device, I2C_ISR_NACKF)) {
					retval = -1;
				}
				if (i2c_get_flag(gt911_i2c_device, I2C_ISR_TC | I2C_ISR_STOPF)) {
					data2send = 0;
					retval = 0;
					break;
				}
			}
			// Error occurred, stop sending data.
			if (retval != 0) {
				break;
			}
			cdata++;
		};
	}
	free(cdata_org);
	return retval;
}
