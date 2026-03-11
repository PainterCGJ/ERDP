# Test CMakeLists.txt

# 根据芯片系列设置特定的包含路径和定义
if(CHIP_DIR STREQUAL "STM32F4XX")
    set(DRIVER_TEST_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/${CHIP_DIR}/driver_test.c)
    list(APPEND TEST_INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/${CHIP_DIR}
    )
elseif(CHIP_DIR STREQUAL "GD32F4XX")
    set(DRIVER_TEST_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/${CHIP_DIR}/driver_test.c)
    list(APPEND TEST_INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/${CHIP_DIR}
    )
else()
    message(WARNING "No test configuration for chip dir: ${CHIP_DIR}")
endif()

# -----------------------------------------------------------------------------
# HAL Test
# -----------------------------------------------------------------------------
set(HAL_TEST_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/erdp_hal_test.cpp
    ${CMAKE_SOURCE_DIR}/Source/Common/syscalls.c
    ${STARTUP_FILE}
)

add_executable(hal-test.elf EXCLUDE_FROM_ALL ${HAL_TEST_SOURCE})

target_include_directories(hal-test.elf PRIVATE 
    ${TEST_INCLUDE_DIRS}
)

target_link_libraries(hal-test.elf
    osal
    hal-gpio
    hal-uart
    interface-rtos
)

target_link_options(hal-test.elf PRIVATE
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/hal-test.map
    -Wl,--gc-sections
    -Wl,--print-memory-usage
    --specs=nano.specs
)

target_compile_options(hal-test.elf PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
)

target_compile_features(hal-test.elf PRIVATE cxx_std_17)

add_custom_command(
    OUTPUT hal-test.hex
    COMMAND ${CMAKE_OBJCOPY} -O ihex hal-test.elf hal-test.hex
    DEPENDS hal-test.elf
    COMMENT "Generating HAL test HEX file..."
)
add_custom_target(hal-test DEPENDS hal-test.hex)

add_custom_command(TARGET hal-test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo $<TARGET_FILE:hal-test.elf> > ${CMAKE_BINARY_DIR}/firmware_path.txt
    COMMAND ${CMAKE_COMMAND} -E echo "Generated: ${CMAKE_CURRENT_BINARY_DIR}/hal-test.hex"
    COMMAND ${CMAKE_COMMAND} -E echo "--- Firmware Size ---"
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:hal-test.elf>
    COMMAND ${CMAKE_COMMAND} -E echo "---------------------"
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/build/flash
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:hal-test.elf> ${CMAKE_SOURCE_DIR}/build/flash/flash.elf
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/hal-test.hex ${CMAKE_SOURCE_DIR}/build/flash/flash.hex
)
set_target_properties(hal-test PROPERTIES EXCLUDE_FROM_ALL TRUE)

# -----------------------------------------------------------------------------
# OSAL Test
# -----------------------------------------------------------------------------
set(OSAL_TEST_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/erdp_osal_test.cpp
    ${CMAKE_SOURCE_DIR}/Source/Common/syscalls.c
    ${STARTUP_FILE}
)

add_executable(osal-test.elf EXCLUDE_FROM_ALL ${OSAL_TEST_SOURCE})

target_include_directories(osal-test.elf PRIVATE 
    ${TEST_INCLUDE_DIRS}
)

target_compile_options(osal-test.elf PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
)

target_compile_features(osal-test.elf PRIVATE cxx_std_17)

target_link_libraries(osal-test.elf
    osal
)

target_link_options(osal-test.elf PRIVATE
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/osal-test.map
    -Wl,--gc-sections
    -Wl,--print-memory-usage
    --specs=nano.specs
)

add_custom_command(
    OUTPUT osal-test.hex
    COMMAND ${CMAKE_OBJCOPY} -O ihex osal-test.elf osal-test.hex
    DEPENDS osal-test.elf
    COMMENT "Generating OSAL test HEX file..."
)
add_custom_target(osal-test DEPENDS osal-test.hex)

add_custom_command(TARGET osal-test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo $<TARGET_FILE:osal-test.elf> > ${CMAKE_BINARY_DIR}/firmware_path.txt
    COMMAND ${CMAKE_COMMAND} -E echo "Generated: ${CMAKE_CURRENT_BINARY_DIR}/osal-test.hex"
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/build/flash
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:osal-test.elf> ${CMAKE_SOURCE_DIR}/build/flash/flash.elf
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/osal-test.hex ${CMAKE_SOURCE_DIR}/build/flash/flash.hex
)
set_target_properties(osal-test PROPERTIES EXCLUDE_FROM_ALL TRUE)

