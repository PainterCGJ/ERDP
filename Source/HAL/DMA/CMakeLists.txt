# HAL GPIO CMakeLists.txt

set(HAL_DMA_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/erdp_hal_dma.cpp
)

add_library(hal-dma STATIC EXCLUDE_FROM_ALL
    ${HAL_DMA_SOURCES}
)

target_include_directories(hal-dma PUBLIC
    ${CMAKE_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/Source/Common
    ${CMAKE_SOURCE_DIR}/Source/Interface
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(hal-dma PUBLIC
    interface-driver
    osal
)

set_target_properties(hal-dma PROPERTIES
    LINKER_LANGUAGE CXX
)

target_compile_features(hal-dma PUBLIC cxx_std_17)
