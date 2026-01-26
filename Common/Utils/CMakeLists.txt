cmake_minimum_required(VERSION 3.16)
project(Utils)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(HEADERS
    HashUtil.h
    StringUtil.h
    FileUtil.h
    TimeUtil.h
    PathUtil.h
    MemoryPool.h
)

set(SOURCES
    PathUtil.cpp
    MemoryPool.cpp
)

add_library(${PROJECT_NAME} STATIC
    ${SOURCES}
    ${HEADERS}
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

message(STATUS "Utils module configured")
