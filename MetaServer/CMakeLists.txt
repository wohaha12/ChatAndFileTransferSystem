# MetaServer模块的CMakeLists.txt

# 设置模块名称
set(MODULE_NAME MetaServer)

# 添加子目录
add_subdirectory(Core)
add_subdirectory(Database)
add_subdirectory(Network)
add_subdirectory(Cache)
add_subdirectory(UI)
add_subdirectory(Config)

# 添加可执行文件
add_executable(${MODULE_NAME} main.cpp)

# 链接库
target_link_libraries(${MODULE_NAME} PRIVATE 
    Core 
    Database 
    Network 
    Cache 
    UI 
    Config
    Common
)

# 输出信息
message(STATUS "Added module: ${MODULE_NAME}")
