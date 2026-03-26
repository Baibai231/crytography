#include "PasswordInputDialog.h"

#include <QPainter>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>

PasswordInputDialog::PasswordInputDialog(const QString &title,
                                         const QString &hint,
                                         QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(title);
    resize(400, 250);

    QVBoxLayout *layout = new QVBoxLayout(this);

    QLabel *hintLabel = new QLabel(hint, this);
    hintLabel->setStyleSheet("color: white; font-size: 16px;");
    hintLabel->setAlignment(Qt::AlignCenter);

    inputEdit = new QLineEdit(this);
    inputEdit->setPlaceholderText("请输入解密结果...");
    inputEdit->setStyleSheet("background-color: white; padding: 5px;");

    QPushButton *okBtn = new QPushButton("确认", this);
    QPushButton *cancelBtn = new QPushButton("取消", this);

    layout->addWidget(hintLabel);
    layout->addWidget(inputEdit);
    layout->addWidget(okBtn);
    layout->addWidget(cancelBtn);

    connect(okBtn, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
}

QString PasswordInputDialog::getInput() const {
    return inputEdit->text();
}

void PasswordInputDialog::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    QPixmap bg(":/images/stone.png"); // ⚠️ 确保资源路径正确

    if (!bg.isNull()) {
        painter.drawPixmap(rect(), bg);
    } else {
        painter.fillRect(rect(), QColor(60, 60, 60));
    }
}
