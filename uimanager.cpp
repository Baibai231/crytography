#include "UIManager.h"

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

namespace {

QString dialogStyleSheet(const QString &accentColor)
{
    return QString(R"(
        QDialog {
            background-color: rgba(23, 27, 34, 242);
            border: 1px solid rgba(212, 176, 106, 180);
            border-radius: 14px;
        }
        QLabel {
            color: #f3ede2;
            font-family: 'Microsoft YaHei';
        }
        QLabel[role="title"] {
            font-size: 24px;
            font-weight: 700;
            color: %1;
        }
        QLabel[role="body"] {
            font-size: 15px;
            line-height: 1.5em;
            color: #d7d0c2;
        }
        QPushButton {
            min-height: 40px;
            padding: 0 18px;
            border-radius: 8px;
            border: 1px solid rgba(244, 214, 152, 90);
            background-color: rgba(74, 56, 41, 210);
            color: #fff7ea;
            font-size: 15px;
            font-weight: 600;
        }
        QPushButton:hover {
            background-color: rgba(104, 79, 58, 225);
        }
        QPushButton:pressed {
            background-color: rgba(62, 46, 33, 235);
        }
        QPushButton[variant="primary"] {
            background-color: %1;
            color: #1d140b;
            border: none;
        }
        QPushButton[variant="primary"]:hover {
            background-color: #f3c56a;
        }
        QPushButton[variant="danger"] {
            background-color: #b5523f;
            color: white;
            border: none;
        }
        QPushButton[variant="danger"]:hover {
            background-color: #ca6551;
        }
    )").arg(accentColor);
}

void showStyledDialog(const QString &title,
                      const QString &message,
                      const QString &accentColor,
                      const QString &buttonText,
                      QWidget *parent)
{
    QDialog dialog(parent);
    dialog.setWindowTitle(title);
    dialog.setModal(true);
    dialog.setFixedSize(420, 220);
    dialog.setStyleSheet(dialogStyleSheet(accentColor));

    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    layout->setContentsMargins(28, 24, 28, 24);
    layout->setSpacing(16);

    QLabel *titleLabel = new QLabel(title, &dialog);
    titleLabel->setProperty("role", "title");

    QLabel *messageLabel = new QLabel(message, &dialog);
    messageLabel->setProperty("role", "body");
    messageLabel->setWordWrap(true);

    QPushButton *confirmButton = new QPushButton(buttonText, &dialog);
    confirmButton->setProperty("variant", "primary");
    confirmButton->setCursor(Qt::PointingHandCursor);

    layout->addWidget(titleLabel);
    layout->addWidget(messageLabel);
    layout->addStretch();
    layout->addWidget(confirmButton);

    QObject::connect(confirmButton, &QPushButton::clicked, &dialog, &QDialog::accept);

    dialog.exec();
}

}

UIManager::UIManager(QObject *parent)
    : QObject(parent) {}

void UIManager::showInfoDialog(const QString &title, const QString &message, QWidget *parent)
{
    showStyledDialog(title, message, "#d7a84e", "继续", parent);
}

void UIManager::showErrorDialog(const QString &title, const QString &message, QWidget *parent)
{
    showStyledDialog(title, message, "#c96b56", "重试", parent);
}

int UIManager::showDeathDialog(int level, QWidget *parent) {
    QDialog dialog(parent);
    dialog.setWindowTitle("战败");
    dialog.setModal(true);
    dialog.setFixedSize(520, 260);
    dialog.setStyleSheet(dialogStyleSheet("#d7a84e"));

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
    mainLayout->setContentsMargins(28, 24, 28, 24);
    mainLayout->setSpacing(16);

    QLabel *titleLabel = new QLabel(QString("你在第 %1 关战败了！").arg(level), &dialog);
    QLabel *infoLabel = new QLabel("英雄，要重新挑战这一关吗？", &dialog);
    titleLabel->setProperty("role", "title");
    infoLabel->setProperty("role", "body");

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(12);

    QPushButton *retryBtn = new QPushButton("回到本关起点", &dialog);
    QPushButton *menuBtn = new QPushButton("返回主菜单", &dialog);
    QPushButton *exitBtn = new QPushButton("退出游戏", &dialog);
    retryBtn->setProperty("variant", "primary");
    exitBtn->setProperty("variant", "danger");
    retryBtn->setCursor(Qt::PointingHandCursor);
    menuBtn->setCursor(Qt::PointingHandCursor);
    exitBtn->setCursor(Qt::PointingHandCursor);

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(infoLabel);
    mainLayout->addStretch();

    buttonLayout->addWidget(retryBtn);
    buttonLayout->addWidget(menuBtn);
    buttonLayout->addWidget(exitBtn);

    mainLayout->addLayout(buttonLayout);

    // 返回值设计（很关键）
    connect(retryBtn, &QPushButton::clicked, &dialog, [&dialog]() {
        dialog.done(1);
    });

    connect(menuBtn, &QPushButton::clicked, &dialog, [&dialog]() {
        dialog.done(2);
    });

    connect(exitBtn, &QPushButton::clicked, &dialog, [&dialog]() {
        dialog.done(3);
    });

    return dialog.exec();
}

void UIManager::showInfo(const QString &title, const QString &message, QWidget *parent) {
    showInfoDialog(title, message, parent);
}
