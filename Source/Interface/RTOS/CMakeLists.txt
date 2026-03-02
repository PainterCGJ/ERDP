# Source/Interface/RTOS/CMakeLists.txt

if(RTOS_TYPE STREQUAL "FREERTOS")
    add_subdirectory(FreeRTOS)
else()
    message(WARNING "Unknown RTOS type: ${RTOS_TYPE}")
endif()
