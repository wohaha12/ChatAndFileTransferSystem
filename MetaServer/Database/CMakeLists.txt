cmake_minimum_required(VERSION 3.16)
project(MetaServerDatabase)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Sql)
find_package(MySQL)

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

# Link MySQL if found
if(MySQL_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE MySQL::mysql_client)
    target_include_directories(${PROJECT_NAME} PRIVATE ${MYSQL_INCLUDE_DIR})
    message(STATUS "MySQL found: ${MYSQL_INCLUDE_DIR}")
else()
    message(WARNING "MySQL not found, database functionality may be limited")
endif()

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/Common/Protocol
)

message(STATUS "MetaServerDatabase module configured")
