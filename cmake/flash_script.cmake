# flash_script.cmake
# 烧录脚本 - 执行实际的烧录操作

# 优先读取缓存文件
set(TARGET_ELF "")
if(EXISTS "${FIRMWARE_CACHE_FILE}")
  file(READ "${FIRMWARE_CACHE_FILE}" TARGET_ELF)
  string(STRIP "${TARGET_ELF}" TARGET_ELF)
endif()

# 如果缓存文件不存在或无效，搜索已构建的固件
if(NOT TARGET_ELF OR NOT EXISTS "${TARGET_ELF}")
  set(TARGET_ELF "")
  foreach(PATH ${FIRMWARE_SEARCH_PATHS})
    if(EXISTS "${PATH}")
      set(TARGET_ELF "${PATH}")
      break()
    endif()
  endforeach()
endif()

# 验证固件是否存在
if(NOT TARGET_ELF OR NOT EXISTS "${TARGET_ELF}")
  message(FATAL_ERROR "No firmware found!\nPlease build a target first (e.g., interface-driver-test or driver-test)")
endif()

get_filename_component(FIRMWARE_NAME "${TARGET_ELF}" NAME)
message(STATUS "===========================================")
message(STATUS "Flashing: ${FIRMWARE_NAME}")
message(STATUS "Firmware: ${TARGET_ELF}")
message(STATUS "===========================================")

execute_process(
  COMMAND ${OPENOCD_EXECUTABLE} -f ${OPENOCD_CFG} -c "init; reset halt; program ${TARGET_ELF} verify reset; shutdown"
  RESULT_VARIABLE FLASH_RESULT
)

if(FLASH_RESULT EQUAL 0)
  message(STATUS "Flash completed successfully!")
else()
  message(FATAL_ERROR "Flash failed with exit code: ${FLASH_RESULT}")
endif()
