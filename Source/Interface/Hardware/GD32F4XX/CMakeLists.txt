# GD32F4XX Interface Hardware CMakeLists.txt

set(GD32F4XX_IF_INC_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_SOURCE_DIR}/Source/Interface
    ${CMAKE_SOURCE_DIR}/Source/Common
    ${CMAKE_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/Source/Kernel/Drivers/GD32F4XX/CMSIS/GD/GD32F4xx/Include
    ${CMAKE_SOURCE_DIR}/Source/Kernel/Drivers/GD32F4XX/CMSIS
    ${CMAKE_SOURCE_DIR}/Source/Kernel/Drivers/GD32F4XX/GD32F4xx_standard_peripheral/Include
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

target_compile_definitions(interface PRIVATE
    GD32F4XX
    GD32F470
    USE_STDPERIPH_DRIVER
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
