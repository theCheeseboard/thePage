QT       += core gui thelib
TARGET = thepage
SHARE_APP_NAME = thepage

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

unix:!macx {
    # Include the-libs build tools
    include(/usr/share/the-libs/pri/buildmaster.pri)

    DEFINES += SYSTEM_LIBRARY_DIRECTORY=\\\"$$[QT_INSTALL_LIBS]\\\"

    QT += thelib
    TARGET = thepage

    LIBS += -L$$OUT_PWD/../libthepage/ -lthepage

    target.path = /usr/bin

    desktop.path = /usr/share/applications
    blueprint {
        desktop.files = com.vicr123.thepage-blueprint.desktop
    } else {
        desktop.files = com.vicr123.thepage.desktop
    }

    icon.path = /usr/share/icons/hicolor/scalable/apps/
    icon.files = icons/thepage.svg

    defaults.files = defaults.conf
    defaults.path = /etc/theSuite/thepage/

#    metainfo.files = com.vicr123.thepage.metainfo.xml
#    metainfo.path = /usr/share/metainfo

    INSTALLS += target desktop icon defaults #metainfo
}

win32 {
    # Include the-libs build tools
    include(C:/Program Files/thelibs/pri/buildmaster.pri)

    QT += winextras

    INCLUDEPATH += "C:/Program Files/thelibs/include" "C:/Program Files/theinstaller/include"
    LIBS += -L"C:/Program Files/thelibs/lib" -L"C:/Program Files/theinstaller/lib" -lthe-libs -ltheinstaller
    RC_FILE = icon.rc
    TARGET = thePage

    DEFINES += HAVE_THEINSTALLER

    win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../libthepage/release/ -lthepage
    else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../libthepage/debug/ -lthepage
}

macx {
    # Include the-libs build tools
    include(/usr/local/share/the-libs/pri/buildmaster.pri)

    QT += macextras
    LIBS += -framework CoreFoundation -framework AppKit
    QMAKE_INFO_PLIST = Info.plist

    LIBS += -L$$OUT_PWD/../libthepage/ -lthepage

    blueprint {
        TARGET = "thePage Blueprint"
        ICON = icon-bp.icns
    } else {
        TARGET = "thePage"
        ICON = icon.icns
    }

    INCLUDEPATH += "/usr/local/include/the-libs"
    LIBS += -L/usr/local/lib -lthe-libs

    QMAKE_POST_LINK += $$quote(cp $${PWD}/dmgicon.icns $${PWD}/app-dmg-background.png $${PWD}/node-appdmg-config*.json $${OUT_PWD}/..)
}

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    documentviewer.cpp \
    main.cpp \
    mainwindow.cpp \
    pageviewer.cpp \
    pluginmanager.cpp

HEADERS += \
    documentviewer.h \
    mainwindow.h \
    pageviewer.h \
    pluginmanager.h

FORMS += \
    documentviewer.ui \
    mainwindow.ui \
    pageviewer.ui

unix:!macx {
    DEFINES += SYSTEM_LIBRARY_DIRECTORY=\\\"$$[QT_INSTALL_LIBS]\\\"
}

INCLUDEPATH += $$PWD/../libthepage
DEPENDPATH += $$PWD/../libthepage

RESOURCES += \
    resources.qrc

DISTFILES += \
    com.vicr123.thepage-blueprint.desktop \
    com.vicr123.thepage.desktop \
    defaults.conf
