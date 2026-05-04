cmake_minimum_required(VERSION 3.16)

################################################################################
# LVGL - Light and Versatile Graphics Library
# https://lvgl.io
# https://github.com/lvgl/lvgl
################################################################################

project(lvgl)

set(LVGL_VERSION "9.1.0")

option(LVGL_BUILD_STATIC "Build as static library" ON)

# Collect all source files from src directory
# This adds ALL source files, LVGL's internal #if/#ifdef will control what gets compiled
set(LVGL_SOURCES)

# Add ALL src subdirectories - let lv_conf.h control what's actually compiled
# LVGL uses LV_USE_* macros to conditionally include code
set(LVGL_ALL_DIRS
    core
    misc
    draw
    draw/convert
    draw/sw
    draw/sw/blend
    draw/sw/blend/neon
    draw/sw/blend/riscv_v
    draw/dma2d
    draw/espressif/ppa
    draw/eve
    draw/nanovg
    draw/nema_gfx
    draw/nxp/g2d
    draw/nxp/pxp
    draw/opengles
    draw/sdl
    draw/snapshot
    draw/vg_lite
    draw/renesas/dave2d
    display
    indev
    widgets
    layouts
    layouts/flex
    layouts/grid
    font
    font/fmt_txt
    font/font_manager
    font/imgfont
    font/binfont_loader
    themes
    themes/default
    themes/simple
    themes/mono
    tick
    stdlib
    stdlib/builtin
    stdlib/clib
    stdlib/micropython
    stdlib/rtthread
    stdlib/uefi
    osal
    libs
    libs/bin_decoder
    libs/bmp
    libs/freetype
    libs/gif
    libs/lodepng
    libs/libpng
    libs/libjpeg_turbo
    libs/libwebp
    libs/barcode
    libs/ffmpeg
    libs/gltf
    libs/tiny_ttf
    libs/qrcode
    libs/rlottie
    others
    others/file_explorer
    others/fragment
    others/translation

    # Drivers
    drivers
    drivers/display
    drivers/display/drm
    drivers/display/fb
    drivers/display/ft81x
    drivers/display/ili9341
    drivers/display/lcd
    drivers/display/lovyan_gfx
    drivers/display/nv3007
    drivers/display/nxp_elcdif
    drivers/display/renesas_glcdc
    drivers/display/st7735
    drivers/display/st7789
    drivers/display/st7796
    drivers/display/st_ltdc
    drivers/display/tft_espi
    drivers/evdev
    drivers/libinput
    drivers/nuttx
    drivers/nuttx/mock
    drivers/opengles
    drivers/opengles/glad
    drivers/qnx
    drivers/sdl
    drivers/uefi
    drivers/wayland

    debugging
    debugging/test
    debugging/monkey
    debugging/sysmon
    debugging/vg_lite_tvg
)

# Add all source files from listed directories
foreach(dir ${LVGL_ALL_DIRS})
    set(target_path "${CMAKE_CURRENT_SOURCE_DIR}/src/${dir}")
    if(EXISTS "${target_path}")
        file(GLOB_RECURSE sources "${target_path}/*.c")
        list(APPEND LVGL_SOURCES ${sources})
    endif()
endforeach()

# Add root level .c files
file(GLOB root_sources "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
list(APPEND LVGL_SOURCES ${root_sources})

# Include directories - add all possible directories
# LVGL's internal #ifdef will handle conditional compilation
set(LVGL_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Add all subdirectories as include paths
foreach(dir ${LVGL_ALL_DIRS})
    set(target_path "${CMAKE_CURRENT_SOURCE_DIR}/src/${dir}")
    if(EXISTS "${target_path}")
        list(APPEND LVGL_INCLUDE_DIRS "${target_path}")
    endif()
endforeach()

# Remove duplicates
if(CMAKE_VERSION VERSION_GREATER "3.21")
    list(REMOVE_DUPLICATES LVGL_INCLUDE_DIRS)
endif()

# Create static library
add_library(lvgl STATIC ${LVGL_SOURCES})

target_link_libraries(lvgl PRIVATE
    rtos
)

target_include_directories(lvgl PUBLIC ${LVGL_INCLUDE_DIRS})

target_compile_definitions(lvgl PUBLIC
    LV_LVGL_H_INCLUDE_SIMPLE
    LV_CONF_INCLUDE_SIMPLE
)

target_compile_features(lvgl PUBLIC c_std_99)

target_compile_options(lvgl PRIVATE
    $<$<CONFIG:Release>:-O2>
)

set_target_properties(lvgl PROPERTIES
    OUTPUT_NAME "lvgl"
    POSITION_INDEPENDENT_CODE OFF
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

message(STATUS "LVGL ${LVGL_VERSION} - Building all components")
message(STATUS "  Source files: ${LVGL_SOURCES}")
message(STATUS "  Include dirs: ${LVGL_INCLUDE_DIRS}")
