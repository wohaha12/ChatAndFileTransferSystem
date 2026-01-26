cmake_minimum_required(VERSION 3.16)
project(MetaServerDatabase)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Sql)

set(HEADERS
    DatabaseManager.h
    UserDAO.h
    FileDAO.h
    FriendDAO.h
)

set(SOURCES
    DatabaseManager.cpp
    UserDAO.cpp
    FileDAO.cpp
    FriendDAO.cpp
)

add_library(${PROJECT_NAME} STATIC
    ${SOURCES}
    ${HEADERS}
)

target_link_libraries(${PROJECT_NAME}
    Qt6::Core
    Qt6::Sql
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/Common/Protocol
)

message(STATUS "MetaServerDatabase module configured")
