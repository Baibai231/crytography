#include "CaesarPuzzle.h"
#include "aimanager.h"
#include "uimanager.h"
#include "aichatdialog.h"
#include "spinwheeldialog.h"

#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRandomGenerator>

CaesarPuzzle::CaesarPuzzle(AIManager *manager,QWidget *parent) : QDialog(parent), aiManager(manager) {

    setWindowTitle("古老的封印");
    setFixedSize(460, 400);
    setStyleSheet(R"(
        QDialog {
            background-color: #161a20;
        }
        QLabel {
            color: #efe7d8;
            font-family: 'Microsoft YaHei';
        }
        QLabel[role="title"] {
            font-size: 24px;
            font-weight: 700;
            color: #e4b45f;
        }
        QLabel[role="subtitle"] {
            color: #e2d5bd;
            font-size: 14px;
        }
        QFrame[role="panel"] {
            background-color: rgba(44, 31, 22, 170);
            border: 1px solid rgba(228, 180, 95, 95);
            border-radius: 14px;
        }
        QLabel[role="cipher"] {
            color: #ffd58a;
            font-size: 28px;
            font-weight: 700;
            letter-spacing: 3px;
        }
        QLabel[role="hint"] {
            color: #eadfca;
            font-size: 15px;
        }
        QLabel[role="offset"] {
            color: #ffe29f;
            font-size: 20px;
            font-weight: 700;
        }
        QLineEdit {
            padding: 10px 12px;
            font-size: 16px;
            border: 2px solid #6b5638;
            border-radius: 8px;
            background-color: #f8f3e8;
            color: #231912;
        }
        QLineEdit:focus {
            border-color: #e4b45f;
        }
        QPushButton {
            background-color: #d7a84e;
            color: #1f160e;
            font-weight: 700;
            min-height: 44px;
            border-radius: 8px;
            font-size: 17px;
            border: none;
        }
        QPushButton:hover {
            background-color: #ecc166;
        }
        QPushButton:pressed {
            background-color: #c8973f;
        }
    )");

    setupPuzzle(); // 初始化题目

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(28, 26, 28, 26);
    layout->setSpacing(18);

    QLabel *title = new QLabel("石门上的刻痕", this);
    title->setProperty("role", "title");
    title->setAlignment(Qt::AlignCenter);

    QLabel *subtitle = new QLabel("观察刻痕规律，输入正确咒语解除封印", this);
    subtitle->setProperty("role", "subtitle");
    subtitle->setAlignment(Qt::AlignCenter);

    QFrame *panel = new QFrame(this);
    panel->setProperty("role", "panel");
    panel->setMinimumHeight(150);

    QVBoxLayout *panelLayout = new QVBoxLayout(panel);
    panelLayout->setContentsMargins(20, 20, 20, 20);
    panelLayout->setSpacing(14);

    QLabel *cipherTitle = new QLabel("当前密文", panel);
    cipherTitle->setProperty("role", "hint");
    cipherTitle->setAlignment(Qt::AlignCenter);

    infoLabel = new QLabel(encryptedText, panel);
    infoLabel->setProperty("role", "cipher");
    infoLabel->setAlignment(Qt::AlignCenter);

    QLabel *offsetLabel = new QLabel("偏移量未知，需要自行推理", panel);
    offsetLabel->setProperty("role", "offset");
    offsetLabel->setAlignment(Qt::AlignCenter);

    panelLayout->addWidget(cipherTitle);
    panelLayout->addWidget(infoLabel);
    panelLayout->addWidget(offsetLabel);

    inputEdit = new QLineEdit(this);
    inputEdit->setPlaceholderText("在此键入解密后的咒语...");
    inputEdit->setAlignment(Qt::AlignCenter);

    QPushButton *visualBtn = new QPushButton("进入解密转盘", this);
    visualBtn->setCursor(Qt::PointingHandCursor);


    QPushButton *btn = new QPushButton("破译封印", this);
    btn->setCursor(Qt::PointingHandCursor);
    connect(inputEdit, &QLineEdit::returnPressed, this, &CaesarPuzzle::checkAnswer);

    QPushButton *hintBtn = new QPushButton("询问神秘提示", this);
    hintBtn->setCursor(Qt::PointingHandCursor);

    layout->addWidget(title);
    layout->addWidget(subtitle);
    layout->addWidget(panel);
    layout->addSpacing(10);
    layout->addWidget(inputEdit);


    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->setSpacing(12);
    bottomLayout->setContentsMargins(0, 0, 0, 0);

    hintBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    visualBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    hintBtn->setMinimumHeight(42);
    visualBtn->setMinimumHeight(42);

    bottomLayout->addWidget(hintBtn);
    bottomLayout->addWidget(visualBtn);

    layout->addWidget(btn);
    layout->addLayout(bottomLayout);

    // 连接信号
    connect(btn, &QPushButton::clicked, this, &CaesarPuzzle::checkAnswer);

    connect(visualBtn, &QPushButton::clicked, this, [=]() {

        SpinWheelDialog *dlg = new SpinWheelDialog(
            encryptedText,
            answer,
            offset,
            this
            );

        dlg->exec();
    });

    connect(hintBtn, &QPushButton::clicked, this, [=]() {
        AIChatDialog dialog(aiManager, this);

        GameState state;
        state.puzzleType = "caesar";
        state.encryptedText = encryptedText;
        state.userInput = "";
        state.attemptCount = 0;
        state.solved = false;

        // ✅ 传入AI对话框
        dialog.setGameState(state);


        dialog.exec();
    });
}

void CaesarPuzzle::setupPuzzle() {
    // 1. 词库
    QStringList words = {"PIXEL", "SWORD", "LEVEL", "HERO", "MAGIC", "QUEST", "DOOR", "GATE"};
    answer = words[QRandomGenerator::global()->bounded(words.size())];

    // 2. 随机偏移量 (1-5)
    offset = QRandomGenerator::global()->bounded(1, 6);

    // 3. 凯撒加密核心逻辑
    encryptedText = "";
    for(QChar c : answer) {
        int originalPos = c.toLatin1() - 'A';
        int newPos = (originalPos + offset) % 26;
        encryptedText.append(QChar('A' + newPos));
    }
}

void CaesarPuzzle::checkAnswer() {
    if (inputEdit->text().trimmed().toUpper() == answer) {
        UIManager::showInfoDialog("成功", "石门发出沉重的响声，缓缓开启了！", this);
        accept(); // 关闭对话框并返回 QDialog::Accepted
    } else {
        UIManager::showErrorDialog("失败", "咒语无效，请重新审视密文。", this);
        reject();
    }
}
