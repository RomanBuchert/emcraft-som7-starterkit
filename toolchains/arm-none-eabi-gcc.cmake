message("Including arm-none-eabi-gcc.cmake")

set(CMAKE_SYSTEM_NAME               Generic)
set(CMAKE_SYSTEM_PROCESSOR          arm)


set(CMAKE_CROSSCOMPILING	TRUE)
# Without that flag CMake is not able to pass test compilation check
set(CMAKE_TRY_COMPILE_TARGET_TYPE   STATIC_LIBRARY)

################################################################################
# Configuration
# Build Type
set(TC_BUILDTYPES
	Debug
	Release
)

set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${TC_BUILDTYPES})

# Toolchain path
set(TC_TOOLCHAIN_PATH "" CACHE PATH "Path to toolchain")

# Optimization
set(TC_OPTIMIZATION
	-O0
	-O1
	-O2
	-O3
	-Os
	-Ofast
	-Og
)
set(TC_OPTIMIZATION_DEBUG -O0 CACHE STRING "Optimization for Debug")
set_property(CACHE TC_OPTIMIZATION_DEBUG PROPERTY STRINGS ${TC_OPTIMIZATION})
set(TC_OPTIMIZATION_RELEASE -O2 CACHE STRING "Optimization for Release")
set_property(CACHE TC_OPTIMIZATION_RELEASE PROPERTY STRINGS ${TC_OPTIMIZATION})
set(TC_OPTIMIZATION_LANGSTD_C "" CACHE STRING "Language standard (--std=)")
set(TC_OPTIMIZATION_LANGSTD_CXX "" CACHE STRING "Language standard (--std=)")
set(TC_OPTIMIZATION_OTHER "" CACHE STRING "Other optimization flags")

if(WIN32)
	set(CMAKE_EXECUTABLE_SUFFIX .exe)
endif()

# Debug
set (TC_DEBUG_LEVEL_LIST
	""	#None
	-g1	#Minimal
	-g	#Default
	-g3 #Maximum
)

set(TC_DEBUG_LEVEL -g CACHE STRING "Debug Level")
set_property(CACHE TC_DEBUG_LEVEL PROPERTY STRINGS ${TC_DEBUG_LEVEL_LIST})

set(TC_DEBUG_FLAGS_OTHER "" CACHE STRING "Other debug flags")

# Assembler flags
set(TC_ASM_FLAGS_OTHER "" CACHE STRING "Additional assembler flags")

# Compiler flags
set(TC_C_FLAGS_OTHER "" CACHE STRING "Additional compiler flags")

set(TC_C_FLAGS_FLOAT_ABI_LIST
	soft
	softfp
	hard
)
set(TC_C_FLAGS_FLOAT_ABI soft CACHE STRING "Float ABI (-mfloat-abi)")
set_property(CACHE TC_C_FLAGS_FLOAT_ABI PROPERTY STRINGS ${TC_C_FLAGS_FLOAT_ABI_LIST})

set(TC_C_FLAGS_FLOAT_FPU "" CACHE STRING "FPU hardware (-mfpu)")

set(TC_C_FLAGS_ARM_FAMILY "" CACHE STRING "ARM family (-mcpu)")
set(TC_C_FLAGS_MARCH "" CACHE STRING "ARM architecture (-march)")

set(TC_C_FLAGS_ARM_INSTRUCTIONSET_LIST
	-mthumb
	-marm
) 
set(TC_C_FLAGS_ARM_INSTRUCTIONSET -mthumb CACHE STRING "ARM instruction set")
set_property(CACHE TC_C_FLAGS_ARM_INSTRUCTIONSET PROPERTY STRINGS ${TC_C_FLAGS_ARM_INSTRUCTIONSET_LIST})

# Linker flags
set(TC_L_FLAGS_NANO_SPECS OFF CACHE BOOL "Use newlib-nano (--specs=nano.specs)")
set(TC_L_FLAGS_NOSYS_SPECS OFF CACHE BOOL "Do not use syscalls (--specs=nosys.specs)") 
set(TC_L_FLAGS_NOSTARTFILES OFF CACHE BOOL "Do not use startfiles (-nostartfiles)")
set(TC_L_FLAGS_NODEFAULTLIBS OFF CACHE BOOL "Do not use default libs (-nodefaultlibs)")
set(TC_L_FLAGS_NOSTARTDEFAULTLIBS OFF CACHE BOOL "No startup or default libs (-nostdlib)")
set(TC_L_FLAGS_REM_UNUSED_SECTIONS ON CACHE BOOL "Remove unused sections (-Xlinker --gc-sections)")
set(TC_L_FLAGS_OTHER "" CACHE STRING "Other linker flags")

# Process configuration
if (NOT ${TC_TOOLCHAIN_PATH} STREQUAL "")
	set (TC_TOOLCHAIN_BINARY_PATH
		${TC_TOOLCHAIN_PATH}/bin/
	)
	set (TC_TOOLCHAIN_INCLUDE_PATH
		${TC_TOOLCHAIN_PATH}/arm-none-eabi/include/
	)
	set (TC_TOOLCHAIN_LIBRARY_PATH
		${TC_TOOLCHAIN_PATH}/arm-none-eabi/lib/
	)
endif()


