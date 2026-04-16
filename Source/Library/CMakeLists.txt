add_subdirectory(st7789)
add_subdirectory(lvgl)
add_library(library INTERFACE)

target_link_libraries(library INTERFACE 
    #st7789
    lvgl
    )
