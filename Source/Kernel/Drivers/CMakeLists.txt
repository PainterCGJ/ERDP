# Source/Kernel/Drivers/CMakeLists.txt

# 根据 CHIP_TYPE 选择不同的驱动库
if(CHIP_TYPE MATCHES "STM32F4.*")
    add_subdirectory(STM32F4XX)
elseif(CHIP_TYPE MATCHES "GD32F4.*")
    add_subdirectory(GD32F4XX)
else()
    message(FATAL_ERROR "Unsupported chip type: ${CHIP_TYPE}")
endif()
