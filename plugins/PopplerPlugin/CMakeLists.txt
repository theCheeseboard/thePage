project(lib VERSION 1.0.0 LANGUAGES CXX)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets DBus)
find_package(libcontemporary)
cntp_find_pkgconfig(poppler poppler-qt${QT_VERSION_MAJOR} REQUIRED
#        WIN_FALLBACK_DLL "${TAGLIB_ROOT}/bin/tag.dll"
#        WIN_FALLBACK_IMPLIB "${TAGLIB_ROOT}/lib/tag.lib"
#        WIN_FALLBACK_INCLUDE "${TAGLIB_ROOT}/include"
)

set(SOURCES
    plugin.cpp
    popplerdocument.cpp
    popplerdocumentprovider.cpp
    popplerpage.cpp
)

set(HEADERS
    plugin.h
    popplerdocument.h
    popplerdocumentprovider.h
    popplerpage.h
)

add_library(plugin-poppler SHARED ${SOURCES} ${HEADERS})
target_include_directories(plugin-poppler PUBLIC ../../libthepage/)

set_target_properties(plugin-poppler PROPERTIES
        OUTPUT_NAME popplerPlugin
        FRAMEWORK FALSE
        TRANSLATION_SOURCES "${SOURCES};${HEADERS}"
    )

cntp_init_plugin(thepage plugin-poppler 20 popplerplugin)
cntp_translate(plugin-poppler)

target_link_libraries(plugin-poppler Qt::Widgets Qt::DBus libcontemporary libthepage PkgConfig::poppler)
