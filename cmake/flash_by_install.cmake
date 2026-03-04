# flash_by_install.cmake
# 烧录脚本 - 使用 install 命令

set(OPENOCD_EXECUTABLE "openocd")

if(NOT DEFINED DEBUGGER_TYPE)
  set(DEBUGGER_TYPE "CMSIS-DAP")
endif()

if(CHIP_TYPE STREQUAL "STM32F407VET6")
  if(DEBUGGER_TYPE STREQUAL "STLINK")
    set(OPENOCD_CFG "${PROJECT_SOURCE_DIR}/Scripts/OpenOCD/openocd_stm32f4_stlink.cfg")
  else()
    set(OPENOCD_CFG "${PROJECT_SOURCE_DIR}/Scripts/OpenOCD/openocd_stm32f407.cfg")
  endif()
elseif(CHIP_TYPE STREQUAL "GD32F470ZIT6")
  if(DEBUGGER_TYPE STREQUAL "STLINK")
    set(OPENOCD_CFG "${PROJECT_SOURCE_DIR}/Scripts/OpenOCD/openocd_gd32f470_stlink.cfg")
  else()
    set(OPENOCD_CFG "${PROJECT_SOURCE_DIR}/Scripts/OpenOCD/openocd_gd32f470.cfg")
  endif()
else()
  message(FATAL_ERROR "Unknown CHIP_TYPE: ${CHIP_TYPE}")
endif()

get_filename_component(BUILD_DIR_NAME "${CMAKE_BINARY_DIR}" NAME)
set(FLASH_COPY_DIR "${PROJECT_SOURCE_DIR}/build/flash")

set(FIRMWARE_SEARCH_PATHS
  "${FLASH_COPY_DIR}/flash.elf"
  "${CMAKE_BINARY_DIR}/Source/Test/interface-driver-test.elf"
  "${CMAKE_BINARY_DIR}/Source/Test/interface-rtos-test.elf"
  "${CMAKE_BINARY_DIR}/Source/Test/osal-test.elf"
  "${CMAKE_BINARY_DIR}/Source/Test/driver-test.elf"
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
    set(FLASH_COPY_DIR \"${PROJECT_SOURCE_DIR}/build/flash\")
    set(FIRMWARE_SEARCH_PATHS \"${FIRMWARE_SEARCH_PATHS}\")
    set(OPENOCD_CFG \"${OPENOCD_CFG}\")
    set(OPENOCD_EXECUTABLE \"${OPENOCD_EXECUTABLE}\")
    
    # 优先使用 flash.elf（由 copy_flash_files.cmake 自动生成）
    set(TARGET_ELF \"\")
    if(EXISTS \"\${FLASH_COPY_DIR}/flash.elf\")
      set(TARGET_ELF \"\${FLASH_COPY_DIR}/flash.elf\")
    endif()
    
    # 如果 flash.elf 不存在，搜索已构建的固件
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
      message(FATAL_ERROR \"No firmware found!\nPlease build a target first (e.g., interface-driver-test)\")
    endif()
    
    get_filename_component(FIRMWARE_NAME \"\${TARGET_ELF}\" NAME)
    message(STATUS \"===========================================\")
    message(STATUS \"Flashing: \${FIRMWARE_NAME}\")
    message(STATUS \"Firmware: \${TARGET_ELF}\")
    message(STATUS \"===========================================\")
    
    # Align with the reference project's proven flow: direct program in one session.
    execute_process(
      COMMAND \${OPENOCD_EXECUTABLE}
              -f \${OPENOCD_CFG}
              -c init
              -c \"reset halt\"
              -c \"program \${TARGET_ELF} verify reset\"
              -c shutdown
      RESULT_VARIABLE FLASH_RESULT
      OUTPUT_VARIABLE FLASH_STDOUT
      ERROR_VARIABLE FLASH_STDERR
    )
    
    if(NOT \"\${FLASH_STDOUT}\" STREQUAL \"\")
      message(STATUS \"OpenOCD stdout:\\n\${FLASH_STDOUT}\")
    endif()
    if(NOT \"\${FLASH_STDERR}\" STREQUAL \"\")
      message(STATUS \"OpenOCD stderr:\\n\${FLASH_STDERR}\")
    endif()

    if(NOT \"\${FLASH_RESULT}\" STREQUAL \"0\")
      message(WARNING \"Initial program failed, try mass erase + reprogram...\")
      execute_process(
        COMMAND \${OPENOCD_EXECUTABLE}
                -f \${OPENOCD_CFG}
                -c init
                -c \"reset halt\"
                -c \"stm32f4x mass_erase 0\"
                -c shutdown
        RESULT_VARIABLE FLASH_ERASE_RESULT
        OUTPUT_VARIABLE FLASH_ERASE_STDOUT
        ERROR_VARIABLE FLASH_ERASE_STDERR
      )
      if(NOT \"\${FLASH_ERASE_STDOUT}\" STREQUAL \"\")
        message(STATUS \"OpenOCD erase stdout:\\n\${FLASH_ERASE_STDOUT}\")
      endif()
      if(NOT \"\${FLASH_ERASE_STDERR}\" STREQUAL \"\")
        message(STATUS \"OpenOCD erase stderr:\\n\${FLASH_ERASE_STDERR}\")
      endif()

      if(\"\${FLASH_ERASE_RESULT}\" STREQUAL \"0\")
        execute_process(
          COMMAND \${OPENOCD_EXECUTABLE}
                  -f \${OPENOCD_CFG}
                  -c init
                  -c \"reset halt\"
                  -c \"program \${TARGET_ELF} verify reset\"
                  -c shutdown
          RESULT_VARIABLE FLASH_RETRY_RESULT
          OUTPUT_VARIABLE FLASH_RETRY_STDOUT
          ERROR_VARIABLE FLASH_RETRY_STDERR
        )
        if(NOT \"\${FLASH_RETRY_STDOUT}\" STREQUAL \"\")
          message(STATUS \"OpenOCD retry stdout:\\n\${FLASH_RETRY_STDOUT}\")
        endif()
        if(NOT \"\${FLASH_RETRY_STDERR}\" STREQUAL \"\")
          message(STATUS \"OpenOCD retry stderr:\\n\${FLASH_RETRY_STDERR}\")
        endif()
        set(FLASH_RESULT \"\${FLASH_RETRY_RESULT}\")
      else()
        set(FLASH_RESULT \"\${FLASH_ERASE_RESULT}\")
      endif()
    endif()

    if(\"\${FLASH_RESULT}\" STREQUAL \"0\")
      message(STATUS \"Flash completed successfully!\")
    else()
      message(FATAL_ERROR \"Flash failed, result: \${FLASH_RESULT}\")
    endif()
  "
)
