# Common模块的CMakeLists.txt

# 设置模块名称
set(MODULE_NAME Common)

# 添加子目录
add_subdirectory(Protocol)
add_subdirectory(Utils)

# 添加头文件到库
add_library(${MODULE_NAME} INTERFACE)

# 设置库的包含目录
target_include_directories(${MODULE_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# 链接子模块
target_link_libraries(${MODULE_NAME} INTERFACE Protocol Utils)

# 输出信息
message(STATUS "Added module: ${MODULE_NAME}")
