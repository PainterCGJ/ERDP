# Source/Interface/Hardware/CMakeLists.txt

if(CHIP_TYPE MATCHES "STM32F4.*")
    add_subdirectory(STM32F4XX)
elseif(CHIP_TYPE MATCHES "GD32F4.*")
    add_subdirectory(GD32F4XX)
else()
    message(FATAL_ERROR "Unsupported chip type for Interface: ${CHIP_TYPE}")
endif()
