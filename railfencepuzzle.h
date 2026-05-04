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

    QString answer = "HELLOWORLD";

    void shakeWindow();  // 震动效果

    AIManager *aiManager;
    GameState gameState;
    QJsonArray conversationHistory;

    int attemptCount = 0;
};

#endif
