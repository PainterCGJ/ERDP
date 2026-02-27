# RTOS CMakeLists.txt

set(RTOS_INC_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS
    ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/port/GCC/ARM_CM4F
    ${CMAKE_SOURCE_DIR}/Source/Library/printf
    ${CMAKE_SOURCE_DIR}/Source/Kernel/Drivers/GD32F4XX
    ${CMAKE_SOURCE_DIR}/Source/Kernel/Drivers/GD32F4XX/CMSIS/GD/GD32F4xx/Include
    ${CMAKE_SOURCE_DIR}/Source/Kernel/Drivers/GD32F4XX/CMSIS
    ${CMAKE_SOURCE_DIR}/Source/Kernel/Drivers/GD32F4XX/GD32F4xx_standard_peripheral/Include
    ${CMAKE_SOURCE_DIR}/Source
)

set(RTOS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/src/croutine.c
    ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/src/event_groups.c
    ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/src/list.c
    ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/src/queue.c
    ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/src/stream_buffer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/src/tasks.c
    ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/src/timers.c
    ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/port/GCC/ARM_CM4F/port.c
    ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/port/MemMang/heap_4.c
)

add_library(rtos STATIC
    ${RTOS_SOURCES}
)

target_compile_definitions(rtos PRIVATE
    FREERTOS
    __GNUC__
    ARM_MATH_CM4
    GD32F470
    USE_STDPERIPH_DRIVER
)

target_include_directories(rtos PUBLIC
    ${RTOS_INC_DIRS}
)

target_compile_options(rtos PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

target_link_options(rtos PRIVATE
    -Wl,--print-memory-usage
)

add_custom_target(rtos.lib ALL DEPENDS ${CMAKE_BINARY_DIR}/rtos.lib)

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/rtos.lib
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_BINARY_DIR}/librtos.a
        ${CMAKE_BINARY_DIR}/rtos.lib
    DEPENDS rtos
    COMMENT "Generating Keil MDK compatible rtos.lib"
    VERBATIM
)
