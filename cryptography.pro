QT       += core gui widgets multimedia multimediawidgets

CONFIG += c++17

SOURCES += \
    aichatdialog.cpp \
    aimanager.cpp \
    arrow.cpp \
    caesarpuzzle.cpp \
    gamestate.cpp \
    inputcontroller.cpp \
    levelmanager.cpp \
    levelselectdialog.cpp \
    main.cpp \
    GameWindow.cpp \
    miniaespuzzle.cpp \
    miniaesvisualizer.cpp \
    Player.cpp \
    puzzlemanager.cpp \
    railfencepuzzle.cpp \
    railfencevisualizer.cpp \
    spinwheeldialog.cpp \
    startscreen.cpp \
    terrainblock.cpp \
    uimanager.cpp \
    videocontroller.cpp \
    vigenerepuzzle.cpp \
    vigenerevisualizer.cpp

HEADERS += \
    GameWindow.h \
    Player.h \
    aichatdialog.h \
    aimanager.h \
    arrow.h \
    caesarpuzzle.h \
    gameprogress.h \
    gamestate.h \
    inputcontroller.h \
    levelmanager.h \
    levelselectdialog.h \
    miniaespuzzle.h \
    miniaesvisualizer.h \
    puzzlemanager.h \
    railfencepuzzle.h \
    railfencevisualizer.h \
    spinwheeldialog.h \
    startscreen.h \
    terrainblock.h \
    uimanager.h \
    videocontroller.h \
    vigenerepuzzle.h \
    vigenerevisualizer.h

RESOURCES += \
    res.qrc

CONFIG += console
