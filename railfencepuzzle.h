#ifndef RAILFENCEPUZZLE_H
#define RAILFENCEPUZZLE_H

#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>

class RailFencePuzzle : public QDialog {
    Q_OBJECT
public:
    explicit RailFencePuzzle(QWidget *parent = nullptr);

private slots:
    void checkAnswer();

private:
    QLineEdit *inputEdit;

    QString answer = "HELLOWORLD"; // 保留你的答案（虽然当前cpp没用）

    void shakeWindow();  // ✅ 必须新增（用于震动效果）
};

#endif
