#include "aichatdialog.h"

AIChatDialog::AIChatDialog(AIManager *manager, QWidget *parent)
    : QDialog(parent), aiManager(manager)
{
    setWindowTitle("神秘守护者");
    setFixedSize(420, 500);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    countLabel = new QLabel("剩余提问次数：3", this);
    countLabel->setStyleSheet("color:#e4b45f; font-weight:bold;");

    QScrollArea *scroll = new QScrollArea(this);
    QWidget *container = new QWidget();
    chatLayout = new QVBoxLayout(container);
    chatLayout->setAlignment(Qt::AlignTop);
    scroll->setWidget(container);
    scroll->setWidgetResizable(true);

    inputEdit = new QLineEdit(this);
    QPushButton *sendBtn = new QPushButton("发送", this);

    mainLayout->addWidget(countLabel);
    mainLayout->addWidget(scroll);
    mainLayout->addWidget(inputEdit);
    mainLayout->addWidget(sendBtn);

    connect(sendBtn, &QPushButton::clicked, this, [=]() {
        if (askCount >= maxAsk) return;

        QString text = inputEdit->text().trimmed();
        if (text.isEmpty()) return;

        addMessage(text, true);
        inputEdit->clear();

        askCount++;
        countLabel->setText(QString("剩余提问次数：%1").arg(maxAsk - askCount));

        PuzzleContext ctx;
        ctx.type = "caesar";
        ctx.content = "当前谜题";
        ctx.progress = "玩家提问中";

        aiManager->askAI(ctx, text);
    });

    connect(aiManager, &AIManager::responseReady, this, [=](QString reply){
        addMessage(reply, false);
    });
}

void AIChatDialog::addMessage(const QString &text, bool isUser)
{
    QWidget *bubble = new QWidget;
    QVBoxLayout *bubbleLayout = new QVBoxLayout(bubble);
    bubbleLayout->setContentsMargins(10, 10, 10, 10);

    QString cleanText = text;

    // 去掉首尾空白（包括 \n）
    cleanText = cleanText.trimmed();

    // ❗关键：去掉多余换行（特别是开头）
    while (cleanText.startsWith("\n")) {
        cleanText.remove(0, 1);
    }

    // ❗可选：限制连续空行（防止AI乱排版）
    cleanText.replace("\n\n\n", "\n\n");

    QLabel *msg = new QLabel(cleanText);
    msg->setWordWrap(true);
    msg->setMaximumWidth(240);

    // 🔥 关键修复点
    msg->setTextFormat(Qt::PlainText);

    bubbleLayout->addWidget(msg);

    if (isUser) {
        bubble->setStyleSheet(
            "background:#d7a84e;"
            "color:#1f160e;"
            "border-radius:10px;"
            );
        chatLayout->addWidget(bubble, 0, Qt::AlignRight);
    } else {
        bubble->setStyleSheet(
            "background:#2c1f16;"
            "color:#efe7d8;"
            "border-radius:10px;"
            );
        chatLayout->addWidget(bubble, 0, Qt::AlignLeft);
    }
}
