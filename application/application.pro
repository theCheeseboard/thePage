QT       += core gui thelib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

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

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../libthepage/release/ -lthepage
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../libthepage/debug/ -lthepage
else:unix: LIBS += -L$$OUT_PWD/../libthepage/ -lthepage

INCLUDEPATH += $$PWD/../libthepage
DEPENDPATH += $$PWD/../libthepage
