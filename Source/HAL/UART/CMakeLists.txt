# HAL GPIO CMakeLists.txt

enable_language(CXX)

set(HAL_UART_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/erdp_hal_uart.cpp
)

add_library(hal-uart STATIC EXCLUDE_FROM_ALL
    ${HAL_UART_SOURCES}
)

target_include_directories(hal-uart PUBLIC
    ${CMAKE_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/Source/Common
    ${CMAKE_SOURCE_DIR}/Source/Interface
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(hal-uart PUBLIC
    interface-driver
    osal
)

set_target_properties(hal-uart PROPERTIES
    LINKER_LANGUAGE CXX
)

target_compile_features(hal-uart PUBLIC cxx_std_17)
