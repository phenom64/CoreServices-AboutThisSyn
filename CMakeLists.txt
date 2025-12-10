# CMakeLists.txt for CoreServices-AboutThisSyn
#
# TM & (C) 2025 Syndromatic Ltd. All rights reserved.
# Designed by Syndromatic in Manchester.
#
# This project is licensed under the GNU General Public License v2.0.

cmake_minimum_required(VERSION 3.16)

project(AboutThisSyn VERSION 1.0.0 LANGUAGES CXX)

set(MS_CXX_STANDARD 17 CACHE STRING "C++ standard to use (17/20/23)")
set_property(CACHE MS_CXX_STANDARD PROPERTY STRINGS 17 20 23)
set(CMAKE_CXX_STANDARD ${MS_CXX_STANDARD})
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build options
option(MS_DEBUG "Enable AboutThisSyn debug features" ON)

# Find necessary Qt5 packages
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

# Automatically run moc, uic, and rcc for build automation
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Define the source files for the project
set(PROJECT_SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/hardware.cpp
    src/hardware.h
)

# Add an executable
add_executable(AboutThisSyn ${PROJECT_SOURCES})

# Add the src directory to the include path
target_include_directories(AboutThisSyn PRIVATE src)

# Link the necessary libraries
target_link_libraries(AboutThisSyn
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
)

# Installation path
install(TARGETS AboutThisSyn
    RUNTIME DESTINATION bin
)
