cmake_minimum_required(VERSION 3.16)

project(ClientUI)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

set(SOURCES
    MainWindow.cpp
    LoginDialog.cpp
    ChatWidget.cpp
    FileWidget.cpp
    StyleManager.cpp
    AnimationManager.cpp
)

set(HEADERS
    MainWindow.h
    LoginDialog.h
    ChatWidget.h
    FileWidget.h
    StyleManager.h
    AnimationManager.h
)

set(UI_FILES
    MainWindow.ui
    LoginDialog.ui
    ChatWidget.ui
    FileWidget.ui
)

add_library(${PROJECT_NAME} STATIC
    ${SOURCES}
    ${HEADERS}
    ${UI_FILES}
)

target_link_libraries(${PROJECT_NAME}
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
