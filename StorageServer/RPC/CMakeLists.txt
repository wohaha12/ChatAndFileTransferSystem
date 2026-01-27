cmake_minimum_required(VERSION 3.16)
project(StorageServerRPC)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Network)

set(RPC_SOURCES
    RPCClient.cpp
    RPCServer.cpp
)

set(RPC_HEADERS
    RPCClient.h
    RPCServer.h
)

add_library(${PROJECT_NAME} STATIC ${RPC_SOURCES} ${RPC_HEADERS})

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    ${CMAKE_SOURCE_DIR}/Common
)

target_link_libraries(${PROJECT_NAME}
    Qt6::Core
    Qt6::Network
    CommonProtocol
    CommonUtils
    Threads::Threads
)

message(STATUS "StorageServerRPC module configured")
