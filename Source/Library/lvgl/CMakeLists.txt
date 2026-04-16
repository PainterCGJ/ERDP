cmake_minimum_required(VERSION 3.16)

################################################################################
# LVGL - Light and Versatile Graphics Library
# https://lvgl.io
# https://github.com/lvgl/lvgl
################################################################################

project(lvgl)

set(LVGL_VERSION "9.1.0")

# Options
option(LVGL_BUILD_STATIC "Build as static library" ON)

# Source directories to include
# Core (required)
set(LVGL_CORE_DIRS
    core
    misc
    draw
    display
    indev
    widgets
    layouts
    font
    themes
    tick
)

# Stdlib (required - choose implementation)
set(LVGL_STDLIB_DIRS
    stdlib/builtin
)

# OSAL (required - choose implementation for FreeRTOS)
set(LVGL_OSAL_DIRS
    osal
)

# Drivers (optional - select based on hardware)
# Uncomment or modify as needed for your platform
set(LVGL_DRIVER_DIRS
    # drivers/display/st_ltdc      # STM32 LTDC display controller
    # drivers/display/ili9341     # ILI9341 SPI LCD
    # drivers/display/st7789    # ST7789 SPI LCD
    # drivers/display/st7735    # ST7735 SPI LCD
    # drivers/display/lcd       # Generic MIPI LCD
)

# Libraries (optional)
set(LVGL_LIBS_DIRS
    libs/bin_decoder
    libs/bmp
    # libs/freetype            # FreeType font support (requires external library)
    # libs/gif                 # GIF image support
    # libs/lodepng             # PNG image support
    # libs/libpng              # PNG support (requires external library)
    # libs/libjpeg_turbo       # JPEG support (requires external library)
    # libs/libwebp             # WebP support (requires external library)
    # libs/barcode             # Barcode generation
    # libs/ffmpeg              # Video playback
)

# Extra widgets (optional)
# set(LVGL_EXTRA_DIRS
#     extra
# )

# Others (optional)
set(LVGL_OTHERS_DIRS
    others/file_explorer
    others/fragment
    others/translation
)

# Debugging (optional - disable for production)
set(LVGL_DEBUG_DIRS
    # debugging/test           # Unit tests
    # debugging/monkey         # Monkey testing
    # debugging/sysmon         # System monitor
)

# Collect all source files
set(LVGL_SOURCES)

# Add core directories
foreach(dir ${LVGL_CORE_DIRS})
    file(GLOB_RECURSE sources "${CMAKE_CURRENT_SOURCE_DIR}/src/${dir}/*.c")
    list(APPEND LVGL_SOURCES ${sources})
endforeach()

# Add stdlib
foreach(dir ${LVGL_STDLIB_DIRS})
    file(GLOB_RECURSE sources "${CMAKE_CURRENT_SOURCE_DIR}/src/stdlib/${dir}/*.c")
    list(APPEND LVGL_SOURCES ${sources})
endforeach()

# Add osal
foreach(dir ${LVGL_OSAL_DIRS})
    file(GLOB_RECURSE sources "${CMAKE_CURRENT_SOURCE_DIR}/src/osal/*.c")
    list(APPEND LVGL_SOURCES ${sources})
endforeach()

# Add drivers
foreach(dir ${LVGL_DRIVER_DIRS})
    if(NOT "${dir}" STREQUAL "")
        file(GLOB_RECURSE sources "${CMAKE_CURRENT_SOURCE_DIR}/src/${dir}/*.c")
        list(APPEND LVGL_SOURCES ${sources})
    endif()
endforeach()

# Add libraries
foreach(dir ${LVGL_LIBS_DIRS})
    if(NOT "${dir}" STREQUAL "")
        file(GLOB_RECURSE sources "${CMAKE_CURRENT_SOURCE_DIR}/src/libs/${dir}/*.c")
        list(APPEND LVGL_SOURCES ${sources})
    endif()
endforeach()

# Add extra
# foreach(dir ${LVGL_EXTRA_DIRS})
#     if(NOT "${dir}" STREQUAL "")
#         file(GLOB_RECURSE sources "${CMAKE_CURRENT_SOURCE_DIR}/src/${dir}/*.c")
#         list(APPEND LVGL_SOURCES ${sources})
#     endif()
# endforeach()

# Add others
foreach(dir ${LVGL_OTHERS_DIRS})
    if(NOT "${dir}" STREQUAL "")
        file(GLOB_RECURSE sources "${CMAKE_CURRENT_SOURCE_DIR}/src/${dir}/*.c")
        list(APPEND LVGL_SOURCES ${sources})
    endif()
endforeach()

