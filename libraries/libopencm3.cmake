message("Including libopencm3.cmake")

include (FetchContent)
find_package (Git REQUIRED)

FetchContent_Declare(libopencm3_build
	GIT_REPOSITORY https://github.com/libopencm3/libopencm3.git
	GIT_TAG master
)

FetchContent_MakeAvailable(libopencm3_build)
FetchContent_GetProperties(libopencm3_build)

set (LIBOPENCM3_LINKERSCRIPT "${libopencm3_build_SOURCE_DIR}/lib/cortex-m-generic.ld" CACHE FILEPATH "Path to libopencm3s default linker script")
set (LIBOPENCM3_FAMILY "" CACHE STRING "Libopencm3 family")

add_custom_target(libopencm3_clean
	COMMAND make -j32 clean 2>&1 > ${CMAKE_CURRENT_BINARY_DIR}/libopencm3_clean.log
	USES_TERMINAL
	WORKING_DIRECTORY ${libopencm3_build_SOURCE_DIR}
)

add_custom_target(libopencm3_build
	COMMAND make -j32 > ${CMAKE_CURRENT_BINARY_DIR}/libopencm3_build.log
	USES_TERMINAL
	WORKING_DIRECTORY ${libopencm3_build_SOURCE_DIR}
)

add_library(libopencm3 STATIC IMPORTED)

set_property( TARGET libopencm3
	PROPERTY
		INTERFACE_INCLUDE_DIRECTORIES ${libopencm3_build_SOURCE_DIR}/include/
)


if (NOT ${LIBOPENCM3_FAMILY} STREQUAL "")
	string(TOUPPER ${LIBOPENCM3_FAMILY} LIBOPENCM3_FAMILY_UPPER)
	string(TOLOWER ${LIBOPENCM3_FAMILY} LIBOPENCM3_FAMILY_LOWER)
	target_compile_definitions(libopencm3 INTERFACE -D${LIBOPENCM3_FAMILY_UPPER})
	set_property( TARGET libopencm3
		PROPERTY
			IMPORTED_LOCATION ${libopencm3_build_SOURCE_DIR}/lib/libopencm3_${LIBOPENCM3_FAMILY_LOWER}.a
	)
endif()

add_dependencies(libopencm3 libopencm3_build)

target_link_directories(libopencm3
	INTERFACE ${libopencm3_build_SOURCE_DIR}/lib/
)


