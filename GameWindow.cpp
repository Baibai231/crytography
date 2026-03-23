#include "GameWindow.h"
#include "TerrainBlock.h"
#include "Arrow.h"
#include "VideoPlayer.h"
#include "CaesarPuzzle.h"
#include "RailFencePuzzle.h"

#include <QDialog>
#include <QGraphicsRectItem>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QShowEvent>
#include <QLabel>
#include <QMessageBox>
#include <QPainter>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>

GameWindow::GameWindow(QWidget *parent) : QGraphicsView(parent) {
    scene = new QGraphicsScene(0, 0, 800, 600, this);
    setScene(scene);
    setSceneRect(0, 0, 800, 600);

    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, scene, &QGraphicsScene::advance);

    loadMap(1);

    setFixedSize(800, 600);
    setFocusPolicy(Qt::StrongFocus);
    setFocus();
}

void GameWindow::setBackToMenuHandler(const std::function<void()> &handler) {
    backToMenuHandler = handler;
}

void GameWindow::startNewGame() {
    loadMap(1);
    setFocus();
    activateWindow();
}

void GameWindow::loadMap(int level) {
    clearLevel();
    currentLevel = level;
    const int TILE = 40;

    QPixmap originalBg(":/images/maya_bg.png");
    if (!originalBg.isNull()) {
        QPixmap scaled = originalBg.scaled(800, 600, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        cachedBackground = applyOpacity(scaled, 0.5);
    }

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

    player = new Player();
    player->setPos(40, 100);
    scene->addItem(player);

    player->setDeadHandler([this]() { onPlayerDead(); });
    player->setArrowRequestHandler([this](qreal arrowY) {
        scene->addItem(new Arrow(0, arrowY, 1));
    });
    player->setReachedDoorHandler([this]() { onReachedDoor(); });

    if (gameTimer) {
        gameTimer->start(16);
    }
}

void GameWindow::clearLevel() {
    if (gameTimer) {
        gameTimer->stop();
    }

    if (scene) {
        scene->clear();
        player = nullptr;
        scene->setBackgroundBrush(Qt::NoBrush);
    }
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

void GameWindow::showEvent(QShowEvent *event) {
    QGraphicsView::showEvent(event);
    setFocus();
    activateWindow();
}

void GameWindow::keyPressEvent(QKeyEvent *event) {
    if (event->isAutoRepeat() || !player) return;

    switch (event->key()) {
    case Qt::Key_A:
        player->moveLeft();
        break;
    case Qt::Key_D:
        player->moveRight();
        break;
    case Qt::Key_W:
        player->jump();
        break;
    default:
        break;
    }
}

void GameWindow::keyReleaseEvent(QKeyEvent *event) {
    if (event->isAutoRepeat() || !player) return;

    switch (event->key()) {
    case Qt::Key_A:
        player->stopLeft();
        break;
    case Qt::Key_D:
        player->stopRight();
        break;
    default:
        break;
    }
}

void GameWindow::restartCurrentLevel() {
    loadMap(currentLevel);
    setFocus();
    activateWindow();
}

void GameWindow::onPlayerDead() {
    if (gameTimer) {
        gameTimer->stop();
    }

    QDialog dialog(this);
    dialog.setWindowTitle("战败");
    dialog.setModal(true);

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
    QLabel *titleLabel = new QLabel(QString("你在第 %1 关战败了！").arg(currentLevel), &dialog);
    QLabel *infoLabel = new QLabel("英雄，要重新挑战这一关吗？", &dialog);
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *retryBtn = new QPushButton("回到本关起点", &dialog);
    QPushButton *menuBtn = new QPushButton("返回主菜单", &dialog);
    QPushButton *exitBtn = new QPushButton("退出游戏", &dialog);

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(infoLabel);
    buttonLayout->addWidget(retryBtn);
    buttonLayout->addWidget(menuBtn);
    buttonLayout->addWidget(exitBtn);
    mainLayout->addLayout(buttonLayout);

    connect(retryBtn, &QPushButton::clicked, &dialog, [&dialog]() { dialog.done(1); });
    connect(menuBtn, &QPushButton::clicked, &dialog, [&dialog]() { dialog.done(2); });
    connect(exitBtn, &QPushButton::clicked, &dialog, [&dialog]() { dialog.done(3); });

    const int result = dialog.exec();

    if (result == 1) {
        QTimer::singleShot(0, this, [this]() { restartCurrentLevel(); });
    } else if (result == 2) {
        hide();
        if (backToMenuHandler) {
            backToMenuHandler();
        }
    } else {
        qApp->quit();
    }
}

void GameWindow::onReachedDoor() {
    if (gameTimer) gameTimer->stop();

    VideoPlayer *v = new VideoPlayer("placeholder_path", this);

    connect(v, &VideoPlayer::finished, this, [this]() {
        bool success = false;

        if (currentLevel == 1) {
            CaesarPuzzle p1(this);
            if (p1.exec() == QDialog::Accepted) {
                QMessageBox::information(this, "成功", "凯撒加密已破解！");
                loadMap(2);
                success = true;
            }
        } else {
            RailFencePuzzle p2(this);
            if (p2.exec() == QDialog::Accepted) {
                QMessageBox::information(this, "成功", "栅栏密码已破解！你已彻底清除病毒。");
                hide();
                if (backToMenuHandler) {
                    backToMenuHandler();
                }
                success = true;
            }
        }

        if (!success && player) {
            TerrainBlock *spike = new TerrainBlock(player->x(), player->y(), 40);
            spike->setBrush(Qt::red);
            spike->setData(0, "Arrow");
            scene->addItem(spike);
            player->forceKill();
        }
    });

    v->play();
}

void GameWindow::drawBackground(QPainter *painter, const QRectF &rect) {
    Q_UNUSED(rect);
    QRectF targetRect(0, 0, 800, 600);

    if (!cachedBackground.isNull()) {
        painter->drawPixmap(targetRect, cachedBackground, cachedBackground.rect());
    } else {
        painter->fillRect(targetRect, QColor(240, 240, 240));
    }
}
