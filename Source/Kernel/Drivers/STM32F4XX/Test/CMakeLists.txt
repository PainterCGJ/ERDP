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

target_link_libraries(driver-test.elf driver)

add_custom_command(
    OUTPUT driver-test.hex
    COMMAND ${CMAKE_OBJCOPY} -O ihex driver-test.elf driver-test.hex
    DEPENDS driver-test.elf
    COMMENT "Generating HEX file..."
)
add_custom_target(driver-test DEPENDS driver-test.hex)

# 在构建后记录固件路径
add_custom_command(TARGET driver-test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo $<TARGET_FILE:driver-test.elf> > ${CMAKE_BINARY_DIR}/firmware_path.txt
    COMMAND ${CMAKE_COMMAND} -E echo "Generated: ${CMAKE_CURRENT_BINARY_DIR}/driver-test.hex"
)

# 移除 ALL，使目标不会在构建 all 时自动编译
set_target_properties(driver-test PROPERTIES EXCLUDE_FROM_ALL TRUE)
