#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QTimer>
#include <functional>
#include "Player.h"

class GameWindow : public QGraphicsView {
public:
    explicit GameWindow(QWidget *parent = nullptr);
    void setBackToMenuHandler(const std::function<void()> &handler);
    void startNewGame();

protected:
    void showEvent(QShowEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void drawBackground(QPainter *painter, const QRectF &rect) override;

private:
    QGraphicsScene *scene = nullptr;
    QGraphicsView *view = nullptr;
    Player *player = nullptr;
    QTimer *gameTimer = nullptr;

    void loadMap(int level);
    void clearLevel();
    void restartCurrentLevel();
    void onPlayerDead();
    void onReachedDoor();

    int currentLevel = 1;
    QPixmap cachedBackground;
    std::function<void()> backToMenuHandler;

    QPixmap applyOpacity(const QPixmap &source, qreal opacity);
};

#endif
