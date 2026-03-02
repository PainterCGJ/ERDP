# OSAL (Operating System Abstraction Layer) CMakeLists.txt
# C++17 静态库

enable_language(CXX)

set(OSAL_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/erdp_osal.cpp
)

add_library(osal STATIC EXCLUDE_FROM_ALL
    ${OSAL_SOURCES}
)

target_include_directories(osal PUBLIC
    ${CMAKE_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/Source/Common
    ${CMAKE_SOURCE_DIR}/Source/Interface
)

target_link_libraries(osal PUBLIC
    interface-driver
    interface-rtos
)

set_target_properties(osal PROPERTIES
    LINKER_LANGUAGE CXX
)

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/osal.lib
    COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:osal>
        ${CMAKE_BINARY_DIR}/osal.lib
    DEPENDS osal
    COMMENT "Generating Keil MDK compatible osal.lib"
    VERBATIM
)

add_custom_target(osal.lib DEPENDS osal)

target_compile_features(osal PRIVATE cxx_std_17)

target_compile_options(osal PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Wno-implicit-function-declaration
    -fno-rtti
    -fno-exceptions
)

target_link_options(osal PRIVATE
    -Wl,--print-memory-usage
)
