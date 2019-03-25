

include (../../config.pri)

QT       += xml svg gui network webkit

TARGET = botrace-core
TEMPLATE = lib
CONFIG += staticlib thread

include (engine/engine.pri)
include (renderer/renderer.pri)
include (network/network.pri)
include (ui/ui.pri)
include (ki/ki.pri)
