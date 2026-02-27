# STM32F4XX Interface Hardware CMakeLists.txt

set(STM32F4XX_IF_INC_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_SOURCE_DIR}/Source/Interface
    ${CMAKE_SOURCE_DIR}/Source/Common
)

set(STM32F4XX_IF_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/erdp_if_gpio.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/src/erdp_if_exti.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/src/erdp_if_uart.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/src/erdp_if_spi.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/src/erdp_if_i2c.c
)

add_library(interface STATIC
    ${STM32F4XX_IF_SOURCES}
)

target_include_directories(interface PUBLIC
    ${STM32F4XX_IF_INC_DIRS}
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
    ${CMAKE_SOURCE_DIR}/Source/Kernel/Drivers/STM32F4XX/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc_ride7/startup_stm32f40_41xxx.s
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

add_custom_command(
    OUTPUT interface-test.hex
    COMMAND ${CMAKE_OBJCOPY} -O ihex interface-test.elf interface-test.hex
    DEPENDS interface-test.elf
    COMMENT "Generating interface-test HEX file..."
)
add_custom_target(interface-test ALL DEPENDS interface-test.hex)
