/*
 * uart.h
 *
 *  Created on: 10.12.2021
 *      Author: RBuchert
 */

#ifndef LCD_TEST_UART_H_
#define LCD_TEST_UART_H_

#ifndef STM32F7
#define STM32F7
#endif


#include <stdint.h>
#include <stddef.h>

#include <rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>

typedef struct uart_config_struct uart_config_type;
typedef struct uart_config_pin_struct uart_config_pin_type;
typedef struct uart_config_uart_struct uart_config_uart_type;
typedef struct uart_buffer_struct uart_buffer_type;

struct uart_config_pin_struct {
	enum rcc_periph_clken rcc;
	uint32_t port;
	uint16_t pin;
	uint8_t af;
};

struct uart_config_uart_struct {
	enum rcc_periph_clken rcc;
	uint32_t usart;
	uint32_t baudrate;
};

struct uart_config_struct{
	uart_config_pin_type tx;
	uart_config_pin_type rx;
	uart_config_uart_type uart;
};

extern const uart_config_type uart_usart1_8n1_115200;
extern const uart_config_type uart_usart1_8n1_921600;

void uart_init(uart_config_type* uart_config);
int uart_write(int fd, const void* buf, size_t count);
int uart_read(int fd, void* buf, size_t count);
int uart_getchar();
#endif /* LCD_TEST_UART_H_ */
