# StorageServer RPC模块CMakeLists.txt

# 源文件列表
set(RPC_SOURCES
    RPCClient.cpp
    RPCServer.cpp
)

# 头文件列表
set(RPC_HEADERS
    RPCClient.h
    RPCServer.h
)

# 创建静态库
add_library(storage_server_rpc STATIC ${RPC_SOURCES} ${RPC_HEADERS})

# 设置库的输出名称
set_target_properties(storage_server_rpc PROPERTIES OUTPUT_NAME "storage_server_rpc")

# 包含头文件目录
target_include_directories(storage_server_rpc PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Common
)

# 链接依赖库
target_link_libraries(storage_server_rpc
    common_protocol
    common_utils
    Threads::Threads
)
