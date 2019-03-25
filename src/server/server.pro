#-------------------------------------------------
#
# Project created by QtCreator 2012-06-29T09:15:56
#
#-------------------------------------------------

include (../../config.pri)

QT       += core gui network xml svg

TARGET = botrace-server
TEMPLATE = app
CONFIG += thread

# define prefix for installation
unix {
    target.path = $${PREFIX}/bin
}
win32 {
    target.path = $${PREFIX}
}

INSTALLS += target

message("------------------------------------------------------------------------")
message(Install botrace-server into: $$target.path)
message("------------------------------------------------------------------------")

HEADERS += \
    serverclient.h \
    server.h \
    hostserverdialog.h

SOURCES += \
    main.cpp \
    serverclient.cpp \
    server.cpp \
    hostserverdialog.cpp

FORMS += \
    hostserverdialog.ui

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../core/release/ -lbotrace-core
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../core/debug/ -lbotrace-core
else:symbian: LIBS += -lbotrace-core
else:unix: LIBS += -L$$OUT_PWD/../core/ -lbotrace-core

INCLUDEPATH += $$PWD/../core
DEPENDPATH += $$PWD/../core

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../core/release/libbotrace-core.a
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../core/debug/libbotrace-core.a
else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../core/libbotrace-core.a
