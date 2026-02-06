cmake_minimum_required(VERSION 3.16)
project(Tests VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Test Concurrent)

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
    Qt6::Concurrent
    Protocol
    Utils
    StorageServerCore
    MetaServerCore
    MetaServerDatabase
)

enable_testing()

# 添加测试
add_test(NAME AllTests COMMAND tests)

# 如果需要单独运行每个测试类，可以使用以下方式
# 注意：这需要测试类支持命令行参数来指定运行哪个测试
# add_test(NAME MemoryPoolTest COMMAND tests -class MemoryPoolTest)
# add_test(NAME LoadBalancerTest COMMAND tests -class LoadBalancerTest)
# add_test(NAME ProtocolTest COMMAND tests -class ProtocolTest)
# add_test(NAME DatabaseTest COMMAND tests -class DatabaseTest)
# add_test(NAME TokenValidatorTest COMMAND tests -class TokenValidatorTest)
# add_test(NAME StorageEngineTest COMMAND tests -class StorageEngineTest)

message(STATUS "Tests module configured")
