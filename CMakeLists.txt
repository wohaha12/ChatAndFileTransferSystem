cmake_minimum_required(VERSION 3.16)

# vcpkg toolchain configuration
if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
    message(STATUS "Using vcpkg toolchain: $ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
endif()

project(ChatAndFileTransferSystem VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

option(BUILD_TESTS "Build tests" ON)
option(BUILD_CLIENT "Build client" ON)
option(BUILD_METASERVER "Build meta server" ON)
option(BUILD_STORAGESERVER "Build storage server" ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network Sql)

add_subdirectory(Common)

if(BUILD_CLIENT)
    add_subdirectory(Client)
endif()

if(BUILD_METASERVER)
    add_subdirectory(MetaServer)
endif()

if(BUILD_STORAGESERVER)
    add_subdirectory(StorageServer)
endif()

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(Tests)
endif()

message(STATUS "========================================")
message(STATUS "Project: ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "========================================")
message(STATUS "Build Options:")
message(STATUS "  BUILD_CLIENT: ${BUILD_CLIENT}")
message(STATUS "  BUILD_METASERVER: ${BUILD_METASERVER}")
message(STATUS "  BUILD_STORAGESERVER: ${BUILD_STORAGESERVER}")
message(STATUS "  BUILD_TESTS: ${BUILD_TESTS}")
message(STATUS "========================================")
