#pragma once

#include <QDialog>
#include <QJsonArray>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QScrollArea>
#include "aimanager.h"
#include "gamestate.h"

class QResizeEvent;
class QShowEvent;

class AIChatDialog : public QDialog {
    Q_OBJECT

public:
    explicit AIChatDialog(AIManager *manager, QWidget *parent = nullptr);

    void setGameState(const GameState &state);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;

private:
    AIManager *aiManager;
    QVBoxLayout *chatLayout;
    QLabel *countLabel;
    QLineEdit *inputEdit;
    QScrollArea *chatScrollArea;
    QWidget *chatContainer;
    QPushButton *sendButton = nullptr;
    QJsonArray conversationHistory;

    GameState gameState;
    bool waitingForReply = false;
    int askCount = 0;
    const int maxAsk = 3;

    void addMessage(const QString &text, bool isUser);
    void updateMessageBubbleWidths();
    void updateInputState();
    void sendMessage();
};
