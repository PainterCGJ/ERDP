# flash_by_install.cmake
# 烧录脚本 - 使用 install 命令

set(OPENOCD_EXECUTABLE "openocd")

if(NOT DEFINED DEBUGGER_TYPE)
  set(DEBUGGER_TYPE "CMSIS-DAP")
endif()

if(CHIP_TYPE STREQUAL "STM32F407VET6")
  if(DEBUGGER_TYPE STREQUAL "STLINK")
    set(OPENOCD_CFG "${PROJECT_SOURCE_DIR}/Scripts/OpenOCD/openocd_stm32f407_stlink.cfg")
  else()
    set(OPENOCD_CFG "${PROJECT_SOURCE_DIR}/Scripts/OpenOCD/openocd_stm32f407.cfg")
  endif()
  set(CHIP_DIR "STM32F4XX")
elseif(CHIP_TYPE STREQUAL "GD32F470ZIT6")
  if(DEBUGGER_TYPE STREQUAL "STLINK")
    set(OPENOCD_CFG "${PROJECT_SOURCE_DIR}/Scripts/OpenOCD/openocd_gd32f470_stlink.cfg")
  else()
    set(OPENOCD_CFG "${PROJECT_SOURCE_DIR}/Scripts/OpenOCD/openocd_gd32f470.cfg")
  endif()
  set(CHIP_DIR "GD32F4XX")
else()
  message(FATAL_ERROR "Unknown CHIP_TYPE: ${CHIP_TYPE}")
endif()

get_filename_component(BUILD_DIR_NAME "${CMAKE_BINARY_DIR}" NAME)
set(FIRMWARE_CACHE_FILE "${CMAKE_BINARY_DIR}/firmware_path.txt")

set(FIRMWARE_SEARCH_PATHS
  "${CMAKE_BINARY_DIR}/Interface/Drivers/${CHIP_DIR}/interface-driver-test.elf"
  "${CMAKE_BINARY_DIR}/Source/Interface/Drivers/${CHIP_DIR}/interface-driver-test.elf"
  "${CMAKE_BINARY_DIR}/Source/Test/${CHIP_DIR}/osal-test.elf"
  "${CMAKE_BINARY_DIR}/Drivers/${CHIP_DIR}/Test/driver-test.elf"
)

message(STATUS "===========================================")
message(STATUS "Flash Configuration:")
message(STATUS "  Chip: ${CHIP_TYPE}")
message(STATUS "  Debugger: ${DEBUGGER_TYPE}")
message(STATUS "  Build: ${BUILD_DIR_NAME}")
message(STATUS "  OpenOCD Config: ${OPENOCD_CFG}")
message(STATUS "===========================================")

install(
  CODE "
    set(BUILD_DIR \"${CMAKE_BINARY_DIR}\")
    set(FIRMWARE_CACHE_FILE \"${FIRMWARE_CACHE_FILE}\")
    set(FIRMWARE_SEARCH_PATHS \"${FIRMWARE_SEARCH_PATHS}\")
    set(OPENOCD_CFG \"${OPENOCD_CFG}\")
    set(OPENOCD_EXECUTABLE \"${OPENOCD_EXECUTABLE}\")
    
    # 优先读取缓存文件
    set(TARGET_ELF \"\")
    if(EXISTS \"\${FIRMWARE_CACHE_FILE}\")
      file(READ \"\${FIRMWARE_CACHE_FILE}\" TARGET_ELF)
      string(STRIP \"\${TARGET_ELF}\" TARGET_ELF)
    endif()
    
    # 如果缓存文件不存在或无效，搜索已构建的固件
    if(NOT TARGET_ELF OR NOT EXISTS \"\${TARGET_ELF}\")
      set(TARGET_ELF \"\")
      foreach(PATH \${FIRMWARE_SEARCH_PATHS})
        if(EXISTS \"\${PATH}\")
          set(TARGET_ELF \"\${PATH}\")
          break()
        endif()
      endforeach()
    endif()
    
    # 验证固件是否存在
    if(NOT TARGET_ELF OR NOT EXISTS \"\${TARGET_ELF}\")
      message(FATAL_ERROR \"No firmware found!\nPlease build a target first (e.g., interface-driver-test or driver-test)\")
    endif()
    
    get_filename_component(FIRMWARE_NAME \"\${TARGET_ELF}\" NAME)
    message(STATUS \"===========================================\")
    message(STATUS \"Flashing: \${FIRMWARE_NAME}\")
    message(STATUS \"Firmware: \${TARGET_ELF}\")
    message(STATUS \"===========================================\")
    
    execute_process(
      COMMAND \${OPENOCD_EXECUTABLE} -f \${OPENOCD_CFG} -c \"init; reset halt; program \${TARGET_ELF} verify reset; shutdown\"
      RESULT_VARIABLE FLASH_RESULT
    )
    
    if(FLASH_RESULT EQUAL 0)
      message(STATUS \"Flash completed successfully!\")
    else()
      message(FATAL_ERROR \"Flash failed with exit code: \${FLASH_RESULT}\")
    endif()
  "
  EXCLUDE_FROM_ALL
)
