cmake_minimum_required(VERSION 3.16)
project(StorageServerIO)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(IO_SOURCES
    FileReader.cpp
    FileWriter.cpp
    ChunkManager.cpp
)

set(IO_HEADERS
    FileReader.h
    FileWriter.h
    ChunkManager.h
)

add_library(${PROJECT_NAME} STATIC ${IO_SOURCES} ${IO_HEADERS})

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Common
)

target_link_libraries(${PROJECT_NAME}
    CommonUtils
    Threads::Threads
)

message(STATUS "StorageServerIO module configured")
