cmake_minimum_required(VERSION 3.16)
project(Protocol)

set(HEADERS
    TransHeader.h
    CommandTypes.h
    ProtocolBodies.h
    InternalProtocol.h
    CommonProtocol.h
)

find_package(Qt6 REQUIRED COMPONENTS Core)

add_library(${PROJECT_NAME} STATIC ${HEADERS})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC Qt6::Core)

message(STATUS "Protocol module configured")