# Add debugging
foreach(dir ${LVGL_DEBUG_DIRS})
    if(NOT "${dir}" STREQUAL "")
        file(GLOB_RECURSE sources "${CMAKE_CURRENT_SOURCE_DIR}/src/${dir}/*.c")
        list(APPEND LVGL_SOURCES ${sources})
    endif()
endforeach()

# Include directories
set(LVGL_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/misc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/draw
    ${CMAKE_CURRENT_SOURCE_DIR}/src/draw/sw
    ${CMAKE_CURRENT_SOURCE_DIR}/src/draw/sw/blend
    ${CMAKE_CURRENT_SOURCE_DIR}/src/display
    ${CMAKE_CURRENT_SOURCE_DIR}/src/indev
    ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets
    ${CMAKE_CURRENT_SOURCE_DIR}/src/layouts
    ${CMAKE_CURRENT_SOURCE_DIR}/src/layouts/flex
    ${CMAKE_CURRENT_SOURCE_DIR}/src/layouts/grid
    ${CMAKE_CURRENT_SOURCE_DIR}/src/font
    ${CMAKE_CURRENT_SOURCE_DIR}/src/font/fmt_txt
    ${CMAKE_CURRENT_SOURCE_DIR}/src/font/font_manager
    ${CMAKE_CURRENT_SOURCE_DIR}/src/font/imgfont
    ${CMAKE_CURRENT_SOURCE_DIR}/src/font/binfont_loader
    ${CMAKE_CURRENT_SOURCE_DIR}/src/themes
    ${CMAKE_CURRENT_SOURCE_DIR}/src/themes/default
    ${CMAKE_CURRENT_SOURCE_DIR}/src/themes/simple
    ${CMAKE_CURRENT_SOURCE_DIR}/src/themes/mono
    ${CMAKE_CURRENT_SOURCE_DIR}/src/stdlib
    ${CMAKE_CURRENT_SOURCE_DIR}/src/stdlib/builtin
    ${CMAKE_CURRENT_SOURCE_DIR}/src/osal
    ${CMAKE_CURRENT_SOURCE_DIR}/src/tick
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Add driver include directories
foreach(dir ${LVGL_DRIVER_DIRS})
    if(NOT "${dir}" STREQUAL "")
        list(APPEND LVGL_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/src/${dir}")
    endif()
endforeach()

# Add library include directories
foreach(dir ${LVGL_LIBS_DIRS})
    if(NOT "${dir}" STREQUAL "")
        list(APPEND LVGL_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/src/libs/${dir}")
    endif()
endforeach()

# Add extra include directories
# foreach(dir ${LVGL_EXTRA_DIRS})
#     if(NOT "${dir}" STREQUAL "")
#         list(APPEND LVGL_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/src/${dir}")
#     endif()
# endforeach()

# Add others include directories
foreach(dir ${LVGL_OTHERS_DIRS})
    if(NOT "${dir}" STREQUAL "")
        list(APPEND LVGL_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/src/${dir}")
    endif()
endforeach()

# Add debugging include directories
foreach(dir ${LVGL_DEBUG_DIRS})
    if(NOT "${dir}" STREQUAL "")
        list(APPEND LVGL_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/src/${dir}")
    endif()
endforeach()

# Remove duplicates
if(CMAKE_VERSION VERSION_GREATER "3.21")
    list(REMOVE_DUPLICATES LVGL_INCLUDE_DIRS)
endif()

# Create static library
add_library(lvgl STATIC ${LVGL_SOURCES})

# Set include directories
target_include_directories(lvgl PUBLIC ${LVGL_INCLUDE_DIRS})

# Set compile definitions
target_compile_definitions(lvgl PUBLIC
    LV_LVGL_H_INCLUDE_SIMPLE
    LV_CONF_INCLUDE_SIMPLE
)

# Set C standard
target_compile_features(lvgl PUBLIC c_std_99)

# Set optimization options for release builds
target_compile_options(lvgl PRIVATE
    $<$<CONFIG:Release>:-O2>
)

# Link FreeRTOS (or other RTOS)
# Uncomment and modify based on your RTOS
# target_link_libraries(lvgl PRIVATE
#     freertos
#     # Add other dependencies as needed
# )

# Set library output name
set_target_properties(lvgl PROPERTIES
    OUTPUT_NAME "lvgl"
    POSITION_INDEPENDENT_CODE OFF
)

# Export compile commands for IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

message(STATUS "LVGL ${LVGL_VERSION} configuration:")
message(STATUS "  Source files: ${LVGL_SOURCES}")
message(STATUS "  Include dirs: ${LVGL_INCLUDE_DIRS}")
