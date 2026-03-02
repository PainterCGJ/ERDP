# Source/Interface/RTOS/FreeRTOS/CMakeLists.txt

set(RTOS_IF_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/erdp_if_rtos.c
)

add_library(interface-rtos STATIC
    ${RTOS_IF_SOURCES}
)

target_include_directories(interface-rtos PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/Source/Interface
    ${CMAKE_SOURCE_DIR}/Source/Kernel/RTOS/FreeRTOS/inc
    ${CMAKE_SOURCE_DIR}/Source/Kernel/RTOS/FreeRTOS
)

if(CPU_TYPE STREQUAL "cortex-m4")
    target_include_directories(interface-rtos PUBLIC
        ${CMAKE_SOURCE_DIR}/Source/Kernel/RTOS/FreeRTOS/port/GCC/ARM_CM4F
    )
elseif(CPU_TYPE STREQUAL "cortex-m3")
    target_include_directories(interface-rtos PUBLIC
        ${CMAKE_SOURCE_DIR}/Source/Kernel/RTOS/FreeRTOS/port/GCC/ARM_CM3
    )
endif()

target_compile_definitions(interface-rtos PUBLIC
    FREERTOS
)

target_link_libraries(interface-rtos PUBLIC
    interface-driver
)

if(CPU_TYPE STREQUAL "cortex-m4")
    target_compile_definitions(interface-rtos PUBLIC
        ARM_MATH_CM4
    )
elseif(CPU_TYPE STREQUAL "cortex-m3")
    target_compile_definitions(interface-rtos PUBLIC
        ARM_MATH_CM3
    )
endif()
