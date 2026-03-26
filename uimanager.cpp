#include "UIManager.h"

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>

UIManager::UIManager(QObject *parent)
    : QObject(parent) {}

int UIManager::showDeathDialog(int level, QWidget *parent) {
    QDialog dialog(parent);
    dialog.setWindowTitle("战败");
    dialog.setModal(true);

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);

    QLabel *titleLabel = new QLabel(QString("你在第 %1 关战败了！").arg(level), &dialog);
    QLabel *infoLabel = new QLabel("英雄，要重新挑战这一关吗？", &dialog);

    QHBoxLayout *buttonLayout = new QHBoxLayout();

    QPushButton *retryBtn = new QPushButton("回到本关起点", &dialog);
    QPushButton *menuBtn = new QPushButton("返回主菜单", &dialog);
    QPushButton *exitBtn = new QPushButton("退出游戏", &dialog);

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(infoLabel);

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
    QMessageBox::information(parent, title, message);
}
