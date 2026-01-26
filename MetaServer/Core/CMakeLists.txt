cmake_minimum_required(VERSION 3.16)
project(MetaServerCore)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Network Sql)

set(HEADERS
    MetaServer.h
    AuthModule.h
    LoadBalancer.h
    FileMetaMgr.h
)

set(SOURCES
    MetaServer.cpp
    AuthModule.cpp
    LoadBalancer.cpp
    FileMetaMgr.cpp
)

add_library(${PROJECT_NAME} STATIC
    ${SOURCES}
    ${HEADERS}
)

target_link_libraries(${PROJECT_NAME}
    Qt6::Core
    Qt6::Network
    Qt6::Sql
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/Common/Protocol
        ${CMAKE_SOURCE_DIR}/Common/Utils
)

message(STATUS "MetaServerCore module configured")
