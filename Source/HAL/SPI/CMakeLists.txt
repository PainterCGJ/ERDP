# HAL GPIO CMakeLists.txt

enable_language(CXX)

set(HAL_SPI_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/erdp_hal_spi.cpp
)

add_library(hal-spi STATIC EXCLUDE_FROM_ALL
    ${HAL_SPI_SOURCES}
)

target_include_directories(hal-spi PUBLIC
    ${CMAKE_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/Source/Common
    ${CMAKE_SOURCE_DIR}/Source/Interface
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(hal-spi PUBLIC
    interface-driver
    osal
    hal-dma
)

set_target_properties(hal-spi PROPERTIES
    LINKER_LANGUAGE CXX
)

target_compile_features(hal-spi PUBLIC cxx_std_17)
