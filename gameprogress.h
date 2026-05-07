#ifndef GAMEPROGRESS_H
#define GAMEPROGRESS_H

#include <QSettings>

class GameProgress {
public:
    static int getMaxUnlockedLevel() {
        QSettings settings("CryptoGame", "LevelData");
        return settings.value("maxLevel", 1).toInt();
    }

    static void unlockLevel(int level) {
        QSettings settings("CryptoGame", "LevelData");
        int currentMax = settings.value("maxLevel", 1).toInt();
        if (level > currentMax) {
            settings.setValue("maxLevel", level);
        }
    }

    static void reset() {
        QSettings settings("CryptoGame", "LevelData");
        settings.setValue("maxLevel", 1);
    }
};

#endif
