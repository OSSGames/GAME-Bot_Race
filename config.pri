
#RELEASEBUILD
#GITREV
#GITTAG
#GITSH1
#GAME_VERSION

# get git revision
isEmpty(RELEASEBUILD) {
    unix {
        GITREV = $$system(git describe --tags | cut -d'-' -f2)
        GITTAG = $$system(git describe --tags | cut -d'-' -f1)
        GITSH1 = $$system(git log --pretty=format:'%h' -1)
        GAME_VERSION=$${GITTAG}"-"$${GITREV}
        isEmpty(GITREV) {
            GAME_VERSION=$${GITTAG}
        }
    }
}

CONFIG(release, debug|release) {
    message(release build in progress...)
    CONFIG += warn_off
    DEFINES += QT_NO_DEBUG_OUTPUT QT_NO_WARNING_OUTPUT
}
CONFIG(debug, debug|release) {
    message(debug build in progress...)
    CONFIG += warn_on
}

# define prefix for installation
unix {
    isEmpty( PREFIX ) { PREFIX = /usr }
    SHARE_DIR = $${PREFIX}/share/botrace
    TRANS_DIR = $${SHARE_DIR}/translations
    BOARD_DIR = $${SHARE_DIR}/boards
    THEME_DIR = $${SHARE_DIR}/themes
}

win32 {
    isEmpty( PREFIX ) { PREFIX = $$OUT_PWD }
    SHARE_DIR = $${PREFIX}
    TRANS_DIR = $${SHARE_DIR}/translations
    BOARD_DIR = $${SHARE_DIR}/boards
    THEME_DIR = $${SHARE_DIR}/themes
    #    RC_FILE = $$PWD/Icons/BotRace-win32.rc
}

DEFINES += SHARE_DIR=$${SHARE_DIR}
DEFINES += GITREV=$${GITREV}
DEFINES += GITTAG=$${GITTAG}
DEFINES += GITSH1=$${GITSH1}
DEFINES += GAME_VERSION=$${GAME_VERSION}
