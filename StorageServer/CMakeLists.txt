cmake_minimum_required(VERSION 3.10)
project(StorageServer VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 输出目录设置
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)

# 包含头文件目录
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../Common
)

# 添加子目录
add_subdirectory(Core)
add_subdirectory(Network)
add_subdirectory(IO)
add_subdirectory(RPC)
add_subdirectory(Config)

# 查找依赖库
find_package(Threads REQUIRED)

# 创建可执行文件
add_executable(storage_server main.cpp)

# 链接库
target_link_libraries(storage_server
    storage_server_core
    storage_server_network
    storage_server_io
    storage_server_rpc
    storage_server_config
    common_protocol
    common_utils
    Threads::Threads
)

# 安装配置
install(TARGETS storage_server
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
