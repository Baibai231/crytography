QT       += core gui widgets multimedia multimediawidgets

CONFIG += c++17

# 确保这三个 cpp 都在列表里
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

# 确保这两个 h 都在列表里
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

# 只有你创建了资源文件才保留这行
RESOURCES += \
    res.qrc

# 如果你是 Windows 系统，这一行能帮你看到调试输出
CONFIG += console
