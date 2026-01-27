cmake_minimum_required(VERSION 3.16)
project(MetaServer VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network Sql)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(Core)
add_subdirectory(Database)
add_subdirectory(Network)
add_subdirectory(Cache)
add_subdirectory(UI)
add_subdirectory(Config)

set(SOURCES
    main.cpp
)

add_executable(${PROJECT_NAME}
    ${SOURCES}
)

target_link_libraries(${PROJECT_NAME}
    Common
    MetaServerCore
    MetaServerDatabase
    MetaServerNetwork
    MetaServerCache
    MetaServerUI
    MetaServerConfig
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    Qt6::Sql
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

message(STATUS "MetaServer module configured")
