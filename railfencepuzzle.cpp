#include "RailFencePuzzle.h"
#include <QGroupBox>    // 修复点
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QMessageBox>

RailFencePuzzle::RailFencePuzzle(QWidget *parent) : QDialog(parent) {
    setWindowTitle("安全终端 - 栅栏锁");
    setFixedSize(500, 600); // 稍微调高高度以容纳说明文字

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 1. 标题
    QLabel *title = new QLabel("【第二关】破碎的栅栏 (栅栏密码)");
    title->setStyleSheet("font-size: 20px; font-weight: bold; color: #E67E22;");
    mainLayout->addWidget(title);

    // 2. 密文显示区域
    QLabel *cipherLabel = new QLabel("密文: HLOOLRDELW\n说明：黑客使用了 2 层栅栏进行加密。");
    cipherLabel->setStyleSheet("font-family: monospace; font-size: 16px; background: #f0f0f0; padding: 10px; border: 1px solid #ccc;");
    mainLayout->addWidget(cipherLabel);

    // 3. 详细解密提示（QGroupBox 区域）
    QGroupBox *tipBox = new QGroupBox("解密指南 (新手必读)");
    QVBoxLayout *tipLayout = new QVBoxLayout(tipBox);

    QLabel *helpText = new QLabel(
        "什么是栅栏密码 (2层)？\n\n"
        "就像把一段话拆成两行写，然后再拼起来：\n"
        "第一行：H   L   O   O   L  (取第1,3,5,7,9个字母)\n"
        "第二行：  E   L   W   R   D(取第2,4,6,8,10个字母)\n\n"
        "解密方法：\n"
        "1. 密文共10个字母，前5个是第一行，后5个是第二行。\n"
        "2. 第一行：H L O O L\n"
        "3. 第二行：R D E L W (注意：示例密文对应不同单词)\n"
        "4. 像拉链一样上下交替读取：1上 -> 1下 -> 2上 -> 2下..."
        );
    helpText->setWordWrap(true);
    helpText->setStyleSheet("color: #555; font-size: 13px;");
    tipLayout->addWidget(helpText);
    mainLayout->addWidget(tipBox);

    // 4. 输入区域
    inputEdit = new QLineEdit();
    inputEdit->setPlaceholderText("请输入还原后的明文指令...");
    inputEdit->setFixedHeight(35);
    mainLayout->addWidget(inputEdit);

    // 5. 验证按钮
    QPushButton *btn2 = new QPushButton("验证指令");
    btn2->setFixedHeight(40);
    btn2->setStyleSheet("background-color: #E67E22; color: white; font-weight: bold;");
    connect(btn2, &QPushButton::clicked, this, &RailFencePuzzle::checkAnswer);
    mainLayout->addWidget(btn2);

    mainLayout->addStretch(); // 底部留白
}

void RailFencePuzzle::checkAnswer() {
    // 假设正确答案是 HELLOWORLD
    if (inputEdit->text().trimmed().toUpper() == "HELLOWORLD") {
        accept();
    } else {
        QMessageBox::critical(this, "错误", "指令错误！监测到非法入侵，陷阱已激活！");
        reject(); // 触发 GameWindow 里的红色尖刺和死亡逻辑
    }
}
