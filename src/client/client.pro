#-------------------------------------------------
#
# Project created by QtCreator 2012-06-29T09:20:48
#
#-------------------------------------------------

include (../../config.pri)

QT       += core gui network xml svg webkit

TARGET = botrace
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
message(Install botrace-client into: $$target.path)
message("------------------------------------------------------------------------")

HEADERS += \
    localclient.h \
    networkclient.h \
    carddeckwidget.h \
    carditem.h \
    dropspot.h \
    flagitem.h \
    laseritem.h \
    gameview.h \
    gamescene.h \
    gameboard.h \
    mainwindow.h \
    startspot.h \
    selectorientationdialog.h \
    robotitem.h \
    programmingwidget.h \
    newgamedialog.h \
    gameresultscreen.h \
    gamelogandchatwidget.h \
    gameboardanimation.h \
    introscene.h\
    joingamedialog.h \
    gamesimulationitem.h \
    participantlistmodel.h \
    participantlistdelegate.h \
    programchatdock.h

SOURCES += \
    main.cpp \
    localclient.cpp \
    networkclient.cpp \
    carddeckwidget.cpp \
    carditem.cpp \
    dropspot.cpp \
    flagitem.cpp \
    laseritem.cpp \
    gameview.cpp \
    gamescene.cpp \
    gameboard.cpp \
    mainwindow.cpp \
    newgamedialog.cpp \
    startspot.cpp \
    selectorientationdialog.cpp \
    robotitem.cpp \
    programmingwidget.cpp \
    gameresultscreen.cpp \
    gamelogandchatwidget.cpp \
    gameboardanimation.cpp \
    introscene.cpp \
    joingamedialog.cpp \
    gamesimulationitem.cpp \
    participantlistmodel.cpp \
    participantlistdelegate.cpp \
    programchatdock.cpp

FORMS += \
    mainwindow.ui \
    selectorientationdialog.ui \
    newgamedialog.ui \
    gamelogandchatwidget.ui \
    joingamedialog.ui \
    programchatdock.ui


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../core/release/ -lbotrace-core
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../core/debug/ -lbotrace-core
else:symbian: LIBS += -lbotrace-core
else:unix: LIBS += -L$$OUT_PWD/../core/ -lbotrace-core

INCLUDEPATH += $$PWD/../core
DEPENDPATH += $$PWD/../core

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../core/release/libbotrace-core.a
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../core/debug/libbotrace-core.a
else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../core/libbotrace-core.a
