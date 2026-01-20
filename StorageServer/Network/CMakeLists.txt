# StorageServer Network模块CMakeLists.txt

# 源文件列表
set(NETWORK_SOURCES
    TcpServer.cpp
    Connection.cpp
    ConnectionPool.cpp
)

# 头文件列表
set(NETWORK_HEADERS
    TcpServer.h
    Connection.h
    ConnectionPool.h
)

# 创建静态库
add_library(storage_server_network STATIC ${NETWORK_SOURCES} ${NETWORK_HEADERS})

# 设置库的输出名称
set_target_properties(storage_server_network PROPERTIES OUTPUT_NAME "storage_server_network")

# 包含头文件目录
target_include_directories(storage_server_network PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Common
)

# 链接依赖库
target_link_libraries(storage_server_network
    common_protocol
    common_utils
    Threads::Threads
)
