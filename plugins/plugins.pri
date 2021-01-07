CONFIG += plugin

INCLUDEPATH += $$PWD/../libthepage
DEPENDPATH += $$PWD/../libthepage

unix:!macx: {
    LIBS += -L$$OUT_PWD/../../libthepage/ -lthepage

    CI = $$(CI)
    if (isEmpty(CI)) {
        target.path = $$[QT_INSTALL_LIBS]/thepage/plugins
    } else {
        target.path = /usr/lib/thepage/plugins
    }
    INSTALLS += target translations

    QMAKE_POST_LINK += $$QMAKE_COPY_DIR $$quote($$_PRO_FILE_PWD_/translations) $$shell_quote($$OUT_PWD) && \
        $$QMAKE_COPY $$quote($$_PRO_FILE_PWD_/defaults.conf) $$shell_quote($$OUT_PWD)
}

win32 {
    CONFIG(release, debug|release) {
        LIBS += -L$$OUT_PWD/../../libthepage/release/ -lthepage
    } else {
        CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../libthepage/debug/ -lthepage
    }
}

HEADERS +=

SOURCES +=
