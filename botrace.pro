
include (config.pri)

message("------------------------------------------------------------------------")
message(Building: $$GAME_VERSION - from git ref $$GITSH1 )
message("------------------------------------------------------------------------")

DEFINES += VERSION=$${GAME_VERSION}
DEFINES += GITREV=$${GITREV}
DEFINES += GITTAG=$${GITTAG}

TEMPLATE = subdirs
CONFIG += ordered

# add additional files
include (icons/icons.pri)
include (translations/translations.pri)

message("------------------------------------------------------------------------")
message("Install into:" )
message(PREFIX: $${PREFIX} )
message(SHARE_DIR: $${SHARE_DIR} )
message(TRANS_DIR: $${TRANS_DIR} )
message(BOARD_DIR: $${BOARD_DIR} )
message(THEME_DIR: $${THEME_DIR} )
message("------------------------------------------------------------------------")

# install translations
translations.path =  $${TRANS_DIR}
translations.files = $${BINTRANSLATIONS}
DEFINES += TRANS_DIR=$${TRANS_DIR}
INSTALLS += translations

#install all boards
boards.path = $${BOARD_DIR}
boards.files = boards/*
DEFINES += BOARD_DIR=$${BOARD_DIR}
INSTALLS += boards

#install game manuals
manuals.path = $${SHARE_DIR}/doc
manuals.files = doc/*.html doc/images
DEFINES += DOC_DIR=$${SHARE_DIR}/doc
INSTALLS += manuals

#install all themes
guithemes.path = $${THEME_DIR}/gui
guithemes.files = themes/gui/*
cardthemes.path = $${THEME_DIR}/cards
cardthemes.files = themes/cards/*
boardthemes.path = $${THEME_DIR}/board
boardthemes.files = themes/board/*
robotthemes.path = $${THEME_DIR}/robots
robotthemes.files = themes/robots/*
DEFINES += THEME_DIR=$${THEME_DIR}
INSTALLS += guithemes cardthemes boardthemes robotthemes

#install game icon and starter
unix {
    desktop.path = $${PREFIX}/share/applications
    desktop.files = icons/botrace.desktop
    desktop.files += icons/botrace-editor.desktop
    desktop.files += icons/botrace-server.desktop
    desktopicons.path = $${PREFIX}/share/pixmaps
    desktopicons.files = $${APPLICATIONICON}
    desktopicons.files += $${EDITORICON}
    desktopicons.files += $${SERVERICON}
    INSTALLS += desktop desktopicons
}


SUBDIRS += \
    src/core \
    src/editor \
    src/server \
    src/client
