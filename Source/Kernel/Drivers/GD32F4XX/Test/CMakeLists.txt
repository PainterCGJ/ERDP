# GD32F4XX Test CMakeLists.txt

set(DRIVER_TEST_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/driver_test.c
)

add_executable(driver-test.elf ${DRIVER_TEST_SOURCE})
target_link_libraries(driver-test.elf driver-lib)

add_custom_command(
    OUTPUT driver-test.hex
    COMMAND ${CMAKE_OBJCOPY} -O ihex driver-test.elf driver-test.hex
    DEPENDS driver-test.elf
    COMMENT "Generating HEX file..."
)
add_custom_target(driver-test ALL DEPENDS driver-test.hex)
