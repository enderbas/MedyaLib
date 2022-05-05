cmake_minimum_required(VERSION 3.5)

project(MedyaLib VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Concurrent REQUIRED)
set(APP_ICON_RESOURCE "${CMAKE_CURRENT_SOURCE_DIR}/../resources.qrc")

set(PROJECT_SOURCES
        ${APP_ICON_RESOURCE}
        main.cpp
        medyalib.cpp
        medyalib.h
        medyalib.ui
        databasehelper.cpp
        databasehelper.h
        presentationwidget.cpp
        presentationwidget.h
        presentationwidget.ui
        badgetree.h
        badgetree.cpp
        badgetree.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(MedyaLib
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    add_executable(MedyaLib
        ${PROJECT_SOURCES})
endif()

target_link_libraries(MedyaLib PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql Qt${QT_VERSION_MAJOR}::Concurrent)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(MedyaLib)
endif()
