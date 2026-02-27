set(OPENOCD_EXECUTABLE "openocd")

if(CHIP_TYPE STREQUAL "STM32F407VET6")
  set(OPENOCD_CFG "${PROJECT_SOURCE_DIR}/Scripts/OpenOCD/openocd_stm32f407.cfg")
  set(IF_CHIP_DIR "STM32F4XX")
elseif(CHIP_TYPE STREQUAL "GD32F470ZIT6")
  set(OPENOCD_CFG "${PROJECT_SOURCE_DIR}/Scripts/OpenOCD/openocd_gd32f470.cfg")
  set(IF_CHIP_DIR "GD32F4XX")
else()
  message(FATAL_ERROR "Unknown CHIP_TYPE: ${CHIP_TYPE}")
endif()

set(TARGET_ELF "${CMAKE_BINARY_DIR}/Source/Interface/Hardware/${IF_CHIP_DIR}/interface-test.elf")

install(
  CODE CODE
  "MESSAGE(\"===========================================\")"
  CODE "MESSAGE(\"Flashing ${TARGET_ELF}...\")"
  CODE "MESSAGE(\"Using OpenOCD config: ${OPENOCD_CFG}\")"
  CODE "MESSAGE(\"===========================================\")"
  CODE "execute_process(COMMAND ${OPENOCD_EXECUTABLE} -f ${OPENOCD_CFG} -c \"init; reset halt; program ${TARGET_ELF} verify reset; shutdown\")"
  RESULT_VARIABLE FLASH_RESULT
)

if(FLASH_RESULT)
  message(FATAL_ERROR "Flash failed with exit code: ${FLASH_RESULT}")
else()
  message(STATUS "Flash completed successfully!")
endif()
