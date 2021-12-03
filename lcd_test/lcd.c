/*
 * lcd.c
 *
 *  Created on: 02.12.2021
 *      Author: RBuchert
 */

#include <stdint.h>
#include <lcd.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/ltdc.h>
#include <rcc.h>

static void lcd_init_gpio();
static void lcd_config_gpio(uint32_t gpio, uint16_t gpios);
static void lcd_init_ltdc();

void lcd_init() {
	lcd_init_gpio();
	gpio_set(GPIOF, GPIO10);
	lcd_set_backlight(100);
	lcd_init_ltdc();

}

void lcd_set_backlight(uint8_t value) {
	if (value) {
		gpio_set(GPIOI, GPIO0);
	}
	else {
		gpio_clear(GPIOI, GPIO0);
	}
}

static void lcd_init_ltdc() {
	// Enable the LTDC clock
	rcc_periph_clock_enable(RCC_LTDC);

	// Disable the LCD-TFT controller
	ltdc_ctrl_disable(LTDC_GCR_LTDCEN);		// Disable display

	// Configure pixel clock
	static struct rcc_sai_pll sai_pll = {
			.vco_mhz = 1,
			.plln = 384,
			.pllp = 0,
			.pllq = 2,
			.pllr = 2,
			.plldivq = 31,
			.plldivr = RCC_DCKCFGR1_PLLSAIDIVR_DIVR_16
	};
	rcc_set_saip_pll(&sai_pll);

	// Configure sync timings.
	static const struct lcd_tft_info disp_info_max = {
			.pixelclock_hz = 12000000,
			.hori_sync = 1,
			.hori_disp_area = 480,
			.hori_back_porch = 255,
			.hori_front_porch = 65,
			.vert_sync = 1,
			.vert_disp_area = 272,
			.vert_back_porch = 31,
			.vert_front_porch = 97
	};
	static const struct lcd_tft_info disp_info_min = {
			.pixelclock_hz = 12000000,
			.hori_sync = 1,
			.hori_disp_area = 480,
			.hori_back_porch = 36,
			.hori_front_porch = 4,
			.vert_sync = 1,
			.vert_disp_area = 272,
			.vert_back_porch = 3,
			.vert_front_porch = 2
	};
	struct lcd_tft_info* disp_info = (struct lcd_tft_info*)&disp_info_min;
	ltdc_set_tft_sync_timings(disp_info->hori_sync, disp_info->vert_sync,
			disp_info->hori_back_porch, disp_info->vert_back_porch,
			disp_info->hori_disp_area, disp_info->vert_disp_area,
			disp_info->hori_front_porch, disp_info->vert_front_porch);

	// Configure signal polarity
	ltdc_ctrl_disable(LTDC_GCR_HSPOL);		// HSync polarity low
	ltdc_ctrl_disable(LTDC_GCR_VSPOL);		// VSync polarity low
	ltdc_ctrl_disable(LTDC_GCR_DITHER);		// Disable dithering
	ltdc_ctrl_disable(LTDC_GCR_PCPOL);		// Clock polarity low
	ltdc_ctrl_disable(LTDC_GCR_DEPOL);		// No data enable is low

	// Set background color (to gray)
	ltdc_set_background_color(127, 127, 127);

	// Enable interrupts

	// Layer 1
	// Window
	ltdc_setup_windowing(LTDC_LAYER_1,
			disp_info->hori_back_porch, disp_info->vert_back_porch,
			disp_info->hori_sync, disp_info->vert_sync,
			disp_info->hori_disp_area, disp_info->vert_disp_area);
	// Pixel  format
	ltdc_set_pixel_format(LTDC_LAYER_1, LTDC_LxPFCR_ARGB8888);
	// Framebuffer start addresss
	ltdc_set_fbuffer_address(LTDC_LAYER_1, 0xC0000000);
	// line length (horizontal pixels)
	ltdc_set_fb_line_length(LTDC_LAYER_1, disp_info->hori_disp_area * 4, disp_info->hori_disp_area * 4);
	// line count (vertical pixels)
	ltdc_set_fb_line_count(LTDC_LAYER_1, disp_info->vert_disp_area);

	// Enable layers
	ltdc_layer_ctrl_enable(LTDC_LAYER_1, LTDC_LxCR_LAYER_ENABLE);

	// Reload shadow register
	ltdc_reload(LTDC_SRCR_RELOAD_IMR);

	// Enable the LCD-TFT controller
	ltdc_ctrl_enable(LTDC_GCR_LTDCEN);
}


static void lcd_init_gpio() {
	// LTDC Interface pins
	static const uint16_t gpioi_pins = GPIO12 | GPIO13 | GPIO14;

	static const uint16_t gpioj_pins = GPIO0  | GPIO1  | GPIO2  | GPIO3  |
						  	  	  	   GPIO4  | GPIO5  | GPIO6  | GPIO7  |
									   GPIO8  | GPIO9  | GPIO10 | GPIO11 |
									   GPIO12 | GPIO13 | GPIO14 | GPIO15;

	static const uint16_t gpiok_pins = GPIO0  | GPIO1  | GPIO2  | GPIO3  |
						  	  	  	   GPIO4  | GPIO5  | GPIO6  | GPIO7;

	rcc_periph_clock_enable(RCC_GPIOF);
	rcc_periph_clock_enable(RCC_GPIOI);
	rcc_periph_clock_enable(RCC_GPIOJ);
	rcc_periph_clock_enable(RCC_GPIOK);

	// Power and backlight pins
	gpio_mode_setup(GPIOF, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO10);
	gpio_set_output_options(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, GPIO10);
	gpio_mode_setup(GPIOI, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO0);
	gpio_set_output_options(GPIOI, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, GPIO0);

	// LTDC interface
	lcd_config_gpio(GPIOI, gpioi_pins);
	lcd_config_gpio(GPIOJ, gpioj_pins);
	lcd_config_gpio(GPIOK, gpiok_pins);

}

static void lcd_config_gpio(uint32_t gpio, uint16_t gpios) {
	gpio_mode_setup(gpio, GPIO_MODE_AF, GPIO_PUPD_NONE, gpios);
	gpio_set_output_options(gpio, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, gpios);
	gpio_set_af(gpio, GPIO_AF14, gpios);
}

