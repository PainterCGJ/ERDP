# Source/Interface/CMakeLists.txt

add_subdirectory(Drivers)

if(EXISTS "${CMAKE_SOURCE_DIR}/Source/Interface/RTOS/CMakeLists.txt")
    add_subdirectory(RTOS)
endif()
