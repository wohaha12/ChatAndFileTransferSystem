# StorageServer Config模块CMakeLists.txt

# 源文件列表
set(CONFIG_SOURCES
    StorageServerConfig.cpp
)

# 头文件列表
set(CONFIG_HEADERS
    StorageServerConfig.h
)

# 创建静态库
add_library(storage_server_config STATIC ${CONFIG_SOURCES} ${CONFIG_HEADERS})

# 设置库的输出名称
set_target_properties(storage_server_config PROPERTIES OUTPUT_NAME "storage_server_config")

# 包含头文件目录
target_include_directories(storage_server_config PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Common
)

# 链接依赖库
target_link_libraries(storage_server_config
    common_utils
    Threads::Threads
)
