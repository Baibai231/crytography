#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QTimer>
#include <functional>
#include <QPixmap>
#include "aimanager.h"

class Player;
class LevelManager;
class InputController;
class UIManager;
class PuzzleManager;
class VideoController;

class GameWindow : public QGraphicsView {
    Q_OBJECT

public:
    explicit GameWindow(QWidget *parent = nullptr);

    void setBackToMenuHandler(const std::function<void()> &handler);
    void startNewGame();

    QPixmap applyOpacity(const QPixmap &source, qreal opacity);

    void handlePuzzleFailed();

protected:
    void showEvent(QShowEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void drawBackground(QPainter *painter, const QRectF &rect) override;


private:
    QGraphicsScene *scene = nullptr;
    QTimer *gameTimer = nullptr;

    // 新模块
    LevelManager *levelManager = nullptr;
    InputController *inputController = nullptr;
    UIManager *uiManager = nullptr;
    PuzzleManager *puzzleManager = nullptr;
    VideoController *videoController = nullptr;

    Player *player = nullptr;

    std::function<void()> backToMenuHandler;

    void onPlayerDead();
    void onReachedDoor();

    QPixmap cachedBackground;

    AIManager* aiManager;
};

#endif
