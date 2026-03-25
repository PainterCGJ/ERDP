# ERDP CMakeLists.txt
cmake_minimum_required(VERSION 3.20)

# 针对裸机开发的交叉编译配置
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
# 跳过编译器检查时的链接步骤（避免缺少 specs 文件导致的链接错误）
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# 检测 ARM 交叉编译工具链
if(NOT DEFINED ARM_GNU_PATH)
    if(DEFINED ENV{ARM_GNU_PATH})
        set(ARM_GNU_PATH $ENV{ARM_GNU_PATH})
        message(STATUS "Using ARM_GNU_PATH from environment: ${ARM_GNU_PATH}")
    else()
        # 默认路径（根据常见安装位置）
        set(ARM_GNU_PATH "D:/Material/environment/Tools/arm-gnu-toolchain-13.2.Rel1-mingw-w64-i686-arm-none-eabi")
        message(STATUS "Using default ARM_GNU_PATH: ${ARM_GNU_PATH}")
    endif()
endif()

# 规范化路径，防止反斜杠导致的转义错误
if(ARM_GNU_PATH)
    file(TO_CMAKE_PATH "${ARM_GNU_PATH}" ARM_GNU_PATH)
endif()

# 设置交叉编译器
if(ARM_GNU_PATH)
    # 确保 ARM_GNU_PATH 指向工具链根目录（不是 bin 目录）
    if(ARM_GNU_PATH MATCHES "/bin$|\\\\bin$")
        get_filename_component(ARM_GNU_PATH ${ARM_GNU_PATH} DIRECTORY)
    endif()
    
    # 将工具链 bin 目录加入 PATH，以便 project() 能找到编译器
    # 注意：在 Windows 上，ENV{PATH} 使用分号分隔，且路径分隔符最好与系统一致
    file(TO_NATIVE_PATH "${ARM_GNU_PATH}/bin" ARM_GNU_BIN_NATIVE)
    set(ENV{PATH} "${ARM_GNU_BIN_NATIVE};$ENV{PATH}")
endif()

project(ERDP C CXX ASM)

if(ARM_GNU_PATH)
    # 添加 C++ 标准库头文件路径
    message(STATUS "Searching for C++ headers in: ${ARM_GNU_PATH}/arm-none-eabi/include/c++")
    file(GLOB CPP_HEADERS_ROOT "${ARM_GNU_PATH}/arm-none-eabi/include/c++/*")
    
    foreach(DIR ${CPP_HEADERS_ROOT})
        if(IS_DIRECTORY ${DIR})
            get_filename_component(DIR_NAME ${DIR} NAME)
            if(DIR_NAME MATCHES "[0-9]+\.[0-9]+\.[0-9]+")
                message(STATUS "Adding C++ system include path: ${DIR}")
                include_directories(SYSTEM "${DIR}")
                include_directories(SYSTEM "${DIR}/arm-none-eabi")
                include_directories(SYSTEM "${DIR}/backward")
            endif()
        endif()
    endforeach()
endif()

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 从 CMakePresets.json 中获取配置
if(DEFINED DRIVER_PATH)
    message(STATUS "Using driver path: ${DRIVER_PATH}")
else()
    message(STATUS "DRIVER_PATH not defined, using default: Source/Kernel/Drivers/STM32F4XX")
    set(DRIVER_PATH "${CMAKE_SOURCE_DIR}/Source/Kernel/Drivers/STM32F4XX")
endif()

# 从 DRIVER_PATH 中提取芯片系列
# 移除末尾的斜杠以确保 get_filename_component 正常工作
string(REGEX REPLACE "/+$" "" DRIVER_PATH_NO_SLASH "${DRIVER_PATH}")
get_filename_component(CHIP_DIR ${DRIVER_PATH_NO_SLASH} NAME)
message(STATUS "Chip directory: ${CHIP_DIR}")

