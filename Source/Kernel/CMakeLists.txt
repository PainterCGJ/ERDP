# Source/Kernel/CMakeLists.txt

# 引用 Drivers 文件夹
add_subdirectory(Drivers)

# 引用 RTOS 文件夹
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/RTOS/CMakeLists.txt")
    add_subdirectory(RTOS)
endif()
