cmake_minimum_required(VERSION 3.16)
project(notes VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets WebEngineWidgets WebChannel)

find_program(NPM npm REQUIRED)
find_program(NODE node REQUIRED)

set(PROSE_SRC_DIR ${CMAKE_SOURCE_DIR}/frontend)
set(PROSE_BUNDLE ${CMAKE_BINARY_DIR}/frontend/prose.js)

file(GLOB_RECURSE PROSE_TS_SOURCES
  ${PROSE_SRC_DIR}/src/*.ts
)

add_custom_command(
  OUTPUT ${PROSE_BUNDLE}
  COMMAND ${NPM} install --silent
  COMMAND ${NODE} build.js ${PROSE_BUNDLE}
  WORKING_DIRECTORY ${PROSE_SRC_DIR}
  DEPENDS ${PROSE_TS_SOURCES} ${PROSE_SRC_DIR}/package.json ${PROSE_SRC_DIR}/build.js
  COMMENT "Building ProseMirror TypeScript bundle"
)

add_custom_target(prose_bundle DEPENDS ${PROSE_BUNDLE})

configure_file(
  ${CMAKE_SOURCE_DIR}/src/resources.qrc.in
  ${CMAKE_BINARY_DIR}/resources.qrc
  @ONLY
)

add_executable(notes
    ${CMAKE_BINARY_DIR}/resources.qrc

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
    src/include/notes/EditorBridge_p.h
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
    Qt6::WebEngineWidgets
    Qt6::WebChannel
)

add_dependencies(notes prose_bundle)
