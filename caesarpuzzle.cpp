#include "CaesarPuzzle.h"
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QRandomGenerator>
#include <QMessageBox>

CaesarPuzzle::CaesarPuzzle(QWidget *parent) : QDialog(parent) {
    setWindowTitle("古老的封印");
    setFixedSize(400, 280);
    // 使用 QSS 设置深色系解谜风格
    setStyleSheet("QDialog { background-color: #2c3e50; } "
                  "QLabel { color: #ecf0f1; font-family: 'Microsoft YaHei'; }");

    setupPuzzle(); // 初始化题目

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(15);

    QLabel *title = new QLabel("<b>石门上的刻痕</b>", this);
    title->setStyleSheet("font-size: 22px; color: #f1c40f;"); // 金色标题
    title->setAlignment(Qt::AlignCenter);

    infoLabel = new QLabel(this);
    infoLabel->setText(QString("密文: <font color='#e74c3c' size='6'>%1</font><br>提示（偏移量）: %2")
                           .arg(encryptedText).arg(offset));
    infoLabel->setAlignment(Qt::AlignCenter);

    inputEdit = new QLineEdit(this);
    inputEdit->setPlaceholderText("在此键入解密后的咒语...");
    inputEdit->setStyleSheet("padding: 10px; font-size: 16px; border: 2px solid #34495e; border-radius: 5px;");
    inputEdit->setAlignment(Qt::AlignCenter);

    QPushButton *btn = new QPushButton("破译封印", this);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setStyleSheet("QPushButton { background-color: #27ae60; color: white; font-weight: bold; height: 45px; border-radius: 5px; font-size: 18px; }"
                       "QPushButton:hover { background-color: #2ecc71; }");

    layout->addWidget(title);
    layout->addWidget(infoLabel);
    layout->addWidget(inputEdit);
    layout->addWidget(btn);

    // 连接信号
    connect(btn, &QPushButton::clicked, this, &CaesarPuzzle::checkAnswer);
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
        QMessageBox::information(this, "成功", "石门发出沉重的响声，缓缓开启了！");
        accept(); // 关闭对话框并返回 QDialog::Accepted
    } else {
        QMessageBox::critical(this, "失败", "咒语无效，请重新审视密文。");
        reject();
    }
}
