# HAL CMakeLists.txt

add_subdirectory(GPIO)
add_subdirectory(UART)
add_subdirectory(SPI)

enable_language(CXX)

add_library(hal INTERFACE)

set_target_properties(hal PROPERTIES
    LINKER_LANGUAGE CXX
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

target_link_libraries(hal INTERFACE
    hal-uart
    hal-gpio
    hal-spi
)
