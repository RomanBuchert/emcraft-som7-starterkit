cmake_minimum_required(VERSION 3.22)

include(toolchains/toolchains.cmake)

project(lcd_test LANGUAGES C CXX ASM)

#Override cached variables
set (LIBOPENCM3_FAMILY stm32f7 CACHE STRING "Libopencm3 family" FORCE)

set(TC_C_FLAGS_ARM_FAMILY cortex-m7 CACHE STRING "ARM family (-mcpu)" FORCE)
set(TC_C_FLAGS_FLOAT_ABI hard CACHE STRING "Float ABI (-mfloat-abi)" FORCE)

set(TC_L_FLAGS_NANO_SPECS ON CACHE BOOL "Use newlib-nano (--specs=nano.specs)" FORCE)
set(TC_L_FLAGS_NOSYS_SPECS ON CACHE BOOL "Do not use syscalls (--specs=nosys.specs)" FORCE) 
set(TC_L_FLAGS_NOSTARTFILES ON CACHE BOOL "Do not use startfiles (-nostartfiles)" FORCE)
set(TC_TOOLCHAIN_PATH "C:/Program Files (x86)/GNU Arm Embedded Toolchain/10 2021.07" CACHE PATH "Path to toolchain" FORCE)

include(libraries/libopencm3.cmake)

add_subdirectory(lcd_test)

message("Include Pfade: ${INCLUDE_DIRECTORIES}")
