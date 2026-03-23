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
    QString answer = "HELLOWORLD"; // 对应密文 HLOOLRDELW 的原文
};

#endif
