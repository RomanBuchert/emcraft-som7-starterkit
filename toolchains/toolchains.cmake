message("Including toolchains.cmake")

set(TC_TOOLCHAIN_LIST
	arm-none-eabi-gcc.cmake
)

set(TC_SELECTED_TOOLCHAIN_FILE arm-none-eabi-gcc.cmake CACHE STRING "Toolchain for compiling.")
set_property(CACHE TC_SELECTED_TOOLCHAIN_FILE PROPERTY STRINGS ${TC_TOOLCHAIN_LIST})

include(${CMAKE_CURRENT_LIST_DIR}/${TC_SELECTED_TOOLCHAIN_FILE})
