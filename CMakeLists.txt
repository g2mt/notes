cmake_minimum_required(VERSION 3.16)
project(notes VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

add_library(md4c STATIC
    vendored/md4c/src/md4c.c
    vendored/md4c/src/entity.c
)

target_include_directories(md4c PUBLIC
    vendored/md4c/src
)

add_executable(notes
    src/resources.qrc

    src/main.cpp
    src/MainWindow.cpp
    src/Editor.cpp
    src/EditorDocument.cpp
    src/EditorElement.cpp
    src/EditorCursor.cpp
    src/EditorLine.cpp
    src/blocks/EditorBlock.cpp
    src/blocks/EditorListItemBlock.cpp
    src/blocks/EditorListBlock.cpp
    src/blocks/EditorTableCellBlock.cpp
    src/blocks/EditorTableBlock.cpp
    src/blocks/EditorTableRowBlock.cpp
    src/blocks/EditorHrBlock.cpp
    src/blocks/EditorHeadingBlock.cpp
    src/blocks/EditorCodeBlock.cpp
    src/blocks/EditorAdmonitionBlock.cpp
    src/fragments/EditorFragment.cpp
    src/fragments/EditorBrFragment.cpp
    src/fragments/EditorTextFragment.cpp
    src/EditorTabs.cpp
    src/DocumentManager.cpp
    src/FileTree.cpp
    src/SideBar.cpp
    src/TablePopup.cpp

    ### Contains Q_OBJECT macros, needed for automoc
    src/include/notes/DocumentManager.h
    src/include/notes/Editor.h
    src/include/notes/EditorDocument.h
    src/include/notes/EditorElement.h
    src/include/notes/EditorLine.h
    src/include/notes/EditorTabs.h
    src/include/notes/FileTree.h
    src/include/notes/MainWindow.h
    src/include/notes/SideBar.h
    src/include/notes/TablePopup.h
    src/include/notes/blocks/EditorAdmonitionBlock.h
    src/include/notes/blocks/EditorListBlock.h
    src/include/notes/blocks/EditorBlock.h
    src/include/notes/blocks/EditorListItemBlock.h
    src/include/notes/blocks/EditorCodeBlock.h
    src/include/notes/blocks/EditorTableBlock.h
    src/include/notes/blocks/EditorTableRowBlock.h
    src/include/notes/blocks/EditorHeadingBlock.h
    src/include/notes/blocks/EditorTableCellBlock.h
    src/include/notes/blocks/EditorHrBlock.h
    src/include/notes/fragments/EditorFragment.h
    src/include/notes/fragments/EditorBrFragment.h
    src/include/notes/fragments/EditorTextFragment.h
)

target_include_directories(notes PRIVATE
    src/include
)

target_link_libraries(notes PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    md4c
)
