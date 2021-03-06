cmake_minimum_required(VERSION 3.22)

set(THIS_TARGET_NAME lcd_test)

configure_file(stm32f746ng.ld.in stm32f746ng.ld)

set(LCD_TEST_FLASH_START "0x08000000" CACHE STRING "FLASH start address.")
set(LCD_TEST_FLASH_SIZE "1M" CACHE STRING "FLASH size.")
set(LCD_TEST_IRAM_START "0x20000000" CACHE STRING "Internal RAM start address.")
set(LCD_TEST_IRAM_SIZE "320K" CACHE STRING "Internal RAM size.")
set(LCD_TEST_XRAM_START "0xC0000000" CACHE STRING "External RAM start address.")
set(LCD_TEST_XRAM_SIZE "8M" CACHE STRING "External RAM size.")

add_executable(${THIS_TARGET_NAME} 
	main.c				# Main
	mpu.c				# Memory protection unit
	systimer.c			# System timer functions 
	rcc.c				# Further clock configuration functions for SAI clock tree
	i2c.c				# Further i2c functions not implemente in libopencm3
	sdram.c				# SDRAM initialization. SDRAM is mapped to addr. 0xc0000000
	lcd.c				# STM32F7 LTDC initialization for em-craft module
	gt911.c				# Touch driver on em-craft module
	buttons.c			# Simple functions for hardware user buttons 1  & 2  
	uart.c
)
target_link_options(${THIS_TARGET_NAME}
	PRIVATE -T stm32f746ng.ld -Wl,-Map=${THIS_TARGET_NAME}.map
)
target_link_libraries(${THIS_TARGET_NAME} libopencm3)
target_include_directories(${THIS_TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_command(TARGET ${THIS_TARGET_NAME}
	COMMAND ${CMAKE_OBJCOPY} ARGS -O elf32-littlearm ${CMAKE_CURRENT_BINARY_DIR}/${THIS_TARGET_NAME} ${CMAKE_CURRENT_BINARY_DIR}/${THIS_TARGET_NAME}.elf
	COMMAND ${CMAKE_OBJCOPY} ARGS -O ihex ${CMAKE_CURRENT_BINARY_DIR}/${THIS_TARGET_NAME} ${CMAKE_CURRENT_BINARY_DIR}/${THIS_TARGET_NAME}.hex
	COMMAND ${CMAKE_OBJCOPY} ARGS -O binary ${CMAKE_CURRENT_BINARY_DIR}/${THIS_TARGET_NAME} ${CMAKE_CURRENT_BINARY_DIR}/${THIS_TARGET_NAME}.bin
	COMMAND ${CMAKE_SIZE} ARGS -A -x ${CMAKE_CURRENT_BINARY_DIR}/${THIS_TARGET_NAME}
	COMMAND ${CMAKE_SIZE} ARGS -B -x --totals ${CMAKE_CURRENT_BINARY_DIR}/${THIS_TARGET_NAME}
	COMMAND ${CMAKE_SIZE} ARGS -G -x --totals ${CMAKE_CURRENT_BINARY_DIR}/${THIS_TARGET_NAME}
)

