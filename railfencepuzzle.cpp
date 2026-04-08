#include "RailFencePuzzle.h"
#include "UIManager.h"
#include "aichatdialog.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QPropertyAnimation>
#include <QHBoxLayout>
#include <QFrame>

RailFencePuzzle::RailFencePuzzle(AIManager *manager, QWidget *parent) : QDialog(parent), aiManager(manager) {

    setWindowTitle("安全终端 - 栅栏锁");
    setFixedSize(500, 520);

    gameState.puzzleType = "rail_fence";
    gameState.encryptedText = "HLOOLRDELW";
    gameState.attemptCount = 0;
    gameState.solved = false;

    setStyleSheet(R"(
        QDialog {
            background-color: #151920;
        }
        QLabel {
            color: #f0e8d8;
            font-family: 'Microsoft YaHei';
        }
        QLabel[role="title"] {
            font-size: 23px;
            font-weight: 700;
            color: #e39b4a;
        }
        QLabel[role="subtitle"] {
            font-size: 14px;
            color: #cfc3af;
        }
        QFrame[role="panel"] {
            background-color: rgba(41, 31, 23, 178);
            border-radius: 14px;
            border: 1px solid rgba(227, 155, 74, 90);
        }
        QLabel[role="cipher"] {
            font-family: Consolas, 'Courier New', monospace;
            font-size: 24px;
            font-weight: 700;
            color: #ffd89e;
        }
        QLabel[role="hint"] {
            font-size: 15px;
            color: #e6d8bf;
        }
        QLineEdit {
            background-color: #f7f0e3;
            color: #231912;
            border: 2px solid #76502f;
            border-radius: 8px;
            padding: 10px 12px;
            font-size: 15px;
        }
        QLineEdit:focus {
            border-color: #e39b4a;
        }
        QPushButton {
            border-radius: 8px;
            min-height: 38px;
            padding: 0 16px;
            font-size: 15px;
            font-weight: 700;
        }
        QPushButton[variant="primary"] {
            background-color: #e39b4a;
            color: #20160e;
            border: none;
        }
        QPushButton[variant="primary"]:hover {
            background-color: #f0ae5d;
        }
        QPushButton[variant="secondary"] {
            background-color: rgba(78, 58, 40, 215);
            color: #f7eedb;
            border: 1px solid rgba(234, 197, 141, 90);
        }
        QPushButton[variant="secondary"]:hover {
            background-color: rgba(101, 76, 53, 225);
        }
    )");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(28, 24, 28, 24);
    mainLayout->setSpacing(16);

    QLabel *title = new QLabel("第二关：破碎的栅栏");
    title->setProperty("role", "title");
    mainLayout->addWidget(title);

    QLabel *subtitle = new QLabel("把密文拆回两层轨道，再按上下交替还原明文", this);
    subtitle->setProperty("role", "subtitle");
    mainLayout->addWidget(subtitle);

    QFrame *cipherPanel = new QFrame(this);
    cipherPanel->setProperty("role", "panel");

    QVBoxLayout *cipherLayout = new QVBoxLayout(cipherPanel);
    cipherLayout->setContentsMargins(20, 18, 20, 18);
    cipherLayout->setSpacing(10);

    QLabel *cipherTitle = new QLabel("截获密文", cipherPanel);
    cipherTitle->setProperty("role", "hint");

    QLabel *cipherLabel = new QLabel("HLOOLRDELW", cipherPanel);
    cipherLabel->setProperty("role", "cipher");
    cipherLabel->setAlignment(Qt::AlignCenter);

    QLabel *cipherHint = new QLabel("黑客使用了 2 层栅栏进行加密。", cipherPanel);
    cipherHint->setProperty("role", "hint");
    cipherHint->setWordWrap(true);

    cipherLayout->addWidget(cipherTitle);
    cipherLayout->addWidget(cipherLabel);
    cipherLayout->addWidget(cipherHint);

    mainLayout->addWidget(cipherPanel);

    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->addStretch();

    QPushButton *hintButton = new QPushButton("询问神秘提示");
    hintButton->setFixedSize(140, 36);
    hintButton->setProperty("variant", "secondary");
    hintButton->setCursor(Qt::PointingHandCursor);
    topLayout->addWidget(hintButton);

    mainLayout->addLayout(topLayout);

    // ✅ 打开AI聊天窗口（核心修改）
    connect(hintButton, &QPushButton::clicked, this, [=]() {

        AIChatDialog dialog(aiManager, this);

        // ✅ 设置关卡上下文（核心）
        GameState state;
        state.puzzleType = "rail_fence";
        state.encryptedText = gameState.encryptedText;
        state.userInput = "";
        state.attemptCount = 0;
        state.solved = false;

        dialog.setGameState(state);

        dialog.exec();
    });


    inputEdit = new QLineEdit();
    inputEdit->setPlaceholderText("请输入还原后的明文指令...");
    inputEdit->setFixedHeight(45);
    mainLayout->addWidget(inputEdit);

    connect(inputEdit, &QLineEdit::returnPressed, this, &RailFencePuzzle::checkAnswer);

    QPushButton *btn2 = new QPushButton("验证指令");
    btn2->setFixedHeight(40);
    btn2->setProperty("variant", "primary");
    btn2->setCursor(Qt::PointingHandCursor);
    connect(btn2, &QPushButton::clicked, this, &RailFencePuzzle::checkAnswer);
    mainLayout->addWidget(btn2);

    mainLayout->addStretch();
}


// ✅ 震动效果
void RailFencePuzzle::shakeWindow() {
    QPoint originalPos = this->pos();

    QPropertyAnimation *animation = new QPropertyAnimation(this, "pos");
    animation->setDuration(300);

    animation->setKeyValueAt(0, originalPos);
    animation->setKeyValueAt(0.1, originalPos + QPoint(-10, 0));
    animation->setKeyValueAt(0.2, originalPos + QPoint(10, 0));
    animation->setKeyValueAt(0.3, originalPos + QPoint(-10, 0));
    animation->setKeyValueAt(0.4, originalPos + QPoint(10, 0));
    animation->setKeyValueAt(0.5, originalPos);

    animation->start(QAbstractAnimation::DeleteWhenStopped);
}


// ❗ 核心逻辑完全不动
void RailFencePuzzle::checkAnswer() {
    if (inputEdit->text().trimmed().toUpper() == "HELLOWORLD") {
        accept();
    } else {
        shakeWindow();

        UIManager::showErrorDialog("错误", "指令错误！请重新输入！", this);
        reject();
        inputEdit->clear();
        inputEdit->setFocus();
    }
}

void RailFencePuzzle::requestHint()
{
    gameState.attemptCount = attemptCount;

    aiManager->requestHint(gameState, conversationHistory);
}
