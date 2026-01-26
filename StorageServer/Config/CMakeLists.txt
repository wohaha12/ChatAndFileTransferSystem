cmake_minimum_required(VERSION 3.16)
project(StorageServerConfig)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CONFIG_SOURCES
    StorageServerConfig.cpp
)

set(CONFIG_HEADERS
    StorageServerConfig.h
)

add_library(${PROJECT_NAME} STATIC ${CONFIG_SOURCES} ${CONFIG_HEADERS})

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Common
)

target_link_libraries(${PROJECT_NAME}
    CommonUtils
    Threads::Threads
)

message(STATUS "StorageServerConfig module configured")
