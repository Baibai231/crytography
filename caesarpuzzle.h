#ifndef CAESARPUZZLE_H
#define CAESARPUZZLE_H

#include <QDialog>

#include "aimanager.h"

// 前向声明，减少头文件依赖
class QLineEdit;
class QLabel;

class CaesarPuzzle : public QDialog {
    Q_OBJECT

public:
    explicit CaesarPuzzle(AIManager *manager,QWidget *parent = nullptr);

private slots:
    void checkAnswer(); // 检查玩家输入的槽函数

private:
    void setupPuzzle(); // 生成随机题目的内部逻辑

    QString answer;        // 正确原文
    QString encryptedText; // 加密后的密文
    int offset;            // 随机偏移量

    QLabel *infoLabel;
    QLineEdit *inputEdit;

    AIManager* aiManager;


protected:
    // 当点击对话框右上角“X”关闭时，Qt 默认会触发 reject()
    void reject() override {
        QDialog::reject(); // 保持默认行为
    }
};

#endif
