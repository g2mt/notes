cmake_minimum_required(VERSION 3.16)
project(notes VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)


add_executable(notes
    src/resources.qrc

    src/main.cpp
    src/MainWindow.cpp
    src/Editor.cpp
    src/EditorTabs.cpp
    src/DocumentManager.cpp
    src/FileTree.cpp
    src/SideBar.cpp
    src/TablePopup.cpp

    ### Contains Q_OBJECT macros, needed for automoc
    src/include/notes/DocumentManager.h
    src/include/notes/Editor.h
    src/include/notes/EditorDocument.h
    src/include/notes/EditorTabs.h
    src/include/notes/FileTree.h
    src/include/notes/MainWindow.h
    src/include/notes/SideBar.h
    src/include/notes/TablePopup.h
)

target_include_directories(notes PRIVATE
    src/include
)

target_link_libraries(notes PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)
