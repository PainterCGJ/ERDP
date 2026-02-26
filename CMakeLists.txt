cmake_minimum_required(VERSION 3.10)

# 交叉编译设置
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

# 告诉 CMake 不要尝试链接可执行文件（嵌入式系统）
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(ERDP LANGUAGES C ASM)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 从 CMakePresets.json 中获取配置
if(DEFINED DRIVER_PATH)
    message(STATUS "Using driver path: ${DRIVER_PATH}")
else()
    message(STATUS "DRIVER_PATH not defined, using default: Source/Kernel/Driver/stm32f4xx")
    set(DRIVER_PATH "${CMAKE_SOURCE_DIR}/Source/Kernel/Driver/stm32f4xx")
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
    set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/Source/Kernel/Driver/stm32f4xx/Linker/STM32F407VET6_FLASH.ld")
endif()

# 芯片系列配置
if(CHIP_TYPE MATCHES "STM32F4.*")
    set(CHIP_SERIES "STM32F4")
    # 设置 STM32 特定的宏
    if(CHIP_TYPE MATCHES "STM32F407.*")
        set(STM32_CHIP "STM32F407xx")
        add_definitions(-DSTM32F40_41xxx)
    elseif(CHIP_TYPE MATCHES "STM32F429.*")
        set(STM32_CHIP "STM32F429xx")
        add_definitions(-DSTM32F427_437xx)
    else()
        set(STM32_CHIP "STM32F407xx")
        add_definitions(-DSTM32F40_41xxx)
    endif()
    
    # 添加 STM32F4 特定的子目录
    if(EXISTS "${DRIVER_PATH}/CMakeLists.txt")
        add_subdirectory(${DRIVER_PATH} ${CMAKE_BINARY_DIR}/driver)
    else()
        message(FATAL_ERROR "Driver CMakeLists.txt not found at: ${DRIVER_PATH}")
    endif()
elseif(CHIP_TYPE MATCHES "GD32F4.*")
    set(CHIP_SERIES "GD32F4")
    # 设置 GD32 特定的宏
    add_definitions(-DGD32F470)
    
    # 未来添加 GD32F4 特定的子目录
    # if(EXISTS "${DRIVER_PATH}/CMakeLists.txt")
    #     add_subdirectory(${DRIVER_PATH} ${CMAKE_BINARY_DIR}/driver)
    # else()
    #     message(FATAL_ERROR "Driver CMakeLists.txt not found at: ${DRIVER_PATH}")
    # endif()
else()
    message(FATAL_ERROR "Unsupported chip type: ${CHIP_TYPE}")
endif()

message(STATUS "Using chip series: ${CHIP_SERIES}")

# 编译选项 (ARM GCC)
add_compile_options(
    ${CPU_PARAMETERS}
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
    ${CPU_PARAMETERS}
    -Wl,--print-memory-usage
    -T ${LINKER_SCRIPT}
)

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