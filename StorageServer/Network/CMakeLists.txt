cmake_minimum_required(VERSION 3.16)
project(StorageServerNetwork)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Network)

set(NETWORK_SOURCES
    TcpServer.cpp
    Connection.cpp
    ConnectionPool.cpp
)

set(NETWORK_HEADERS
    TcpServer.h
    Connection.h
    ConnectionPool.h
)

add_library(${PROJECT_NAME} STATIC ${NETWORK_SOURCES} ${NETWORK_HEADERS})

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Common
)

target_link_libraries(${PROJECT_NAME}
    Common
    Qt6::Core
    Qt6::Network
    Threads::Threads
)

message(STATUS "StorageServerNetwork module configured")
