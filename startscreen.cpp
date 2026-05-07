#include "StartScreen.h"
#include "LevelSelectDialog.h"
#include "UIManager.h"

StartScreen::StartScreen(QWidget *parent) : QWidget(parent) {
    setFixedSize(800, 600);

    mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignCenter);
    mainLayout->setSpacing(20);

    titleLabel = new QLabel("像素大冒险", this);

    startBtn = new QPushButton("开始游戏", this);
    selectLevelBtn = new QPushButton("选择关卡", this);
    howToPlayBtn = new QPushButton("游戏玩法", this);
    releaseBtn = new QPushButton("发行说明", this);

    // Add to layout
    mainLayout->addWidget(titleLabel);
    mainLayout->addSpacing(30);
    mainLayout->addWidget(startBtn);
    mainLayout->addWidget(selectLevelBtn); // Added
    mainLayout->addWidget(howToPlayBtn);
    mainLayout->addWidget(releaseBtn);

    // Signals
    connect(startBtn, &QPushButton::clicked, this, &StartScreen::startClicked);
    connect(selectLevelBtn, &QPushButton::clicked, this, &StartScreen::showLevelSelect);
    connect(howToPlayBtn, &QPushButton::clicked, this, &StartScreen::showHowToPlay);
    connect(releaseBtn, &QPushButton::clicked, this, &StartScreen::showReleaseNotes);

    applyStyle();
}

void StartScreen::showLevelSelect() {
    LevelSelectDialog dialog(this);
    connect(&dialog, &LevelSelectDialog::levelSelected, this, &StartScreen::levelSelected);
    dialog.exec();
}

void StartScreen::applyStyle() {
    QString style = R"(
        QWidget { background-color: #2c1f16; }
        QLabel { color: #f8f3e8; font-size: 50px; font-weight: bold; font-family: "Microsoft YaHei"; }
        QPushButton { 
            background-color: #d7a84e; color: white; border-radius: 10px; padding: 10px; 
            min-width: 200px; font-size: 20px; 
        }
        QPushButton:hover { background-color: #c8973f; }
    )";

    this->setStyleSheet(style);
}

void StartScreen::showHowToPlay() {
    UIManager::showInfoDialog("游戏玩法",
                              "A 键：向左移动\n"
                              "D 键：向右移动\n"
                              "W 键：跳跃\n\n"
                              "躲避陷阱，抵达石门并破解密码机关。",
                              this);
}

void StartScreen::showReleaseNotes() {
    UIManager::showInfoDialog("发行说明",
                              "版本：v1.0.0\n"
                              "开发者：Baibai\n"
                              "功能：基础物理引擎、碰撞检测、死亡重开与密码解谜。",
                              this);
}
