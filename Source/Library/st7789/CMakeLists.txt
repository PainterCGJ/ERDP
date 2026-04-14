enable_language(CXX)

set(ST7789_INC_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/port
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/Source/Board

)

set(ST7789_SOURCES
    ${CMAKE_}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/driver_st7789.c
    ${CMAKE_CURRENT_SOURCE_DIR}/port/driver_st7789_interface.cpp
)

add_library(st7789 STATIC
    ${ST7789_SOURCES}
)

target_include_directories(st7789 PUBLIC
    ${ST7789_INC_DIRS}
)

target_link_libraries(st7789 PUBLIC
    hal
    osal
    log-service
)

target_compile_features(st7789 PUBLIC cxx_std_17)
