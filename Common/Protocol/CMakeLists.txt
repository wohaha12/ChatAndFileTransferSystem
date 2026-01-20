# Protocol模块的CMakeLists.txt

# 设置模块名称
set(MODULE_NAME Protocol)

# 定义源文件
set(HEADERS
    TransHeader.h
    CommandTypes.h
    ClientProtocol.h
    InternalProtocol.h
)

# 添加头文件到库
add_library(${MODULE_NAME} INTERFACE)

# 设置库的包含目录
target_include_directories(${MODULE_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# 依赖项
# target_link_libraries(${MODULE_NAME} INTERFACE ...)

# 输出信息
message(STATUS "Added module: ${MODULE_NAME}")
