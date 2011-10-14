# Add more folders to ship with the application, here
folder_01.source = qml/sudoku-united
folder_01.target = qml
DEPLOYMENTFOLDERS = folder_01

# Taken from Butaca pro file
isEmpty(PREFIX) {
    PREFIX = /usr
}
BINDIR = $$PREFIX/bin
DATADIR =$$PREFIX/share

splash.files += splash.jpg
splash.path = $$DATADIR/$${TARGET}/

icon64.path = $$DATADIR/icons/hicolor/64x64/apps
icon64.files += sudoku-united64.png

INSTALLS += splash icon64

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

symbian:TARGET.UID3 = 0xE282D591

# Smart Installer package's UID
# This UID is from the protected range and therefore the package will
# fail to install if self-signed. By default qmake uses the unprotected
# range value if unprotected UID is defined for the application and
# 0x2002CCCF value if protected UID is given to the application
#symbian:DEPLOYMENT.installer_header = 0x2002CCCF

# Allow network access on Symbian
symbian:TARGET.CAPABILITY += NetworkServices

# If your application uses the Qt Mobility libraries, uncomment the following
# lines and add the respective components to the MOBILITY variable.
CONFIG += mobility

CONFIG += link_pkgconfig
PKGCONFIG += TelepathyQt4

#contains(MEEGO_EDITION,harmattan) {
CONFIG += qdeclarative-boostable
CONFIG += qt-boostable
PKGCONFIG += qdeclarative-boostable
#}

# Add dependency to symbian components
# CONFIG += qtquickcomponents

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp \
    board.cpp \
    player.cpp \
    game.cpp \
    message.cpp \
    boardgenerator.cpp \
    sudoku.cpp \
    adapters/serveradapter.cpp \
    adapters/abstractserver.cpp \
    adapters/tcp/tcpserver.cpp \
    adapters/abstractclient.cpp \
    adapters/tcp/tcpclient.cpp \
    adapters/tcp/tcpgameinfo.cpp \
    adapters/gameinfo.cpp \
    adapters/telepathy/telepathyclient.cpp \
    adapters/telepathy/telepathyserver.cpp \
    adapters/telepathy/telepathygameinfo.cpp \
    adapters/telepathy/telepathyhandler.cpp \
    adapters/bluetooth/bluetoothclient.cpp \
    adapters/bluetooth/bluetoothserver.cpp \
    adapters/bluetooth/bluetoothgameinfo.cpp \
    settings.cpp \
    highscore.cpp

# Please do not modify the following two lines. Required for deployment.
include(qmlapplicationviewer/qmlapplicationviewer.pri)
qtcAddDeployment()

OTHER_FILES += \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog \

HEADERS += \
    board.h \
    player.h \
    game.h \
    message.h \
    boardgenerator.h \
    sudoku.h \
    adapters/serveradapter.h \
    adapters/abstractserver.h \
    adapters/tcp/tcpserver.h \
    adapters/abstractclient.h \
    adapters/tcp/tcpclient.h \
    adapters/tcp/tcpgameinfo.h \
    adapters/gameinfo.h \
    adapters/telepathy/telepathyclient.h \
    adapters/telepathy/telepathyserver.h \
    adapters/telepathy/telepathygameinfo.h \
    adapters/telepathy/telepathyhandler.h \
    adapters/bluetooth/bluetoothclient.h \
    adapters/bluetooth/bluetoothserver.h \
    adapters/bluetooth/bluetoothgameinfo.h \
    settings.h \
    highscore.h

RESOURCES += resources.qrc

QT += dbus
MOBILITY += connectivity systeminfo

!isEmpty(BREAKPAD_PATH) {
    !isEmpty(SCRATCHBOX_PATH) {
        QMAKE_POST_LINK = cp $(TARGET) $${SCRATCHBOX_PATH}/users/$$(USER)/$${BREAKPAD_PATH}; $${SCRATCHBOX_PATH}/login $${BREAKPAD_PATH}/src/tools/linux/dump_syms/dump_syms $${SCRATCHBOX_PATH}/users/$$(USER)/$${BREAKPAD_PATH}/$(TARGET) > sudoku-united-`date +%Y%m%d%H%M%S`.sym
        BREAKPAD_PATH = $${SCRATCHBOX_PATH}/users/$$(USER)/$${BREAKPAD_PATH}
    } else {
        QMAKE_POST_LINK = $${BREAKPAD_PATH}/src/tools/linux/dump_syms/dump_syms $(TARGET) > sudoku-united-`date +%Y%m%d%H%M%S`.sym
    }

    LIBS += -L$${BREAKPAD_PATH}/src/client/linux -lbreakpad_client
    INCLUDEPATH += $${BREAKPAD_PATH}/src
    DEFINES += ENABLE_BREAKPAD
}


