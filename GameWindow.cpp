#include "GameWindow.h"

#include "LevelManager.h"
#include "InputController.h"
#include "aimanager.h"
#include "PuzzleManager.h"
#include "VideoController.h"
#include "uimanager.h"
#include "GameProgress.h"

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
    aiManager->setApiKey("");


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

void GameWindow::startNewGame(int level) {

    QPixmap originalBg(":/images/maya_bg.png");
    if (!originalBg.isNull()) {
        QPixmap scaled = originalBg.scaled(800, 600, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        cachedBackground = applyOpacity(scaled, 0.5);
    }

    levelManager->loadLevel(level);
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

    int level = levelManager->getCurrentLevel();
    QString videoText;

    if (level == 1) {
        videoText = "石门缓缓开启，古老的凯撒密文在墙壁上闪烁...\n你感觉到一股来自罗马时代的力量。";
    } else if (level == 2) {
        videoText = "穿越破碎的栅栏，你来到一个充满电路与代码的密室...\n黑客留下的痕迹依然清晰。";
    } else if (level == 3) {
        videoText = "两面相对的镜子折射出无限的光芒...\n对称的祭坛等待密钥的唤醒。";
    } else {
        videoText = "黄金矩阵在黑暗中浮现，四个格子闪烁着神秘的光芒...\nMini-AES 的核心密钥，等待被破解。";
    }

    videoController->playVideo("placeholder_path", videoText, this, [this, level]() {
        bool success = puzzleManager->runPuzzle(level, this);

        if (success) {
            GameProgress::unlockLevel(level + 1);

            if (level == 1) {
                uiManager->showInfo("成功", "凯撒加密已破解！", this);
                levelManager->loadLevel(2);
            }
            else if (level == 2) {
                uiManager->showInfo("成功", "栅栏密码已破解！祭坛的轮廓正在显现。", this);
                levelManager->loadLevel(3);
            }
            else if (level == 3) {
                uiManager->showInfo("成功", "维吉尼亚对称密码已破解！矩阵实验室的门缓缓打开。", this);
                levelManager->loadLevel(4);
            }
            else {
                uiManager->showInfo("通关", "Mini-AES 矩阵之钥已被破解！神庙的力量已被你掌控。", this);
                hide();
                if (backToMenuHandler) {
                    backToMenuHandler();
                }
                return;
            }

            player = levelManager->getPlayer();
            inputController->setPlayer(player);
            gameTimer->start(16);
        }
        else {
            handlePuzzleFailed();
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

void GameWindow::handlePuzzleFailed() {
    gameTimer->stop();

    int result = uiManager->showDeathDialog(levelManager->getCurrentLevel(), this);

    if (result == 1) {
        // 重新开始本关
        QTimer::singleShot(0, this, [this]() {
            levelManager->loadLevel(levelManager->getCurrentLevel());

            player = levelManager->getPlayer();
            inputController->setPlayer(player);

            gameTimer->start(16);
        });
    }
    else if (result == 2) {
        // 返回菜单
        hide();
        if (backToMenuHandler) {
            backToMenuHandler();
        }
    }
    else {
        qApp->quit();
    }
}