# -----------------------------------------------------------------------------
# Interface RTOS Test
# -----------------------------------------------------------------------------
set(INTERFACE_RTOS_TEST_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/erdp_if_rtos_test.c
    ${STARTUP_FILE}
    ${CMAKE_SOURCE_DIR}/Source/Common/syscalls.c
)

add_executable(interface-rtos-test.elf ${INTERFACE_RTOS_TEST_SOURCE})

target_include_directories(interface-rtos-test.elf PRIVATE ${TEST_INCLUDE_DIRS})

target_link_libraries(interface-rtos-test.elf
    interface-driver
    interface-rtos
)

target_link_options(interface-rtos-test.elf PRIVATE
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/interface-rtos-test.map
    -Wl,--gc-sections
    -Wl,--print-memory-usage
    --specs=nano.specs
)

add_custom_command(
    OUTPUT interface-rtos-test.hex
    COMMAND ${CMAKE_OBJCOPY} -O ihex interface-rtos-test.elf interface-rtos-test.hex
    DEPENDS interface-rtos-test.elf
    COMMENT "Generating interface-rtos-test HEX file..."
)
add_custom_target(interface-rtos-test ALL DEPENDS interface-rtos-test.hex)

add_custom_command(TARGET interface-rtos-test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo $<TARGET_FILE:interface-rtos-test.elf> > ${CMAKE_BINARY_DIR}/firmware_path.txt
    COMMAND ${CMAKE_COMMAND} -E echo "Generated: ${CMAKE_CURRENT_BINARY_DIR}/interface-rtos-test.hex"
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/build/flash
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:interface-rtos-test.elf> ${CMAKE_SOURCE_DIR}/build/flash/flash.elf
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/interface-rtos-test.hex ${CMAKE_SOURCE_DIR}/build/flash/flash.hex
)

# -----------------------------------------------------------------------------
# Interface Driver Test
# -----------------------------------------------------------------------------
set(INTERFACE_DRIVER_TEST_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/erdp_if_test.c
    ${STARTUP_FILE}
)

add_executable(interface-driver-test.elf ${INTERFACE_DRIVER_TEST_SOURCE})

target_include_directories(interface-driver-test.elf PRIVATE ${TEST_INCLUDE_DIRS})

target_link_libraries(interface-driver-test.elf
    interface-driver
    driver
)

target_link_options(interface-driver-test.elf PRIVATE
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/interface-driver-test.map
    -Wl,--gc-sections
    -Wl,--print-memory-usage
    --specs=nano.specs
)

add_custom_command(
    OUTPUT interface-driver-test.hex
    COMMAND ${CMAKE_OBJCOPY} -O ihex interface-driver-test.elf interface-driver-test.hex
    DEPENDS interface-driver-test.elf
    COMMENT "Generating interface-driver-test HEX file..."
)
add_custom_target(interface-driver-test DEPENDS interface-driver-test.hex)

add_custom_command(TARGET interface-driver-test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo $<TARGET_FILE:interface-driver-test.elf> > ${CMAKE_BINARY_DIR}/firmware_path.txt
    COMMAND ${CMAKE_COMMAND} -E echo "Generated: ${CMAKE_CURRENT_BINARY_DIR}/interface-driver-test.hex"
)
set_target_properties(interface-driver-test PROPERTIES EXCLUDE_FROM_ALL TRUE)

# -----------------------------------------------------------------------------
# Driver Test
# -----------------------------------------------------------------------------
if(EXISTS ${DRIVER_TEST_SOURCE})
    set(DRIVER_TEST_SOURCES
        ${DRIVER_TEST_SOURCE}
        ${STARTUP_FILE}
    )

    add_executable(driver-test.elf ${DRIVER_TEST_SOURCES})

    target_include_directories(driver-test.elf PRIVATE ${TEST_INCLUDE_DIRS})

    target_link_libraries(driver-test.elf driver)

    target_link_options(driver-test.elf PRIVATE
        -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/driver-test.map
        -Wl,--gc-sections
        -Wl,--print-memory-usage
        --specs=nano.specs
    )

    add_custom_command(
        OUTPUT driver-test.hex
        COMMAND ${CMAKE_OBJCOPY} -O ihex driver-test.elf driver-test.hex
        DEPENDS driver-test.elf
        COMMENT "Generating driver test HEX file..."
    )
    add_custom_target(driver-test DEPENDS driver-test.hex)

    add_custom_command(TARGET driver-test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo $<TARGET_FILE:driver-test.elf> > ${CMAKE_BINARY_DIR}/firmware_path.txt
        COMMAND ${CMAKE_COMMAND} -E echo "Generated: ${CMAKE_CURRENT_BINARY_DIR}/driver-test.hex"
    )
    set_target_properties(driver-test PROPERTIES EXCLUDE_FROM_ALL TRUE)
endif()
