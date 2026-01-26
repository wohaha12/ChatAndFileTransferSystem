cmake_minimum_required(VERSION 3.16)
project(StorageServerCore)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CORE_SOURCES
    StorageServer.cpp
    StorageEngine.cpp
    TokenValidator.cpp
)

set(CORE_HEADERS
    StorageServer.h
    StorageEngine.h
    TokenValidator.h
)

add_library(${PROJECT_NAME} STATIC ${CORE_SOURCES} ${CORE_HEADERS})

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Common
)

target_link_libraries(${PROJECT_NAME}
    StorageServerNetwork
    StorageServerIO
    StorageServerRPC
    StorageServerConfig
    CommonProtocol
    CommonUtils
    Threads::Threads
)

message(STATUS "StorageServerCore module configured")
