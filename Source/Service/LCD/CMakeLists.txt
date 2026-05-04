set(LCD_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/lcd_service.cpp
)

add_library(lcd-service STATIC EXCLUDE_FROM_ALL
    ${LCD_SOURCE}
)

target_include_directories(lcd-service PUBLIC
    ${CMAKE_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/Source/Common
    ${CMAKE_SOURCE_DIR}/Source/Board
    
    # ${CMAKE_SOURCE_DIR}/Source/Library/st7789/port
    # ${CMAKE_SOURCE_DIR}/Source/Library/st7789/src
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(lcd-service
    osal
    hal
    log-service
    lvgl
)

set_target_properties(lcd-service PROPERTIES
    LINKER_LANGUAGE CXX
)
