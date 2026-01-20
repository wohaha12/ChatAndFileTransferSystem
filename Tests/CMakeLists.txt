cmake_minimum_required(VERSION 3.10)
project(Tests CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Test)

include_directories(
    ${CMAKE_SOURCE_DIR}/Common/Protocol
    ${CMAKE_SOURCE_DIR}/Common/Utils
    ${CMAKE_SOURCE_DIR}/MetaServer/Core
    ${CMAKE_SOURCE_DIR}/MetaServer/Database
    ${CMAKE_SOURCE_DIR}/MetaServer/Cache
    ${CMAKE_SOURCE_DIR}/StorageServer/Core
)

set(TEST_SOURCES
    Tests/test_memory_pool.cpp
    Tests/test_database_manager.cpp
    Tests/test_protocol.cpp
    Tests/test_auth_module.cpp
    Tests/test_load_balancer.cpp
    Tests/test_redis_manager.cpp
    Tests/test_token_validator.cpp
    Tests/test_storage_engine.cpp
)

add_executable(tests ${TEST_SOURCES})
target_link_libraries(tests
    Qt5::Core
    Qt5::Test
    pthread
)

enable_testing()
add_test(NAME MemoryPoolTest COMMAND tests)
add_test(NAME DatabaseManagerTest COMMAND tests)
add_test(NAME ProtocolTest COMMAND tests)
add_test(NAME AuthModuleTest COMMAND tests)
add_test(NAME LoadBalancerTest COMMAND tests)
add_test(NAME RedisManagerTest COMMAND tests)
add_test(NAME TokenValidatorTest COMMAND tests)
add_test(NAME StorageEngineTest COMMAND tests)