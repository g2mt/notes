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
    src/main.cpp
    src/MainWindow.cpp
    src/TextEditor.cpp
    src/DocumentManager.cpp

    ### Contains Q_OBJECT macros, needed for automoc
    src/include/notes/DocumentManager.h
    src/include/notes/MainWindow.h
    src/include/notes/TextEditor.h
)

target_include_directories(wordpad PRIVATE src/include)

target_link_libraries(wordpad PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)