set(COMPILE_OPTIONS
	$<$<COMPILE_LANGUAGE:C>:${TC_C_FLAGS_OTHER}>
	$<$<COMPILE_LANGUAGE:CXX>:${TC_C_FLAGS_OTHER}>
	$<$<COMPILE_LANGUAGE:ASM>:${TC_ASM_FLAGS_OTHER}>
	$<$<CONFIG:Debug>:${TC_OPTIMIZATION_DEBUG}>
	$<$<CONFIG:Debug>:${TC_DEBUG_LEVEL}>
	$<$<CONFIG:Debug>:${TC_DEBUG_FLAGS_OTHER}>
	$<$<CONFIG:Release>:${TC_OPTIMIZATION_RELEASE}>
	$<$<CONFIG:Release>:${TC_RELEASE_LEVEL}>
	${TC_C_FLAGS_ARM_INSTRUCTIONSET}
	-mfloat-abi=${TC_C_FLAGS_FLOAT_ABI}
)

add_compile_options(
	${COMPILE_OPTIONS}
)

include_directories(BEFORE
	${TC_TOOLCHAIN_INCLUDE_PATH}
)

if (${TC_C_FLAGS_FLOAT_ABI} STREQUAL "hard")
	if(NOT ${TC_C_FLAGS_FLOAT_FPU} STREQUAL "")
		add_compile_options(-mfpu=${TC_C_FLAGS_FLOAT_FPU})
		add_link_options(-mfpu=${TC_C_FLAGS_FLOAT_FPU})
	endif()
endif()

if(NOT ${TC_C_FLAGS_ARM_FAMILY} STREQUAL "")
	add_compile_options(-mcpu=${TC_C_FLAGS_ARM_FAMILY})
	add_link_options(-mcpu=${TC_C_FLAGS_ARM_FAMILY})
endif()

if (NOT ${TC_C_FLAGS_MARCH} STREQUAL "")
	add_compile_options(-march=${TC_C_FLAGS_MARCH})
	add_link_options(-march=${TC_C_FLAGS_MARCH})
endif()

add_link_options (
	-fdata-sections
	-ffunction-sections
	$<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
	$<$<BOOL:${TC_L_FLAGS_NANO_SPECS}>:--specs=nano.specs>
	$<$<BOOL:${TC_L_FLAGS_NOSYS_SPECS}>:--specs=nosys.specs>
	$<$<BOOL:${TC_L_FLAGS_NOSTARTFILES}>:-nostartfiles>
	$<$<BOOL:${TC_L_FLAGS_NODEFAULTLIBS}>:-nodefaultlibs>
	$<$<BOOL:${TC_L_FLAGS_NOSTARTDEFAULTLIBS}>:-nostdlib>
	$<$<BOOL:${TC_L_FLAGS_REM_UNUSED_SECTIONS}>:-Xlinker>
	$<$<BOOL:${TC_L_FLAGS_REM_UNUSED_SECTIONS}>:--gc-sections>
	-mfloat-abi=${TC_C_FLAGS_FLOAT_ABI}

)

set(CMAKE_AR                        ${TC_TOOLCHAIN_BINARY_PATH}arm-none-eabi-ar${CMAKE_EXECUTABLE_SUFFIX})
set(CMAKE_ASM_COMPILER              ${TC_TOOLCHAIN_BINARY_PATH}arm-none-eabi-gcc${CMAKE_EXECUTABLE_SUFFIX})
set(CMAKE_C_COMPILER                ${TC_TOOLCHAIN_BINARY_PATH}arm-none-eabi-gcc${CMAKE_EXECUTABLE_SUFFIX})
set(CMAKE_CXX_COMPILER              ${TC_TOOLCHAIN_BINARY_PATH}arm-none-eabi-gcc${CMAKE_EXECUTABLE_SUFFIX})
set(CMAKE_LINKER                    ${TC_TOOLCHAIN_BINARY_PATH}arm-none-eabi-gcc${CMAKE_EXECUTABLE_SUFFIX})
set(CMAKE_OBJCOPY                   ${TC_TOOLCHAIN_BINARY_PATH}arm-none-eabi-objcopy${CMAKE_EXECUTABLE_SUFFIX} CACHE INTERNAL "")
set(CMAKE_RANLIB                    ${TC_TOOLCHAIN_BINARY_PATH}arm-none-eabi-ranlib${CMAKE_EXECUTABLE_SUFFIX} CACHE INTERNAL "")
set(CMAKE_SIZE                      ${TC_TOOLCHAIN_BINARY_PATH}arm-none-eabi-size${CMAKE_EXECUTABLE_SUFFIX} CACHE INTERNAL "")
set(CMAKE_STRIP                     ${TC_TOOLCHAIN_BINARY_PATH}arm-none-eabi-strip${CMAKE_EXECUTABLE_SUFFIX} CACHE INTERNAL "")

# set(CMAKE_C_FLAGS                   "-Wno-psabi --specs=nano.specs --specs=nosys.specs -fdata-sections -ffunction-sections -Wl,--gc-sections" CACHE INTERNAL "")
# set(CMAKE_CXX_FLAGS                 "${CMAKE_C_FLAGS} -fno-exceptions" CACHE INTERNAL "")

set(CMAKE_C_FLAGS_DEBUG             "-DDEBUG" CACHE INTERNAL "")
set(CMAKE_C_FLAGS_RELEASE           "-DNDEBUG" CACHE INTERNAL "")
set(CMAKE_CXX_FLAGS_DEBUG           "${CMAKE_C_FLAGS_DEBUG}" CACHE INTERNAL "")
set(CMAKE_CXX_FLAGS_RELEASE         "${CMAKE_C_FLAGS_RELEASE}" CACHE INTERNAL "")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH) #NEVER
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)	#ONLY
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)	#ONLY
