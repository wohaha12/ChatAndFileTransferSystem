cmake_minimum_required(VERSION 3.16)
project(MetaServerCore)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Network Sql)
find_package(OpenSSL)

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

target_link_libraries(${PROJECT_NAME} PUBLIC
    Qt6::Core
    Qt6::Network
    Qt6::Sql
    Protocol
)

# Link OpenSSL if found
if(OpenSSL_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE OpenSSL::SSL OpenSSL::Crypto)
    target_include_directories(${PROJECT_NAME} PRIVATE ${OPENSSL_INCLUDE_DIR})
    message(STATUS "OpenSSL found: ${OPENSSL_INCLUDE_DIR}")
else()
    message(WARNING "OpenSSL not found, using Qt's built-in cryptographic functions")
endif()

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/Common/Protocol
        ${CMAKE_SOURCE_DIR}/Common/Utils
)

message(STATUS "MetaServerCore module configured")
