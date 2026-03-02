# RTOS CMakeLists.txt

if(NOT DEFINED CPU_TYPE)
    set(CPU_TYPE "cortex-m4")
endif()

if(CPU_TYPE STREQUAL "cortex-m4")
    set(RTOS_PORT_DIR "ARM_CM4F")
    set(RTOS_PORT_SRC "port.c")
    set(ARM_MATH_DEF "ARM_MATH_CM4")
elseif(CPU_TYPE STREQUAL "cortex-m3")
    set(RTOS_PORT_DIR "ARM_CM3")
    set(RTOS_PORT_SRC "port.c")
    set(ARM_MATH_DEF "ARM_MATH_CM3")
elseif(CPU_TYPE STREQUAL "cortex-m0" OR CPU_TYPE STREQUAL "cortex-m0plus")
    set(RTOS_PORT_DIR "ARM_CM0")
    set(RTOS_PORT_SRC "port.c")
    set(ARM_MATH_DEF "ARM_MATH_CM0")
else()
    message(WARNING "Unknown CPU_TYPE: ${CPU_TYPE}, defaulting to cortex-m4")
    set(RTOS_PORT_DIR "ARM_CM4F")
    set(RTOS_PORT_SRC "port.c")
    set(ARM_MATH_DEF "ARM_MATH_CM4")
endif()

message(STATUS "RTOS Port: ${RTOS_PORT_DIR}")

set(RTOS_INC_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS
    ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/port/GCC/${RTOS_PORT_DIR}
    ${CMAKE_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/Source/Common
    ${CMAKE_SOURCE_DIR}/Source/Interface
)

set(RTOS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/src/croutine.c
    ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/src/event_groups.c
    ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/src/list.c
    ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/src/queue.c
    ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/src/stream_buffer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/src/tasks.c
    ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/src/timers.c
    ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/port/GCC/${RTOS_PORT_DIR}/${RTOS_PORT_SRC}
    ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/port/MemMang/heap_4.c
)

add_library(rtos STATIC
    ${RTOS_SOURCES}
)

# 生成 Keil MDK 兼容的 .lib 文件
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/rtos.lib
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_BINARY_DIR}/librtos.a
        ${CMAKE_BINARY_DIR}/rtos.lib
    DEPENDS rtos
    COMMENT "Generating Keil MDK compatible rtos.lib"
    VERBATIM
)

add_custom_target(rtos.lib DEPENDS rtos)

target_compile_definitions(rtos PRIVATE
    FREERTOS
    ${ARM_MATH_DEF}
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
