#include "GameWindow.h"

#include "LevelManager.h"
#include "InputController.h"
#include "aimanager.h"
#include "PuzzleManager.h"
#include "VideoController.h"
#include "uimanager.h"

#include <QKeyEvent>
#include <QPainter>
#include <QShowEvent>
#include <QApplication>

GameWindow::GameWindow(QWidget *parent)
    : QGraphicsView(parent) {

    scene = new QGraphicsScene(0, 0, 800, 600, this);
    setScene(scene);
    setSceneRect(0, 0, 800, 600);

    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, scene, &QGraphicsScene::advance);

    aiManager = new AIManager(this);
    aiManager->setApiKey("sk-");


    // 初始化各个模块
    levelManager = new LevelManager(scene, this);
    inputController = new InputController();

    puzzleManager = new PuzzleManager(aiManager,this);
    videoController = new VideoController(this);

    // 信号连接
    connect(levelManager, &LevelManager::playerDead, this, &GameWindow::onPlayerDead);
    connect(levelManager, &LevelManager::reachedDoor, this, &GameWindow::onReachedDoor);

    setFixedSize(800, 600);
    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    // 接收AI回复
    connect(aiManager, &AIManager::hintReady, this, [=](QString reply){
        qDebug() << "AI:" << reply;

        // TODO：后面接UI显示
    });

    connect(aiManager, &AIManager::errorOccurred, this, [=](QString err){
        qDebug() << "AI Error:" << err;
    });
}

void GameWindow::setBackToMenuHandler(const std::function<void()> &handler) {
    backToMenuHandler = handler;
}

void GameWindow::startNewGame() {

    QPixmap originalBg(":/images/maya_bg.png");
    if (!originalBg.isNull()) {
        QPixmap scaled = originalBg.scaled(800, 600, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        cachedBackground = applyOpacity(scaled, 0.5);
    }

    levelManager->loadLevel(1);
    player = levelManager->getPlayer();

    inputController->setPlayer(player);

    gameTimer->start(16);

    setFocus();
    activateWindow();
}

void GameWindow::showEvent(QShowEvent *event) {
    QGraphicsView::showEvent(event);
    setFocus();
    activateWindow();
}

void GameWindow::keyPressEvent(QKeyEvent *event) {
    inputController->handleKeyPress(event);
}

void GameWindow::keyReleaseEvent(QKeyEvent *event) {
    inputController->handleKeyRelease(event);
}

void GameWindow::onPlayerDead() {
    gameTimer->stop();

    int result = uiManager->showDeathDialog(levelManager->getCurrentLevel(), this);

    if (result == 1) {
        QTimer::singleShot(0, this, [this]() {
            levelManager->loadLevel(levelManager->getCurrentLevel());

            player = levelManager->getPlayer();
            inputController->setPlayer(player);

            gameTimer->start(16);
        });
    }
    else if (result == 2) {
        hide();
        if (backToMenuHandler) {
            backToMenuHandler();
        }
    }
    else {
        qApp->quit();
    }
}

void GameWindow::onReachedDoor() {
    gameTimer->stop();

    videoController->playVideo("placeholder_path", this, [this]() {

        bool success = puzzleManager->runPuzzle(levelManager->getCurrentLevel(), this);

        if (success) {
            if (levelManager->getCurrentLevel() == 1) {
                uiManager->showInfo("成功", "凯撒加密已破解！", this);

                levelManager->loadLevel(2);
                player = levelManager->getPlayer();
                inputController->setPlayer(player);
                gameTimer->start(16);
            }
            else {
                uiManager->showInfo("成功", "栅栏密码已破解！你已彻底清除病毒。", this);

                hide();
                if (backToMenuHandler) {
                    backToMenuHandler();
                }
            }
        }
        else {
            if (player) {
                player->forceKill();
            }
        }
    });
}

QPixmap GameWindow::applyOpacity(const QPixmap &source, qreal opacity) {
    if (source.isNull()) return QPixmap();

    QPixmap transparentPixmap(source.size());
    transparentPixmap.fill(Qt::transparent);

    QPainter painter(&transparentPixmap);
    painter.setOpacity(opacity);
    painter.drawPixmap(0, 0, source);
    painter.end();

    return transparentPixmap;
}

void GameWindow::drawBackground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect);

    QRectF targetRect(0, 0, 800, 600);

    if (!cachedBackground.isNull()) {
        painter->drawPixmap(targetRect, cachedBackground, cachedBackground.rect());
    } else {
        painter->fillRect(targetRect, QColor(240, 240, 240));
    }
}

