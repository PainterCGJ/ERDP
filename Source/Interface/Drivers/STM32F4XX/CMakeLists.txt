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

add_library(interface-driver STATIC
    ${STM32F4XX_IF_SOURCES}
)

target_include_directories(interface-driver PUBLIC
    ${STM32F4XX_IF_INC_DIRS}
)

target_link_libraries(interface-driver PUBLIC
    driver
)

target_compile_options(interface-driver PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

target_link_options(interface-driver PRIVATE
    -Wl,--print-memory-usage
)

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/interface-driver.lib
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_BINARY_DIR}/libinterface-driver.a
        ${CMAKE_BINARY_DIR}/interface-driver.lib
    DEPENDS interface-driver
    COMMENT "Generating Keil MDK compatible interface-driver.lib"
    VERBATIM
)

add_custom_target(interface-driver.lib DEPENDS ${CMAKE_BINARY_DIR}/interface-driver.lib)

# Interface Driver Test
set(INTERFACE_DRIVER_TEST_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/erdp_if_test.c
    ${CMAKE_SOURCE_DIR}/Source/Kernel/Drivers/STM32F4XX/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc_ride7/startup_stm32f40_41xxx.s
)

add_executable(interface-driver-test.elf ${INTERFACE_DRIVER_TEST_SOURCE})

target_include_directories(interface-driver-test.elf PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_SOURCE_DIR}/Source/Interface
)

target_link_libraries(interface-driver-test.elf
    interface-driver
    driver
)

add_custom_command(
    OUTPUT interface-driver-test.hex
    COMMAND ${CMAKE_OBJCOPY} -O ihex interface-driver-test.elf interface-driver-test.hex
    DEPENDS interface-driver-test.elf
    COMMENT "Generating interface-driver-test HEX file..."
)
add_custom_target(interface-driver-test DEPENDS interface-driver-test.hex)

# 在构建后记录固件路径
add_custom_command(TARGET interface-driver-test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo $<TARGET_FILE:interface-driver-test.elf> > ${CMAKE_BINARY_DIR}/firmware_path.txt
    COMMAND ${CMAKE_COMMAND} -E echo "Generated: ${CMAKE_CURRENT_BINARY_DIR}/interface-driver-test.hex"
)

# 移除 ALL，使目标不会在构建 all 时自动编译
set_target_properties(interface-driver-test PROPERTIES EXCLUDE_FROM_ALL TRUE)
