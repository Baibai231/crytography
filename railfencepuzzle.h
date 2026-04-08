#ifndef RAILFENCEPUZZLE_H
#define RAILFENCEPUZZLE_H

#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>

#include "aimanager.h"
#include "gamestate.h"

class RailFencePuzzle : public QDialog {
    Q_OBJECT
public:
    explicit RailFencePuzzle(AIManager *aiManager,QWidget *parent = nullptr);

private slots:
    void checkAnswer();
    void requestHint();

private:
    QLineEdit *inputEdit;

    QString answer = "HELLOWORLD"; // 保留你的答案（虽然当前cpp没用）

    void shakeWindow();  // ✅ 必须新增（用于震动效果）

    AIManager *aiManager;
    GameState gameState;
    QJsonArray conversationHistory;

    int attemptCount = 0;
};

#endif
