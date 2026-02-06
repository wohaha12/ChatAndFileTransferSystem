cmake_minimum_required(VERSION 3.16)
project(MetaServerDatabase)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Sql)

# 尝试查找MySQL（可选）
find_package(MySQL QUIET)

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

target_link_libraries(${PROJECT_NAME} PUBLIC
    Qt6::Core
    Qt6::Sql
)

# Link MySQL if found
if(MySQL_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${MySQL_LIBRARIES})
    target_include_directories(${PROJECT_NAME} PRIVATE ${MySQL_INCLUDE_DIR})
    target_compile_definitions(${PROJECT_NAME} PRIVATE USE_NATIVE_MYSQL)
    message(STATUS "MySQL found: ${MySQL_INCLUDE_DIR}")
else()
    message(STATUS "MySQL not found, using Qt SQL module only")
    target_compile_definitions(${PROJECT_NAME} PRIVATE USE_QT_SQL)
endif()

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/Common/Protocol
)

message(STATUS "MetaServerDatabase module configured")
