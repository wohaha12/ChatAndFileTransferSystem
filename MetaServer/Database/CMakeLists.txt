# Database模块的CMakeLists.txt

# 设置模块名称
set(MODULE_NAME Database)

# 定义源文件
set(HEADERS
    DatabaseManager.h
    UserDAO.h
    FileDAO.h
)

set(SOURCES
    DatabaseManager.cpp
    UserDAO.cpp
    FileDAO.cpp
)

# 添加库
add_library(${MODULE_NAME} STATIC ${HEADERS} ${SOURCES})

# 设置库的包含目录
target_include_directories(${MODULE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# 依赖项
# target_link_libraries(${MODULE_NAME} PRIVATE ...)

# 输出信息
message(STATUS "Added module: ${MODULE_NAME}")
