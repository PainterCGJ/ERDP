set(LOG_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/log_service.cpp
)

add_library(log-service STATIC EXCLUDE_FROM_ALL
    ${LOG_SOURCE}
)

target_include_directories(log-service PUBLIC
    ${CMAKE_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/Source/Common
    ${CMAKE_SOURCE_DIR}/Source/Interface
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(log-service
    osal
    hal-uart
)

set_target_properties(hal-uart PROPERTIES
    LINKER_LANGUAGE CXX
)
