/*
 * gt911.h
 *
 *  Created on: 06.12.2021
 *      Author: RBuchert
 */

#ifndef LCD_TEST_GT911_H_
#define LCD_TEST_GT911_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define GD911_I2C_7BIT_ADDR1	(0x5D)
#define GD911_I2C_7BIT_ADDR2	(0x14)

struct gt911_info_struct{
	uint32_t ProductID;
	uint16_t FwVersion;
	uint16_t XResolution;
	uint16_t YResolution;
	uint8_t VendorId;
} __attribute__((packed));

void gt911_init();
void gt911_reset(bool state);
void gt911_change_addr(uint8_t new_addr);
int gt911_read(uint16_t reg, void* data, size_t size);
int gt911_write(uint16_t reg, void* data, size_t size);
#endif /* LCD_TEST_GT911_H_ */
