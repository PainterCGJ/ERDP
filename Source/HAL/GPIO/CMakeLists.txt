# HAL GPIO CMakeLists.txt

set(HAL_GPIO_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/erdp_hal_gpio.cpp
)

add_library(hal-gpio STATIC EXCLUDE_FROM_ALL
    ${HAL_GPIO_SOURCES}
)

target_include_directories(hal-gpio PUBLIC
    ${CMAKE_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/Source/Common
    ${CMAKE_SOURCE_DIR}/Source/Interface
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(hal-gpio PUBLIC
    interface-driver
)

set_target_properties(hal-gpio PROPERTIES
    LINKER_LANGUAGE CXX
)

target_compile_features(hal-gpio PUBLIC cxx_std_17)
