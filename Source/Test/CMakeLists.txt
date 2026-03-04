# Test CMakeLists.txt

if(CHIP_TYPE STREQUAL "STM32F407VET6")
    add_subdirectory(STM32F4XX)
elseif(CHIP_TYPE STREQUAL "GD32F470ZIT6")
    add_subdirectory(GD32F4XX)
else()
    message(WARNING "No test defined for chip: ${CHIP_TYPE}")
endif()
