#include "aichatdialog.h"

#include <QFontMetrics>
#include <QFrame>
#include <QJsonObject>
#include <QLabel>
#include <QLayoutItem>
#include <QLineEdit>
#include <QPushButton>
#include <QResizeEvent>
#include <QScrollBar>
#include <QTimer>

namespace {

constexpr int kBubbleTextPadding = 6;
constexpr int kBubbleMinTextWidth = 80;
constexpr qreal kUserBubbleMaxRatio = 0.65;
constexpr qreal kAssistantBubbleMaxRatio = 0.75;

}

AIChatDialog::AIChatDialog(AIManager *manager, QWidget *parent)
    : QDialog(parent), aiManager(manager)
{
    setWindowTitle("神秘守护者");
    resize(420, 500);
    setMinimumSize(360, 420);

    puzzleContext.type = "caesar";
    puzzleContext.content = "当前谜题";
    puzzleContext.progress = "玩家提问中";

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    countLabel = new QLabel("剩余提问次数：3", this);
    countLabel->setStyleSheet("color:#e4b45f; font-weight:bold;");

    chatScrollArea = new QScrollArea(this);
    chatContainer = new QWidget();
    chatLayout = new QVBoxLayout(chatContainer);
    chatLayout->setAlignment(Qt::AlignTop);
    chatLayout->setContentsMargins(8, 8, 8, 8);
    chatLayout->setSpacing(10);
    chatScrollArea->setWidget(chatContainer);
    chatScrollArea->setWidgetResizable(true);
    chatScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    chatScrollArea->setFrameShape(QFrame::NoFrame);

    inputEdit = new QLineEdit(this);
    sendButton = new QPushButton("发送", this);

    mainLayout->addWidget(countLabel);
    mainLayout->addWidget(chatScrollArea);
    mainLayout->addWidget(inputEdit);
    mainLayout->addWidget(sendButton);

    connect(sendButton, &QPushButton::clicked, this, &AIChatDialog::sendMessage);
    connect(inputEdit, &QLineEdit::returnPressed, this, &AIChatDialog::sendMessage);

    connect(aiManager, &AIManager::responseReady, this, [=](QString reply){
        waitingForReply = false;

        QJsonObject assistantMessage;
        assistantMessage["role"] = "assistant";
        assistantMessage["content"] = reply;
        conversationHistory.append(assistantMessage);

        addMessage(reply, false);
        updateInputState();
    });

    connect(aiManager, &AIManager::errorOccurred, this, [=](QString error) {
        waitingForReply = false;
        addMessage("提示请求失败：" + error, false);
        updateInputState();
    });

    updateInputState();
}

void AIChatDialog::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    updateMessageBubbleWidths();
}

void AIChatDialog::addMessage(const QString &text, bool isUser)
{
    QWidget *bubble = new QWidget;
    bubble->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    bubble->setProperty("isUserBubble", isUser);

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
    msg->setTextFormat(Qt::PlainText);
    msg->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    msg->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    msg->setProperty("messageText", true);

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

    updateMessageBubbleWidths();

    QTimer::singleShot(0, this, [this, bubble]() {
        if (!chatScrollArea || !bubble) {
            return;
        }

        chatScrollArea->ensureWidgetVisible(bubble, 0, 12);

        QScrollBar *scrollBar = chatScrollArea->verticalScrollBar();
        scrollBar->setValue(scrollBar->maximum());
    });
}

void AIChatDialog::updateMessageBubbleWidths()
{
    if (!chatContainer) {
        return;
    }

    const int availableWidth = chatScrollArea->viewport()->width();

    for (int i = 0; i < chatLayout->count(); ++i) {
        QLayoutItem *item = chatLayout->itemAt(i);
        QWidget *bubble = item ? item->widget() : nullptr;
        if (!bubble) {
            continue;
        }

        const bool isUserBubble = bubble->property("isUserBubble").toBool();
        const qreal widthRatio = isUserBubble ? kUserBubbleMaxRatio : kAssistantBubbleMaxRatio;
        const int maxTextWidth = qMax(kBubbleMinTextWidth, static_cast<int>(availableWidth * widthRatio));

        const QList<QLabel *> labels = bubble->findChildren<QLabel *>();
        for (QLabel *label : labels) {
            if (!label->property("messageText").toBool()) {
                continue;
            }

            const QString labelText = label->text();

            int naturalTextWidth = 0;
            const QStringList lines = labelText.split('\n');
            for (const QString &line : lines) {
                const QString measuredLine = line.isEmpty() ? QStringLiteral(" ") : line;
                naturalTextWidth = qMax(naturalTextWidth, label->fontMetrics().horizontalAdvance(measuredLine));
            }

            const int textWidth = qBound(
                kBubbleMinTextWidth,
                naturalTextWidth + kBubbleTextPadding,
                maxTextWidth
            );

            label->setFixedWidth(textWidth);

            const int textHeight = label->fontMetrics().boundingRect(
                QRect(0, 0, textWidth, 10000),
                Qt::TextWordWrap | Qt::AlignLeft | Qt::AlignTop,
                labelText
            ).height();
            label->setMinimumHeight(textHeight + 4);
        }

        bubble->adjustSize();
    }

    chatContainer->adjustSize();
}

void AIChatDialog::updateInputState()
{
    const bool limitReached = askCount >= maxAsk;
    const bool canSend = !waitingForReply && !limitReached;

    inputEdit->setEnabled(canSend);
    sendButton->setEnabled(canSend);

    if (waitingForReply) {
        inputEdit->setPlaceholderText("神秘守护者正在思考...");
        return;
    }

    if (limitReached) {
        inputEdit->setPlaceholderText("提问次数已用完");
        return;
    }

    inputEdit->setPlaceholderText("请输入你想追问的内容...");
}

void AIChatDialog::sendMessage()
{
    if (waitingForReply || askCount >= maxAsk) {
        return;
    }

    const QString text = inputEdit->text().trimmed();
    if (text.isEmpty()) {
        return;
    }

    addMessage(text, true);
    inputEdit->clear();

    QJsonObject userMessage;
    userMessage["role"] = "user";
    userMessage["content"] = text;
    conversationHistory.append(userMessage);

    ++askCount;
    countLabel->setText(QString("剩余提问次数：%1").arg(maxAsk - askCount));

    waitingForReply = true;
    updateInputState();

    aiManager->askAI(puzzleContext, conversationHistory);
}
