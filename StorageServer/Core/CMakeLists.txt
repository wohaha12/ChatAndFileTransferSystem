# StorageServer Core模块CMakeLists.txt

# 源文件列表
set(CORE_SOURCES
    StorageServer.cpp
    StorageEngine.cpp
    TokenValidator.cpp
)

# 头文件列表
set(CORE_HEADERS
    StorageServer.h
    StorageEngine.h
    TokenValidator.h
)

# 创建静态库
add_library(storage_server_core STATIC ${CORE_SOURCES} ${CORE_HEADERS})

# 设置库的输出名称
set_target_properties(storage_server_core PROPERTIES OUTPUT_NAME "storage_server_core")

# 包含头文件目录
target_include_directories(storage_server_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Common
)

# 链接依赖库
target_link_libraries(storage_server_core
    storage_server_network
    storage_server_io
    storage_server_rpc
    storage_server_config
    common_protocol
    common_utils
    Threads::Threads
)
