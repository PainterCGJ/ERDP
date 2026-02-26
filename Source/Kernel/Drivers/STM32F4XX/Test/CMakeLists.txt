# STM32F4XX Test CMakeLists.txt

set(DRIVER_TEST_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/driver_test.c
    ${CMAKE_SOURCE_DIR}/Source/Kernel/Drivers/STM32F4XX/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc_ride7/startup_stm32f40_41xxx.s
)

add_executable(driver-test.elf ${DRIVER_TEST_SOURCE})

target_include_directories(driver-test.elf PRIVATE
    ${CMAKE_SOURCE_DIR}/Source/Kernel/Drivers/STM32F4XX/CMSIS/Device/ST/STM32F4xx/Include
    ${CMAKE_SOURCE_DIR}/Source/Kernel/Drivers/STM32F4XX/CMSIS/Include
    ${CMAKE_SOURCE_DIR}/Source/Kernel/Drivers/STM32F4XX/STM32F4xx_StdPeriph_Driver/inc
)

target_compile_definitions(driver-test.elf PRIVATE
    STM32F40_41xxx
    USE_STDPERIPH_DRIVER
)

target_link_libraries(driver-test.elf driver-lib)

add_custom_command(
    OUTPUT driver-test.hex
    COMMAND ${CMAKE_OBJCOPY} -O ihex driver-test.elf driver-test.hex
    DEPENDS driver-test.elf
    COMMENT "Generating HEX file..."
)
add_custom_target(driver-test ALL DEPENDS driver-test.hex)
