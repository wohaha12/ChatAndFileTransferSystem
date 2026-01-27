cmake_minimum_required(VERSION 3.16)
project(Tests VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Test)

include_directories(
    ${CMAKE_SOURCE_DIR}/../Common/Protocol
    ${CMAKE_SOURCE_DIR}/../Common/Utils
    ${CMAKE_SOURCE_DIR}/../MetaServer/Core
    ${CMAKE_SOURCE_DIR}/../MetaServer/Database
    ${CMAKE_SOURCE_DIR}/../StorageServer/Core
)

set(TEST_SOURCES
    main.cpp
    test_memory_pool.cpp
    test_memory_pool.h
    test_load_balancer.cpp
    test_load_balancer.h
    test_protocol.cpp
    test_protocol.h
    test_database.cpp
    test_database.h
    test_token_validator.cpp
    test_token_validator.h
    test_storage_engine.cpp
    test_storage_engine.h
)

add_executable(tests ${TEST_SOURCES})

target_link_libraries(tests
    Qt6::Core
    Qt6::Test
    CommonProtocol
    CommonUtils
)

enable_testing()

QTEST_ADD_TESTCASE(MemoryPoolTest tests_memory_pool.cpp tests_memory_pool.h)
QTEST_ADD_TESTCASE(LoadBalancerTest tests_load_balancer.cpp tests_load_balancer.h)
QTEST_ADD_TESTCASE(ProtocolTest tests_protocol.cpp tests_protocol.h)
QTEST_ADD_TESTCASE(DatabaseTest tests_database.cpp tests_database.h)
QTEST_ADD_TESTCASE(TokenValidatorTest tests_token_validator.cpp tests_token_validator.h)
QTEST_ADD_TESTCASE(StorageEngineTest tests_storage_engine.cpp tests_storage_engine.h)

message(STATUS "Tests module configured")
