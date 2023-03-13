cmake_minimum_required(VERSION 3.21.0)

project(thepage VERSION 1.0.0 LANGUAGES CXX)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets DBus Sql PrintSupport)
find_package(libcontemporary REQUIRED)

set(SOURCES
    documentviewer.cpp documentviewer.ui
    main.cpp
    mainwindow.cpp mainwindow.ui
    pageviewer.cpp pageviewer.ui
    landingpage.cpp landingpage.ui

    print/printcontroller.cpp

    resources.qrc
)

set(HEADERS
    documentviewer.h
    mainwindow.h
    pageviewer.h
    landingpage.h

    print/printcontroller.h
)

# Find Apple lproj files
#set(LPROJ_SOURCES)
#file(GLOB LPROJ_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/translations/apple-lproj/*.lproj)
#FOREACH(DIR ${LPROJ_DIRS})
#    cmake_path(GET DIR FILENAME DIR_NAME)
#    file(GLOB LPROJ_FILES ${DIR}/*.strings)
#    FOREACH(FILE ${LPROJ_FILES})
#        list(APPEND LPROJ_SOURCES ${FILE})
#        set_source_files_properties(${FILE} PROPERTIES
#                MACOSX_PACKAGE_LOCATION Resources/${DIR_NAME})
#    ENDFOREACH()
#ENDFOREACH()
#set(SOURCES ${SOURCES} ${LPROJ_SOURCES})

add_executable(thepage ${SOURCES} ${HEADERS})

set_target_properties(thepage PROPERTIES
        MACOSX_BUNDLE TRUE
#        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in
        MACOSX_BUNDLE_GUI_IDENTIFIER com.vicr123.thepage
        WIN32_EXECUTABLE TRUE
        CNTP_DATA_SUBDIR thepage)

cntp_init(thepage 20)
cntp_translate(thepage)
cntp_target_name(thepage "thePage")
cntp_app_icon(thepage
    BASEICON icons/baseicon.svg
#    BASEICON_MAC icons/baseicon-mac.svg
)
cntp_target_desktop_file(thepage
    DESKTOP_FILE com.vicr123.thepage.desktop
    BLUEPRINT_DESKTOP_FILE com.vicr123.thepage_blueprint.desktop)
cntp_defaults_file(thepage
    DEFAULTS_FILE thepage.conf)

target_link_libraries(thepage Qt::Widgets Qt::DBus Qt::Sql Qt::PrintSupport libcontemporary libthepage)
target_include_directories(thepage PUBLIC ../libthepage/)

install(TARGETS thepage
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        BUNDLE DESTINATION /Applications)
