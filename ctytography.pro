QT       += core gui widgets multimedia multimediawidgets

CONFIG += c++17

# 确保这三个 cpp 都在列表里
SOURCES += \
    arrow.cpp \
    caesarpuzzle.cpp \
    main.cpp \
    GameWindow.cpp \
    Player.cpp \
    railfencepuzzle.cpp \
    startscreen.cpp \
    videoplayer.cpp

# 确保这两个 h 都在列表里
HEADERS += \
    GameWindow.h \
    Player.h \
    arrow.h \
    caesarpuzzle.h \
    railfencepuzzle.h \
    startscreen.h \
    terrainblock.h \
    videoplayer.h

# 只有你创建了资源文件才保留这行
RESOURCES += \
    res.qrc

# 如果你是 Windows 系统，这一行能帮你看到调试输出
CONFIG += console
