cmake_minimum_required(VERSION 3.16)
project(Wordpad VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)


add_executable(wordpad
    src/resources.qrc

    src/main.cpp
    src/MainWindow.cpp
    src/Editor.cpp
    src/EditorTabs.cpp
    src/DocumentManager.cpp
    src/FileTree.cpp

    ### Contains Q_OBJECT macros, needed for automoc
    src/include/notes/DocumentManager.h
    src/include/notes/Editor.h
    src/include/notes/EditorTabs.h
    src/include/notes/FileTree.h
    src/include/notes/MainWindow.h
)

target_include_directories(wordpad PRIVATE
    src/include
)

target_link_libraries(wordpad PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)
