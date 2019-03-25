#-------------------------------------------------
#
# Project created by QtCreator 2012-06-29T08:50:20
#
#-------------------------------------------------

include (../../config.pri)

QT       += core gui xml svg webkit

TARGET = botrace-editor
TEMPLATE = app

CONFIG += thread

RESOURCES     = ../../icons/gameicons.qrc

# define prefix for installation
unix {
    target.path = $${PREFIX}/bin
}
win32 {
    target.path = $${PREFIX}
}

INSTALLS += target

message("------------------------------------------------------------------------")
message(Install botrace-editor into: $$target.path)
message("------------------------------------------------------------------------")


HEADERS += \
    tilelist.h \
    editorwindow.h \
    boardwidget.h \
    boardtile.h \
    boarddetails.h \
    tiledetailwidget.h \
    boardscenarioscene.h \
    boardgraphicsitem.h \
    specialgraphicsitem.h

SOURCES += \
    main.cpp \
    tilelist.cpp \
    editorwindow.cpp \
    boardwidget.cpp \
    boardtile.cpp \
    boarddetails.cpp \
    tiledetailwidget.cpp \
    boardscenarioscene.cpp \
    boardgraphicsitem.cpp \
    specialgraphicsitem.cpp

FORMS += \
    editorwindow.ui \
    boarddetails.ui \
    tiledetailwidget.ui

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../core/release/ -lbotrace-core
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../core/debug/ -lbotrace-core
else:symbian: LIBS += -lbotrace-core
else:unix: LIBS += -L$$OUT_PWD/../core/ -lbotrace-core

INCLUDEPATH += $$PWD/../core
DEPENDPATH += $$PWD/../core

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../core/release/libbotrace-core.a
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../core/debug/libbotrace-core.a
else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../core/libbotrace-core.a
