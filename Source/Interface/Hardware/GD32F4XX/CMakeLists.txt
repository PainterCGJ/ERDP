# GD32F4XX Interface Hardware CMakeLists.txt

set(GD32F4XX_IF_INC_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_SOURCE_DIR}/Source/Interface
    ${CMAKE_SOURCE_DIR}/Source/Common
)

set(GD32F4XX_IF_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/erdp_if_gpio.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/erdp_if_exti.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/erdp_if_uart.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/erdp_if_spi.c
)

add_library(interface STATIC
    ${GD32F4XX_IF_SOURCES}
)

target_include_directories(interface PUBLIC
    ${GD32F4XX_IF_INC_DIRS}
)

target_link_libraries(interface PUBLIC
    driver-lib
)

target_compile_options(interface PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

target_link_options(interface PRIVATE
    -Wl,--print-memory-usage
)

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/interface.lib
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_BINARY_DIR}/libinterface.a
        ${CMAKE_BINARY_DIR}/interface.lib
    DEPENDS interface
    COMMENT "Generating Keil MDK compatible interface.lib"
    VERBATIM
)

add_custom_target(interface.lib ALL DEPENDS ${CMAKE_BINARY_DIR}/interface.lib)

# Interface Test
set(INTERFACE_TEST_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/erdp_if_test.c
    ${CMAKE_SOURCE_DIR}/Source/Kernel/Drivers/GD32F4XX/CMSIS/GD/GD32F4xx/Source/GCC/startup_gd32f450_470.s
)

add_executable(interface-test.elf ${INTERFACE_TEST_SOURCE})

target_include_directories(interface-test.elf PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_SOURCE_DIR}/Source/Interface
)

target_link_libraries(interface-test.elf
    interface
    driver-lib
)

target_compile_options(interface-test.elf PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

add_custom_command(
    OUTPUT interface-test.hex
    COMMAND ${CMAKE_OBJCOPY} -O ihex interface-test.elf interface-test.hex
    DEPENDS interface-test.elf
    COMMENT "Generating interface-test HEX file..."
)
add_custom_target(interface-test DEPENDS interface-test.hex)

# 在构建后记录固件路径
add_custom_command(TARGET interface-test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo $<TARGET_FILE:interface-test.elf> > ${CMAKE_BINARY_DIR}/firmware_path.txt
    COMMAND ${CMAKE_COMMAND} -E echo "Generated: ${CMAKE_CURRENT_BINARY_DIR}/interface-test.hex"
)

# 移除 ALL，使目标不会在构建 all 时自动编译
set_target_properties(interface-test PROPERTIES EXCLUDE_FROM_ALL TRUE)
