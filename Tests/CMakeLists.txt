cmake_minimum_required(VERSION 3.16)
project(Tests VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Test)

include_directories(
    ${CMAKE_SOURCE_DIR}/Common/Protocol
    ${CMAKE_SOURCE_DIR}/Common/Utils
    ${CMAKE_SOURCE_DIR}/MetaServer/Core
    ${CMAKE_SOURCE_DIR}/MetaServer/Database
    ${CMAKE_SOURCE_DIR}/MetaServer/Cache
    ${CMAKE_SOURCE_DIR}/StorageServer/Core
)

set(TEST_SOURCES
    test_memory_pool.cpp
    test_memory_pool.h
    test_protocol.cpp
    test_protocol.h
    test_load_balancer.cpp
    test_load_balancer.h
)

add_executable(tests ${TEST_SOURCES})

target_link_libraries(tests
    Qt6::Core
    Qt6::Test
    CommonProtocol
    CommonUtils
)

enable_testing()

add_test(NAME MemoryPoolTest COMMAND tests)
add_test(NAME ProtocolTest COMMAND tests)
add_test(NAME LoadBalancerTest COMMAND tests)

message(STATUS "Tests module configured")
