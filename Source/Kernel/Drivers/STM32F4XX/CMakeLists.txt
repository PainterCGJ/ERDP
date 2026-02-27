# STM32F4XX 驱动库 CMakeLists.txt

# 设置包含目录
set(STM32F4XX_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/CMSIS/Device/ST/STM32F4xx/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/CMSIS/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/STM32F4xx_StdPeriph_Driver/inc
    ${CMAKE_SOURCE_DIR}/Source
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 收集源文件
set(STM32F4XX_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/CMSIS/Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c
    ${CMAKE_CURRENT_SOURCE_DIR}/stm32f4xx_it.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/syscalls.c
)

# 收集标准外设驱动源文件
file(GLOB_RECURSE STM32F4XX_PERIPH_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/STM32F4xx_StdPeriph_Driver/src/*.c
)

# 从源文件列表中排除 FMC 相关文件（STM32F407 系列不支持 FMC）
list(REMOVE_ITEM STM32F4XX_PERIPH_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_fmc.c")

# 添加标准外设驱动源文件
list(APPEND STM32F4XX_SOURCES ${STM32F4XX_PERIPH_SOURCES})

# 创建静态库
add_library(driver-lib STATIC
    ${STM32F4XX_SOURCES}
)

# 生成 Keil MDK 兼容的 .lib 文件
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/driver-lib.lib
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_BINARY_DIR}/libdriver-lib.a
        ${CMAKE_BINARY_DIR}/driver-lib.lib
    DEPENDS driver-lib
    COMMENT "Generating Keil MDK compatible driver-lib.lib"
    VERBATIM
)

# 设置编译定义
target_compile_definitions(driver-lib PUBLIC
    STM32F4XX
    STM32F40_41xxx
    USE_STDPERIPH_DRIVER
)

# 导出库的包含目录，方便其他目标使用
target_include_directories(driver-lib PUBLIC
    ${STM32F4XX_INCLUDE_DIRS}
)

# 设置编译选项
target_compile_options(driver-lib PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

# 设置链接选项
target_link_options(driver-lib PRIVATE
    -Wl,--print-memory-usage
)

# 添加测试代码（如果存在）
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Test/CMakeLists.txt)
    add_subdirectory(Test)
endif()

# 添加 .lib 文件目标
add_custom_target(driver-lib.lib ALL DEPENDS ${CMAKE_BINARY_DIR}/driver-lib.lib)
