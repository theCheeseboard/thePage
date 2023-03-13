cmake_minimum_required(VERSION 3.1.0)
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.15)
set(QT_VERSION_MAJOR 6 CACHE STRING "Major version of Qt to target")

project(thepage VERSION 1.0.0 LANGUAGES CXX)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    link_directories(/usr/local/lib)
ENDIF()

find_package(QT REQUIRED NAMES Qt6)

add_subdirectory(libthepage)
add_subdirectory(application)
add_subdirectory(plugins)
