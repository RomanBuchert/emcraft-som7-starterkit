
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

uint8_t gt911_set_reg(uint16_t reg) {
}

uint8_t gt911_calc_config_checksum(struct gt911_config_struct* config) {
	uint8_t checksum = 0;
	uint8_t data;
	uint8_t* cfg_ptr = (uint8_t*) config;
	for (size_t i = 0; i < (sizeof(struct gt911_config_struct)-2); i++) {
		checksum += *cfg_ptr++;
	}
	checksum = -checksum;
	config->ConfigChecksum = checksum;
	return checksum;
}

void gt911_update_config(struct gt911_config_struct* config) {
	uint8_t new_checksum = gt911_calc_config_checksum(config);
	config->ConfigFresh = 1;
	gt911_write(0x8047, config, sizeof(struct gt911_config_struct));
}

int gt911_read(uint16_t reg, void* data, size_t size) {

	uint16_t tmpreg = __htons(reg);
	uint32_t timeout;
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
	systimer_remove_timeout(&timeout);
	return retval;
}

int gt911_write(uint16_t reg, void* data, size_t size) {
	uint16_t tmpreg = __htons(reg);
	uint64_t timeout;
	uint8_t* tmpptr;
	int retval = -2;
	char* cdata = calloc(size + 2, 1);
	char* cdata_org = cdata;
	if (cdata == NULL) {
		return -3;
	}

	memcpy(cdata, &tmpreg, 2);
	/*
	tmpptr = (uint8_t*) &tmpreg;
	for (size_t i = 0; i < 2; i++) {
		*cdata++ = *tmpptr++;
	}
	*/

	memcpy(&cdata[2], data, size);
	/*
	tmpptr = (uint8_t*) data;
	for(size_t i = 0; i < size; i++) {
		*cdata++ = *tmpptr++;
	}
	*/
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

#include <stdio.h>
extern int uart_getchar();
extern int uart_kbhit();

static void gt911_print_clear() {
	printf("\033[2J\033[1;1H");
	fflush(stdout);
}

static void gt911_print_gotoxy(int x, int y) {
	printf("\033[%d;%dH", y ,x);
	fflush(stdout);
}

static void gt911_print_header(char c) {
	for (size_t i = 0; i < 80; i++) {
		putchar(c);
	}
	puts(" ");
}

static void gt911_print_mid(const char* string) {
	size_t str_len = strlen(string);
	size_t pos;
	putchar('|');
	for (pos = 1; pos < (80 - str_len) / 2; pos++ ) {
		putchar(' ');
	}
	printf("%s", string);
	pos += str_len;
	for (; pos < 79; pos++) {
		putchar(' ');
	}
	puts("|");
}

static void gt911_print_menuitem(const char* name, const char key) {
	size_t name_len = strlen(name);
	size_t pos;
	printf("| %s ", name);
	pos = name_len + 3;
	for (; pos < 74; pos++) {
		putchar('.');
	}
	printf(" [%c] |", key);
	puts("");
}


static void gt911_print_info() {
	int ch;
	char string[80];
	struct gt911_info_struct info;
	do {
		gt911_read(0x8140, &info, sizeof(info));
		gt911_print_clear();
		gt911_print_header('=');
		gt911_print_mid("GD911 Info");
		gt911_print_header('-');
		char* product_id = (char*) &info.ProductID;
		sprintf(string, "Product ID: %c%c%c%c", *(product_id++), *(product_id++),*(product_id++),*(product_id++));
		gt911_print_mid(string);
		sprintf(string, "Firmware Version: %2.2x", info.FwVersion);
		gt911_print_mid(string);
		sprintf(string, "X-Resolution: %d", info.XResolution);
		gt911_print_mid(string);
		sprintf(string, "Y-Resolution: %d", info.YResolution);
		gt911_print_mid(string);
		sprintf(string, "Vendor ID: %d", info.VendorId);
		gt911_print_mid(string);
		gt911_print_mid("");
		gt911_print_menuitem("Quit", 'q');
		gt911_print_header('=');

		ch = uart_getchar();
	}while (ch != 'q');
}

static void gt911_print_read_data() {
	int ch = 0;
	static uint32_t first_reg;
	uint8_t reg_val[32];
	char string[80];
	do {
		gt911_print_clear();
		fflush(stdout);
		fflush(stdin);
		printf("First register: ");
		fflush(stdout);
		fflush(stdin);
		gets(string);
		sscanf(string, "%x", &first_reg);
		gt911_print_clear();
		do {
			gt911_read(first_reg, reg_val, 64);
			gt911_print_gotoxy(1, 1);
			gt911_print_header('=');
			gt911_print_mid("GD911 Info");
			gt911_print_header('-');
			for (size_t i = 0; i < 64; i++) {
				sprintf(string, "Reg: %4.4X = %2.2X %3i %+4d", first_reg + i, reg_val[i], reg_val[i], (int8_t)reg_val[i]);
				gt911_print_mid(string);
			}
			gt911_print_mid("");
			gt911_print_menuitem("Quit", 'q');
			gt911_print_header('=');
			ch = 0;
			gt911_write(0x814E, &ch, 1);
			systimer_delay_ms(5);
		} while (!uart_kbhit());
		ch = uart_getchar();
	} while (ch != 'q');
}

static void gt911_print_touchpoints() {
	int ch = 0;
	static struct gt911_positions_struct positions;

	static char string[80];
	gt911_print_clear();
	do {
		gt911_print_gotoxy(1, 1);
		gt911_print_header('=');
		gt911_print_mid("GD911 Info");
		gt911_print_header('-');
		gt911_read(0x814e, &positions, sizeof(struct gt911_positions_struct));
		if (positions.Info.BufferStatus) {
			sprintf(string, "Status: %2.2X           ", positions.Info.Value);
			gt911_print_mid(string);
			sprintf(string, "   Buffer: %1d          ", positions.Info.BufferStatus);
			gt911_print_mid(string);
			sprintf(string, "   Large Detect: %1d    ", positions.Info.LargeDetect);
			gt911_print_mid(string);
			sprintf(string, "   Proximity Valid: %1d ", positions.Info.ProximityValid);
			gt911_print_mid(string);
			sprintf(string, "   Have Key: %1d        ", positions.Info.HaveKey);
			gt911_print_mid(string);
			sprintf(string, "   Number of Points: %1d", positions.Info.Touchpoints);
			gt911_print_mid(string);
			sprintf(string, "TrackId: %d", positions.TrackId);
			gt911_print_mid(string);
			for (size_t i = 0; i < 5; i++) {
				if (i < positions.Info.Touchpoints) {
					sprintf(string, "X: %4d, Y: %4d, Size: %4d, Id: %1d",
							positions.Point[i].X,
							positions.Point[i].Y,
							positions.Point[i].Size,
							positions.Point[i].TrackId
					);
					gt911_print_mid(string);
				}
				else {
					gt911_print_mid("");
				}
			}
			gt911_print_mid("");
			gt911_print_menuitem("Quit", 'q');
			gt911_print_header('=');
			ch = 0;
			gt911_write(0x814E, &ch, 1);
		}
		systimer_delay_ms(5);
		ch = 0;
		if (uart_kbhit()) {
			ch = uart_getchar();
		}
	} while (ch != 'q');

}

void gt911_debug() {
	int ch;
	do {
		printf("\33[?25l");
		fflush(stdout);
		gt911_print_clear();
		gt911_print_header('=');
		gt911_print_mid("GD911 Info");
		gt911_print_header('-');
		gt911_print_menuitem("Read Info", '1');
		gt911_print_menuitem("Read Data", '2');
		gt911_print_menuitem("Touchpoints", '3');
		gt911_print_mid("");
		gt911_print_menuitem("Quit", 'q');
		gt911_print_header('=');
		ch = uart_getchar();
		switch (ch) {
		case '1':
			gt911_print_info();
			break;
		case '2':
			gt911_print_read_data();
			break;
		case '3':
			gt911_print_touchpoints();
			break;
		default:
			break;
		}
	} while (ch != 'q');
	gt911_print_clear();
}
