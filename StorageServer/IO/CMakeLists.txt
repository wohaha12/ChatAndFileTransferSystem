# StorageServer IO模块CMakeLists.txt

# 源文件列表
set(IO_SOURCES
    FileReader.cpp
    FileWriter.cpp
    ChunkManager.cpp
)

# 头文件列表
set(IO_HEADERS
    FileReader.h
    FileWriter.h
    ChunkManager.h
)

# 创建静态库
add_library(storage_server_io STATIC ${IO_SOURCES} ${IO_HEADERS})

# 设置库的输出名称
set_target_properties(storage_server_io PROPERTIES OUTPUT_NAME "storage_server_io")

# 包含头文件目录
target_include_directories(storage_server_io PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Common
)

# 链接依赖库
target_link_libraries(storage_server_io
    common_utils
    Threads::Threads
)