# 根据芯片系列设置特定的包含路径和定义
if(CHIP_DIR STREQUAL "STM32F4XX")
    add_definitions(-DSTM32F4XX)
    add_definitions(-DSTM32F40_41xxx)
    add_definitions(-DUSE_STDPERIPH_DRIVER)
    
    # STM32F4 特定的包含路径
    include_directories(
        ${DRIVER_PATH}/CMSIS/Device/ST/STM32F4xx/Include
        ${DRIVER_PATH}/CMSIS/Include
        ${DRIVER_PATH}/STM32F4xx_StdPeriph_Driver/inc
    )
    
    # 设置启动文件和链接脚本
    if(NOT DEFINED STARTUP_FILE)
        set(STARTUP_FILE "${DRIVER_PATH}/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc_ride7/startup_stm32f40_41xxx.s")
    endif()
    if(NOT DEFINED LINKER_SCRIPT)
        set(LINKER_SCRIPT "${DRIVER_PATH}/Linker/STM32F407VET6_FLASH.ld")
    endif()
    
elseif(CHIP_DIR STREQUAL "GD32F4XX")
    add_definitions(-DGD32F4XX)
    add_definitions(-DGD32F450)
    add_definitions(-DUSE_STDPERIPH_DRIVER)
    
    # GD32F4 特定的包含路径
    include_directories(
        ${DRIVER_PATH}/CMSIS/GD/GD32F4xx/Include
        ${DRIVER_PATH}/CMSIS/Include
        ${DRIVER_PATH}/GD32F4xx_standard_peripheral/Include
    )
    
    # 设置启动文件和链接脚本
    if(NOT DEFINED STARTUP_FILE)
        set(STARTUP_FILE "${DRIVER_PATH}/CMSIS/GD/GD32F4xx/Source/GCC/startup_gd32f450_470.s")
    endif()
    if(NOT DEFINED LINKER_SCRIPT)
        set(LINKER_SCRIPT "${DRIVER_PATH}/Linker/GD32F470ZIT6_FLASH.ld")
    endif()
else()
    message(WARNING "Unknown chip directory: ${CHIP_DIR}")
endif()

# 编译选项 (ARM GCC)
if(CPU_PARAMETERS)
    separate_arguments(CPU_PARAMETERS_LIST NATIVE_COMMAND "${CPU_PARAMETERS}")
endif()

add_compile_options(
    ${CPU_PARAMETERS_LIST}
    -Wall
    -Wextra
    -Wpedantic
    -Wshadow
    -Wundef
    -Wno-unused-parameter
    -Wno-unused-function
    -Wno-gnu-variadic-macros
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-pedantic>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-c++11-extensions>
    $<$<COMPILE_LANGUAGE:C>:-Wimplicit-function-declaration>
)

# 链接选项
add_link_options(
    ${CPU_PARAMETERS_LIST}
    -T${LINKER_SCRIPT}
    # -specs=nano.specs  <-- 移除全局的 specs 设置
    -specs=nosys.specs
    -Wl,--gc-sections
    -Wl,-Map=${CMAKE_BINARY_DIR}/output.map
)

# 引用 Kernel 层
add_subdirectory(Source/Kernel)

# 引用 Interface 层
add_subdirectory(Source/Interface)

# 引用 HAL 层
if(EXISTS "${CMAKE_SOURCE_DIR}/Source/HAL/CMakeLists.txt")
    add_subdirectory(Source/HAL)
endif()

# 引用 OSAL 层
if(EXISTS "${CMAKE_SOURCE_DIR}/Source/OSAL/CMakeLists.txt")
    add_subdirectory(Source/OSAL)
endif()

# 引用 Test 层
if(EXISTS "${CMAKE_SOURCE_DIR}/Source/Test/CMakeLists.txt")
    add_subdirectory(Source/Test)
endif()

# 应用程序模块
if(EXISTS "${CMAKE_SOURCE_DIR}/Source/Application/CMakeLists.txt")
    add_subdirectory(Source/Application)
endif()

# 服务模块
if(EXISTS "${CMAKE_SOURCE_DIR}/Source/Service/CMakeLists.txt")
    add_subdirectory(Source/Service)
endif()

# 烧录功能（通过 cmake install 调用）
include(${CMAKE_SOURCE_DIR}/cmake/flash_by_install.cmake)

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
        RESULT_VARIABLE copy_result
    )
    if(copy_result EQUAL 0)
        message(STATUS "Copied compile_commands.json to project root")
    else()
        message(WARNING "Failed to copy compile_commands.json")
    endif()
endif()
