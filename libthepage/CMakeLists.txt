cmake_minimum_required(VERSION 3.4.0)

project(lib VERSION 1.0.0 LANGUAGES CXX)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Multimedia Concurrent Network)
find_package(libcontemporary REQUIRED)

set(SOURCES
    document.cpp
    documentprovider.cpp
    documentprovidermanager.cpp
    page.cpp
)

set(HEADERS
    document.h
    documentprovider.h
    documentprovidermanager.h
    libthepage_global.h
    page.h
    thepageplugininterface.h
)

set(PRIVATE_HEADERS

)

add_library(libthepage SHARED ${SOURCES} ${HEADERS} ${PRIVATE_HEADERS})
set_target_properties(libthepage PROPERTIES
        CNTP_DATA_SUBDIR thepage/libthepage)
cntp_init(libthepage 20)
cntp_translate(libthepage)
cntp_target_public_headers(libthepage
    DESTINATION_DIRECTORY libthepage
    HEADERS ${HEADERS})
set_target_properties(libthepage PROPERTIES
        OUTPUT_NAME thepage
        FRAMEWORK TRUE
        MACOSX_FRAMEWORK_IDENTIFIER com.vicr123.libthepage
        VERSION 1.0.0)

target_link_libraries(libthepage Qt::Widgets Qt::DBus Qt::Multimedia Qt::Concurrent Qt::Network libcontemporary)
target_compile_definitions(libthepage PRIVATE LIBTHEBRANCH_LIBRARY)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set_target_properties(libthepage PROPERTIES OUTPUT_NAME libthepage)
    set(CMAKECONFIG_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/libthepage.framework/Resources/CMake)
ELSE()
    set(CMAKECONFIG_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/libthepage)
ENDIF()

configure_package_config_file(libthepageConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/libthepageConfig.cmake
        INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR})

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libthepageConfig.cmake
        DESTINATION ${CMAKECONFIG_INSTALL_DIR})
install(EXPORT libthepageTargets
        FILE libthepageTargets.cmake
        DESTINATION ${CMAKECONFIG_INSTALL_DIR})

install(TARGETS libthepage
        EXPORT libthepageTargets
        INCLUDES DESTINATION ${libthepage_INCLUDE_DIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        FRAMEWORK DESTINATION ${CMAKE_INSTALL_LIBDIR})

cntp_mark_support_library_directory()
