#include "LevelManager.h"
#include "TerrainBlock.h"
#include "Arrow.h"

#include <QPainter>

LevelManager::LevelManager(QGraphicsScene *scene, QObject *parent)
    : QObject(parent), scene(scene) {}

Player* LevelManager::getPlayer() const {
    return player;
}

int LevelManager::getCurrentLevel() const {
    return currentLevel;
}

void LevelManager::clearLevel() {
    if (scene) {
        scene->clear();
        player = nullptr;
        scene->setBackgroundBrush(Qt::NoBrush);
    }
}

void LevelManager::loadLevel(int level) {
    clearLevel();
    currentLevel = level;

    const int TILE = 40;

    // 背景
    QPixmap originalBg(":/images/maya_bg.png");
    if (!originalBg.isNull()) {
        QPixmap scaled = originalBg.scaled(800, 600, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        cachedBackground = applyOpacity(scaled, 0.5);
    }

    // 地图数据
    int m1[15][20] = {
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {1,1,1,1,0,0,2,1,1,1,1,0,0,1,1,2,1,1,1,3}
    };

    int m2[15][20] = {
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,1,1,0,0,0,1,1,0,0,0,1,1,0,0,0,0},
        {1,1,0,0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,0,0},
        {1,2,1,0,2,1,0,2,1,0,2,1,0,2,1,1,2,1,1,3}
    };

    int (*data)[20] = (level == 1) ? m1 : m2;

    // 创建地形
    for (int r = 0; r < 15; ++r) {
        for (int c = 0; c < 20; ++c) {
            int val = data[r][c];
            if (val == 0) continue;

            TerrainBlock *b = new TerrainBlock(c * TILE, r * TILE + 200, TILE);

            if (val == 1) {
                b->setData(0, "Ground");
            } else if (val == 2) {
                b->setBrush(Qt::yellow);
                b->setData(0, "Trap");
            } else if (val == 3) {
                b->setBrush(Qt::darkCyan);
                b->setData(0, "Door");
            }

            scene->addItem(b);
        }
    }

    // 创建玩家
    player = new Player();
    player->setPos(40, 100);
    scene->addItem(player);

    // 信号转发（关键！）
    player->setDeadHandler([this]() {
        emit playerDead();
    });

    player->setArrowRequestHandler([this](qreal arrowY) {
        scene->addItem(new Arrow(0, arrowY, 1));
    });

    player->setReachedDoorHandler([this]() {
        emit reachedDoor();
    });
}

QPixmap LevelManager::applyOpacity(const QPixmap &source, qreal opacity) {
    if (source.isNull()) return QPixmap();

    QPixmap transparentPixmap(source.size());
    transparentPixmap.fill(Qt::transparent);

    QPainter painter(&transparentPixmap);
    painter.setOpacity(opacity);
    painter.drawPixmap(0, 0, source);
    painter.end();

    return transparentPixmap;
}
