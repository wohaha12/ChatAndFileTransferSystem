cmake_minimum_required(VERSION 3.16)
project(MetaServerCache)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Concurrent)
find_package(hiredis)

set(HEADERS
    RedisManager.h
)

set(SOURCES
    RedisManager.cpp
)

add_library(${PROJECT_NAME} STATIC
    ${SOURCES}
    ${HEADERS}
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    Qt6::Core
    Qt6::Concurrent
)

# Link hiredis if found
if(hiredis_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE hiredis::hiredis)
    target_compile_definitions(${PROJECT_NAME} PRIVATE USE_NATIVE_REDIS)
    message(STATUS "hiredis found")
else()
    message(WARNING "hiredis not found, caching functionality may be limited")
endif()

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

message(STATUS "MetaServerCache module configured")
