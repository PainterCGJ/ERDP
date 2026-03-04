cmake_minimum_required(VERSION 3.10)

# 交叉编译设置
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

# 告诉 CMake 不要尝试链接可执行文件（嵌入式系统）
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(ERDP LANGUAGES C ASM CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 从 CMakePresets.json 中获取配置
if(DEFINED DRIVER_PATH)
    message(STATUS "Using driver path: ${DRIVER_PATH}")
else()
    message(STATUS "DRIVER_PATH not defined, using default: Source/Kernel/Drivers/STM32F4XX")
    set(DRIVER_PATH "${CMAKE_SOURCE_DIR}/Source/Kernel/Drivers/STM32F4XX")
endif()

if(DEFINED CHIP_TYPE)
    message(STATUS "Using chip type: ${CHIP_TYPE}")
else()
    message(STATUS "CHIP_TYPE not defined, using default: STM32F407VET6")
    set(CHIP_TYPE "STM32F407VET6")
endif()

if(DEFINED LINKER_SCRIPT)
    message(STATUS "Using linker script: ${LINKER_SCRIPT}")
else()
    message(STATUS "LINKER_SCRIPT not defined, using default")
    set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/Source/Kernel/Drivers/STM32F4XX/Linker/STM32F407VET6_FLASH.ld")
endif()

if(DEFINED CPU_PARAMETERS)
    message(STATUS "Using CPU parameters: ${CPU_PARAMETERS}")
    set(CMAKE_C_FLAGS "${CPU_PARAMETERS} ${CMAKE_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CPU_PARAMETERS} ${CMAKE_CXX_FLAGS}")
else()
    message(STATUS "CPU_PARAMETERS not defined, using default")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()

# 编译选项 (ARM GCC)
add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -Wshadow
    -Wundef
    -Wno-unused-parameter
    -Wno-unused-function
    -Wimplicit-function-declaration
)

# 链接选项
add_link_options(
    -Wl,--print-memory-usage
    -specs=nosys.specs
    -T ${LINKER_SCRIPT}
)

# 引用 Kernel 层
add_subdirectory(Source/Kernel)

# 引用 Interface 层
add_subdirectory(Source/Interface)

# 引用 OSAL 层
if(EXISTS "${CMAKE_SOURCE_DIR}/Source/OSAL/CMakeLists.txt")
    add_subdirectory(Source/OSAL)
endif()

# 引用 Test 层
if(EXISTS "${CMAKE_SOURCE_DIR}/Source/Test/CMakeLists.txt")
    add_subdirectory(Source/Test)
endif()

# 烧录功能（通过 cmake install 调用）
include(${CMAKE_SOURCE_DIR}/cmake/flash_by_install.cmake)

# 未来模块的框架
# 应用程序模块
# if(EXISTS "${CMAKE_SOURCE_DIR}/Source/Application/CMakeLists.txt")
#     add_subdirectory(Source/Application)
# endif()

# 中间件模块
# if(EXISTS "${CMAKE_SOURCE_DIR}/Source/Middleware/CMakeLists.txt")
#     add_subdirectory(Source/Middleware)
# endif()

# 板级支持包
# if(EXISTS "${CMAKE_SOURCE_DIR}/Source/BSP/CMakeLists.txt")
#     add_subdirectory(Source/BSP)
# endif()

message(STATUS "CMake configuration completed for ${CHIP_TYPE}")

# 自动复制 compile_commands.json 到项目根目录（配置阶段执行）
if(CMAKE_EXPORT_COMPILE_COMMANDS)
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_BINARY_DIR}/compile_commands.json
            ${CMAKE_SOURCE_DIR}/compile_commands.json
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
    message(STATUS "Copied compile_commands.json to source directory")
endif()
