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
    ${CMAKE_CURRENT_SOURCE_DIR}/src/erdp_if_i2c.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/erdp_if_sys.c
)

add_library(interface-driver STATIC
    ${GD32F4XX_IF_SOURCES}
)

target_include_directories(interface-driver PUBLIC
    ${GD32F4XX_IF_INC_DIRS}
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
