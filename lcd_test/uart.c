/*
 * uart.c
 *
 *  Created on: 10.12.2021
 *      Author: RBuchert
 */
#include <uart.h>
#include <stddef.h>

static void uart_init_gpio(uart_config_type* uart_config);
static void uart_init_uart(uart_config_type* uart_config);
static uint32_t uart_stdio = (uint32_t)NULL;

const uart_config_type uart_usart1_8n1_115200 = {
		.tx = {
				.rcc = RCC_GPIOB,
				.port = GPIOB,
				.pin = GPIO6,
				.af = GPIO_AF7,
		},
		.rx = {
				.rcc = RCC_GPIOA,
				.port = GPIOA,
				.pin = GPIO10,
				.af = GPIO_AF7,
		},
		.uart = {
				.rcc = RCC_USART1,
				.usart = USART1,
				.baudrate = 115200,
		}
};

const uart_config_type uart_usart1_8n1_921600 = {
		.tx = {
				.rcc = RCC_GPIOB,
				.port = GPIOB,
				.pin = GPIO6,
				.af = GPIO_AF7,
		},
		.rx = {
				.rcc = RCC_GPIOA,
				.port = GPIOA,
				.pin = GPIO10,
				.af = GPIO_AF7,
		},
		.uart = {
				.rcc = RCC_USART1,
				.usart = USART1,
				.baudrate = 921600,
		}
};

void uart_init(uart_config_type* uart_config) {
	uart_init_gpio(uart_config);
	uart_init_uart(uart_config);
	uart_stdio = uart_config->uart.usart;

}

/*
 * PA10: UART1_RXD
 * PB6 : UART1_TXD
 */
static void uart_init_gpio(uart_config_type* uart_config) {

	// RxD
	rcc_periph_clock_enable(uart_config->rx.rcc);
	gpio_mode_setup(uart_config->rx.port,
			GPIO_MODE_AF,
			GPIO_PUPD_NONE,
			uart_config->rx.pin);
	gpio_set_af(uart_config->rx.port,
			uart_config->rx.af,
			uart_config->rx.pin);

	// TxD
	rcc_periph_clock_enable(uart_config->tx.rcc);
	gpio_mode_setup(uart_config->tx.port,
			GPIO_MODE_AF,
			GPIO_PUPD_NONE,
			uart_config->tx.pin);
	gpio_set_output_options(uart_config->tx.port,
			GPIO_OTYPE_PP,
			GPIO_OSPEED_100MHZ,
			uart_config->tx.pin);
	gpio_set_af(uart_config->tx.port,
			uart_config->tx.af,
			uart_config->tx.pin);
}

static void uart_init_uart(uart_config_type* uart_config) {
	rcc_periph_clock_enable(uart_config->uart.rcc);

	usart_disable(uart_config->uart.usart);
#if 0
	usart_clear_rx_fifo(uart_config->uart.usart);
	usart_clear_tx_fifo(uart_config->uart.usart);
	usart_clear_rx_interrupt(uart_config->uart.usart);
	usart_clear_tx_interrupt(uart_config->uart.usart);
#endif
	usart_set_baudrate(uart_config->uart.usart, uart_config->uart.baudrate);
	usart_set_databits(uart_config->uart.usart, 8);
	usart_set_stopbits(uart_config->uart.usart, USART_STOPBITS_1);
	usart_set_mode(uart_config->uart.usart, USART_MODE_TX_RX);
	usart_set_parity(uart_config->uart.usart, USART_PARITY_NONE);
	usart_set_flow_control(uart_config->uart.usart, USART_FLOWCONTROL_NONE);
	usart_enable(uart_config->uart.usart);
}

int uart_write(int fd, const void* buf, size_t count) {
	char* cbuf = (char*) buf;
	size_t to_sent = count;
	if (fd > 2) {
		return -1;
	}
	if (uart_stdio == (uint32_t)NULL) {
		return -1;
	}
	while (*cbuf && to_sent--) {
		usart_send_blocking(uart_stdio, *(cbuf++));

	}
	return count - to_sent;
}

int uart_read(int fd, void* buf, size_t count) {
	char* cbuf = (char*) buf;
	char c;
	size_t to_read = count;
	if (fd > 2) {
		return -1;
	}

	if (uart_stdio == (uint32_t)NULL) {
		return -1;
	}

	while (to_read--) {
		c = (char) usart_recv_blocking(uart_stdio);
		*(cbuf++) = c;
		if ((c == '\r') || (c == '\n')) {
			*(cbuf) = 0x00;
			break;
		}
	}
	return count - to_read;
}

int uart_getchar() {
	return usart_recv_blocking(uart_stdio);
}

int uart_kbhit() {
	return usart_get_flag(uart_stdio, USART_ISR_RXNE);
}
