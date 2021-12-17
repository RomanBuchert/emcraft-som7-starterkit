
#ifndef STM32F7
#define STM32F7
#endif
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/fsmc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/vector.h>
#include <libopencm3/cm3/scb.h>

#include <i2c.h>
#include <buttons.h>
#include <sdram.h>
#include <gt911.h>
#include <lcd.h>
#include <systimer.h>
#include <uart.h>
extern unsigned _stack;
extern uint32_t end;

#define HEAP_SIZE (64*1024)
//static const void* heap_end = (void*)0xC4000000;
const void* heap_end = ((uint32_t)&end) + HEAP_SIZE;
static void* heap_start = NULL;
static void* prev_heap_end = NULL;
static void* curr_heap_end = NULL;

/* memory mapping struct for System Control Block */
typedef struct {
	uint32_t CPUID;                        /*!< CPU ID Base Register                                     */
	uint32_t ICSR;                         /*!< Interrupt Control State Register                         */
	uint32_t VTOR;                         /*!< Vector Table Offset Register                             */
	uint32_t AIRCR;                        /*!< Application Interrupt / Reset Control Register           */
	uint32_t SCR;                          /*!< System Control Register                                  */
	uint32_t CCR;                          /*!< Configuration Control Register                           */
	uint8_t  SHP[12];                      /*!< System Handlers Priority Registers (4-7, 8-11, 12-15)    */
	uint32_t SHCSR;                        /*!< System Handler Control and State Register                */
	uint32_t CFSR;                         /*!< Configurable Fault Status Register                       */
	uint32_t HFSR;                         /*!< Hard Fault Status Register                                       */
	uint32_t DFSR;                         /*!< Debug Fault Status Register                                          */
	uint32_t MMFAR;                        /*!< Mem Manage Address Register                                  */
	uint32_t BFAR;                         /*!< Bus Fault Address Register                                   */
	uint32_t AFSR;                         /*!< Auxiliary Fault Status Register                              */
	uint32_t PFR[2];                       /*!< Processor Feature Register                               */
	uint32_t DFR;                          /*!< Debug Feature Register                                   */
	uint32_t ADR;                          /*!< Auxiliary Feature Register                               */
	uint32_t MMFR[4];                      /*!< Memory Model Feature Register                            */
	uint32_t ISAR[5];                      /*!< ISA Feature Register                                     */
} __attribute__((packed)) scb_type;

typedef struct{
	uint32_t TYPE;
	uint32_t CTRL;
	uint32_t RNR;
	uint32_t RBAR;
	uint32_t RASR;
	uint32_t RBAR_A1;
	uint32_t RASR_A1;
	uint32_t RBAR_A2;
	uint32_t RASR_A2;
	uint32_t RBAR_A3;
	uint32_t RASR_A3;
} __attribute__((packed)) mpu_type;;

volatile scb_type* scb = (volatile scb_type*)SCB_BASE;
volatile mpu_type* mpu = (volatile mpu_type*)MPU_BASE;

void* _sbrk(int incr) {
	if (heap_start == NULL) {
		heap_start = ((uint32_t)heap_end - HEAP_SIZE);
	}
	if (curr_heap_end == NULL) {
		curr_heap_end = heap_start;
	}
	prev_heap_end = curr_heap_end;
	if ((curr_heap_end + incr) < heap_end) {
		curr_heap_end += incr;
	}
	else {
		return (void*)-1;
	}
	return (void*)prev_heap_end;
}

int _write(int fd, char* ptr, int len) {
	return uart_write(fd, ptr, len);
}

int _read(int fd, char* ptr, int len) {
	return uart_read(fd, ptr, len);
}

void SetPixel(size_t x, size_t y, uint32_t c);

void int2led(void) {
	GPIOD_ODR &= ~GPIO3;
	GPIOD_ODR |= (GPIOI_IDR & GPIO10) >> 7;
	if (gpio_get(GPIOI, GPIO10)) {
		gpio_set(GPIOD, GPIO3);
	}
	else {
		gpio_clear(GPIOD, GPIO3);
	}
}

void hard_fault_handler() {
	uint32_t loop = 1;
	while(loop);
}

void usage_fault_handler() {
	uint32_t loop = 1;
	while(loop);
}

void main() {
	//SCB_VTOR = (uint32_t)&vector_table;
	//SCB_VTOR = 0;
	SCB_SHCSR = 0x70000;
	int number;
	struct gt911_config_struct gt911_config;
	bool DisplayOn = true;
	rcc_clock_setup_hse(&rcc_3v3[RCC_CLOCK_3V3_216MHZ], 12);
	rcc_periph_clock_enable(RCC_GPIOD);
	gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO3);
	gpio_set_output_options(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, GPIO3);
	gpio_clear(GPIOD, GPIO3);
	sdram_init();
	systimer_init();
	uart_init(&uart_usart1_8n1_115200);
	//printf("Stack: %p; End: %p\r\n", &_stack, &end);
	i2c_init_regs();
	button_init();
	/*
	while(!button_get(USR_BTN1)) {
		;
	}
	 */
	lcd_init();
	lcd_set_backlight(0);
	gt911_init();
	gt911_read(0x8047, &gt911_config, sizeof(struct gt911_config_struct));
	gt911_config.ModuleSwitch1 &= ~0x08;
	//gt911_config.YOutputMax = 272;
	gt911_update_config(&gt911_config);
	gt911_read(0x8047, &gt911_config, sizeof(struct gt911_config_struct));
#if 0
#endif
	gt911_debug();
#if 0
	for(uint32_t* addr = (uint32_t*)0xC0000000; addr < (uint32_t*)0xC4000000; addr++) {
		*addr = 0;
	}
	for (uint32_t* addr = (uint32_t*)0xC0000000; addr < (uint32_t*)(0xC0000000 + (480*272*4)); addr++) {
		*addr = 0xFF000000;
	}

	for (;;) {
		if (button_get(USR_BTN1)) {
			DisplayOn ^= true;
			while(button_get(USR_BTN1)) {
				;
			}
			lcd_set_backlight(DisplayOn);
		}
	}
#endif
	char tmp = 0;

	uint16_t gt911_info_data[22];
	volatile int retval;
	uint64_t cntr = 0;
	retval = gt911_write(0x8040, &tmp, 1);
	if (retval != 0) {
		gt911_init();
	}

	for (;;) {
		while (gpio_get(GPIOI, GPIO10) == 0) {
			;
		}
		cntr++;
		retval = gt911_read(0x814E, gt911_info_data, 42);
		if (retval != 0) {
			gt911_init();
		}
		else {
			if (gt911_info_data[0] & 0x80) {
				retval = gt911_write(0x814E, &tmp, 0);
			}
			if (retval != 0) {
				gt911_init();
			}
		}
		while(gpio_get(GPIOI, GPIO10));
		systimer_delay_ms(100);
	}
}

void SetPixel(size_t x, size_t y, uint32_t c) {
	uint32_t* addr = (uint32_t*)(0xC0000000+(4*(y*480+x)));
	*addr = c;
}
