cmake_minimum_required(VERSION 3.16)

project(Client VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# 添加子目录
add_subdirectory(Common/Protocol)
add_subdirectory(Common/Utils)
add_subdirectory(Network)
add_subdirectory(Core)
add_subdirectory(UI)
add_subdirectory(Models)
add_subdirectory(Utils)

# 主程序源文件
set(SOURCES
    main.cpp
)

# 创建可执行文件
add_executable(${PROJECT_NAME}
    ${SOURCES}
)

# 链接子模块
target_link_libraries(${PROJECT_NAME}
    CommonProtocol
    CommonUtils
    ClientNetwork
    ClientCore
    ClientUI
    ClientModels
    ClientUtils
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# 包含目录
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# 设置输出目录
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 安装规则
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)
