# Application CMakeLists.txt

enable_language(CXX)

set(APP_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_SOURCE_DIR}/Source/Common/syscalls.c
    ${STARTUP_FILE}
)

add_executable(app.elf EXCLUDE_FROM_ALL
    ${APP_SOURCES}
)

# target_include_directories(app.elf PRIVATE
#     ${CMAKE_SOURCE_DIR}/Source
#     ${CMAKE_SOURCE_DIR}/Source/Common
#     ${CMAKE_SOURCE_DIR}/Source/Interface
#     ${CMAKE_SOURCE_DIR}/Source/OSAL
#     ${CMAKE_SOURCE_DIR}/Source/HAL
#     ${CMAKE_SOURCE_DIR}/Source/HAL/GPIO
#     ${CMAKE_SOURCE_DIR}/Source/HAL/UART
#     ${CMAKE_CURRENT_SOURCE_DIR}
# )

target_link_libraries(app.elf
    interface-driver
    interface-rtos
    osal
    hal-uart
    hal-gpio
    log-service
)

target_compile_features(app.elf PRIVATE cxx_std_17)

target_link_options(app.elf PRIVATE
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/app.map
    -Wl,--gc-sections
    -Wl,--print-memory-usage
    --specs=nano.specs
)

add_custom_command(
    OUTPUT app.hex
    COMMAND ${CMAKE_OBJCOPY} -O ihex app.elf app.hex
    DEPENDS app.elf
    COMMENT "Generating app HEX file..."
)
add_custom_target(app DEPENDS app.hex)

add_custom_command(TARGET app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo $<TARGET_FILE:app.elf> > ${CMAKE_BINARY_DIR}/firmware_path.txt
    COMMAND ${CMAKE_COMMAND} -E echo "Generated: ${CMAKE_CURRENT_BINARY_DIR}/app.hex"
    COMMAND ${CMAKE_COMMAND} -E echo "--- Firmware Size ---"
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:app.elf>
    COMMAND ${CMAKE_COMMAND} -E echo "---------------------"
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/build/flash
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:app.elf> ${CMAKE_SOURCE_DIR}/build/flash/flash.elf
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/app.hex ${CMAKE_SOURCE_DIR}/build/flash/flash.hex
)
set_target_properties(app PROPERTIES EXCLUDE_FROM_ALL TRUE)
