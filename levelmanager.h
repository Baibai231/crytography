#ifndef LEVELMANAGER_H
#define LEVELMANAGER_H

#include <QObject>
#include <QGraphicsScene>
#include <QPixmap>
#include "Player.h"

class LevelManager : public QObject {
    Q_OBJECT

public:
    explicit LevelManager(QGraphicsScene *scene, QObject *parent = nullptr);

    void loadLevel(int level);
    void clearLevel();

    Player* getPlayer() const;
    int getCurrentLevel() const;

signals:
    void playerDead();
    void reachedDoor();

private:
    QGraphicsScene *scene;
    Player *player = nullptr;
    int currentLevel = 1;
    QPixmap cachedBackground;

    QPixmap applyOpacity(const QPixmap &source, qreal opacity);
};

#endif
