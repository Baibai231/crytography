#include "RailFencePuzzle.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QMessageBox>
#include <QPropertyAnimation>
#include <QPalette>
#include <QPixmap>
#include <QHBoxLayout>

RailFencePuzzle::RailFencePuzzle(QWidget *parent) : QDialog(parent) {
    setWindowTitle("安全终端 - 栅栏锁");
    setFixedSize(500, 600);

    // ✅ 背景 stone
    QPalette palette;
    palette.setBrush(QPalette::Window,
                     QBrush(QPixmap(":/images/stone.png").scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
    setAutoFillBackground(true);
    setPalette(palette);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 1. 标题
    QLabel *title = new QLabel("【第二关】破碎的栅栏 (栅栏密码)");
    title->setStyleSheet("font-size: 20px; font-weight: bold; color: #E67E22;");
    mainLayout->addWidget(title);

    // 2. 密文显示
    QLabel *cipherLabel = new QLabel("密文: HLOOLRDELW\n说明：黑客使用了 2 层栅栏进行加密。");
    cipherLabel->setStyleSheet(
        "font-family: monospace; font-size: 16px; background: #f0f0f0; "
        "padding: 10px; border: 1px solid #ccc;");
    mainLayout->addWidget(cipherLabel);

    // ✅ 3. 提示按钮（右对齐）
    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->addStretch();

    QPushButton *hintButton = new QPushButton("提示");
    hintButton->setFixedSize(80, 35);
    topLayout->addWidget(hintButton);

    mainLayout->addLayout(topLayout);

    connect(hintButton, &QPushButton::clicked, this, [=]() {
        QDialog hintDialog(this);
        hintDialog.setWindowTitle("提示");
        hintDialog.resize(350, 250);

        QVBoxLayout *layout = new QVBoxLayout(&hintDialog);

        QLabel *hintText = new QLabel(
            "什么是栅栏密码 (2层)？\n\n"
            "将字符串拆为两行再组合：\n"
            "第一行：H L O O L\n"
            "第二行：E L W R D\n\n"
            "解密：\n"
            "前一半 + 后一半 → 上下交替读取"
            );

        hintText->setWordWrap(true);
        layout->addWidget(hintText);

        hintDialog.setStyleSheet(R"(
            QDialog {
                background-color: #2c2c2c;
                color: white;
            }
            QLabel {
                font-size: 14px;
            }
        )");

        hintDialog.exec();
    });

    // 4. 输入框
    inputEdit = new QLineEdit();
    inputEdit->setPlaceholderText("请输入还原后的明文指令...");
    inputEdit->setFixedHeight(35);
    mainLayout->addWidget(inputEdit);

    // ✅ 回车直接验证（加分点）
    connect(inputEdit, &QLineEdit::returnPressed, this, &RailFencePuzzle::checkAnswer);

    // 5. 验证按钮
    QPushButton *btn2 = new QPushButton("验证指令");
    btn2->setFixedHeight(40);
    btn2->setStyleSheet("background-color: #E67E22; color: white; font-weight: bold;");
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

        QMessageBox::critical(this, "错误", "指令错误！请重新输入！");

        inputEdit->clear();
        inputEdit->setFocus();
    }
}
