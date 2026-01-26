cmake_minimum_required(VERSION 3.16)
project(Protocol)

add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

message(STATUS "Protocol module configured")
