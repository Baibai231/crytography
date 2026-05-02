#include "PasswordInputDialog.h"

#include <QPainter>
#include <QPainterPath>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QHBoxLayout>

PasswordInputDialog::PasswordInputDialog(const QString &title,
                                         const QString &hint,
                                         QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(title);
    setFixedSize(430, 260);
    setAttribute(Qt::WA_TranslucentBackground);
    setStyleSheet(R"(
        QLabel {
            color: #f6eedf;
            font-family: 'Microsoft YaHei';
        }
        QLabel[role="title"] {
            font-size: 24px;
            font-weight: 700;
            color: #e6b85c;
        }
        QLabel[role="hint"] {
            font-size: 15px;
            color: #d8cfbe;
        }
        QLineEdit {
            background-color: rgba(252, 247, 238, 230);
            color: #2d241b;
            padding: 10px 12px;
            border: 2px solid rgba(230, 184, 92, 150);
            border-radius: 8px;
            font-size: 15px;
        }
        QLineEdit:focus {
            border-color: #f0c875;
        }
        QPushButton {
            min-height: 38px;
            padding: 0 18px;
            border-radius: 8px;
            font-size: 15px;
            font-weight: 600;
        }
        QPushButton[variant="primary"] {
            background-color: #d7a84e;
            color: #22170c;
            border: none;
        }
        QPushButton[variant="primary"]:hover {
            background-color: #ebbe67;
        }
        QPushButton[variant="secondary"] {
            background-color: rgba(70, 53, 39, 215);
            color: #f5ead8;
            border: 1px solid rgba(236, 210, 161, 90);
        }
        QPushButton[variant="secondary"]:hover {
            background-color: rgba(94, 72, 52, 225);
        }
    )");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(28, 26, 28, 24);
    layout->setSpacing(16);

    QLabel *titleLabel = new QLabel(title, this);
    titleLabel->setProperty("role", "title");
    titleLabel->setAlignment(Qt::AlignCenter);

    QLabel *hintLabel = new QLabel(hint, this);
    hintLabel->setProperty("role", "hint");
    hintLabel->setAlignment(Qt::AlignCenter);
    hintLabel->setWordWrap(true);

    inputEdit = new QLineEdit(this);
    inputEdit->setPlaceholderText("请输入解密结果...");

    QPushButton *okBtn = new QPushButton("确认", this);
    QPushButton *cancelBtn = new QPushButton("取消", this);
    okBtn->setProperty("variant", "primary");
    cancelBtn->setProperty("variant", "secondary");
    okBtn->setCursor(Qt::PointingHandCursor);
    cancelBtn->setCursor(Qt::PointingHandCursor);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(10);
    buttonLayout->addWidget(cancelBtn);
    buttonLayout->addWidget(okBtn);

    layout->addWidget(titleLabel);
    layout->addWidget(hintLabel);
    layout->addWidget(inputEdit);
    layout->addStretch();
    layout->addLayout(buttonLayout);

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
    painter.setRenderHint(QPainter::Antialiasing);

    QPixmap bg(":/images/stone.png"); // ⚠️ 确保资源路径正确
    QRect contentRect = rect().adjusted(8, 8, -8, -8);
    QPainterPath path;
    path.addRoundedRect(contentRect, 18, 18);

    painter.fillPath(path, QColor(18, 19, 24, 210));

    if (!bg.isNull()) {
        painter.save();
        painter.setClipPath(path);
        painter.setOpacity(0.24);
        painter.drawPixmap(contentRect, bg.scaled(contentRect.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        painter.restore();
    }

    painter.setPen(QPen(QColor(228, 184, 95, 170), 1.4));
    painter.drawPath(path);
}
