#pragma once

#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QScrollArea>
#include "aimanager.h"

class AIChatDialog : public QDialog {
    Q_OBJECT

public:
    explicit AIChatDialog(AIManager *manager, QWidget *parent = nullptr);

private:
    AIManager *aiManager;
    QVBoxLayout *chatLayout;
    QLabel *countLabel;
    QLineEdit *inputEdit;
    int askCount = 0;
    const int maxAsk = 3;

    void addMessage(const QString &text, bool isUser);
};
