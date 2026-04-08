#include "spinwheeldialog.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QDial>
#include <QPushButton>
#include <QFont>
#include <QDebug>

SpinWheelDialog::SpinWheelDialog(const QString &cipher,
                                 const QString &plain,
                                 int correctOffset,
                                 QWidget *parent)
    : QDialog(parent),
    cipherText(cipher),
    plainText(plain),
    correctOffset(correctOffset)
{
    setWindowTitle("凯撒解密转盘");
    setFixedSize(420, 360);

    QVBoxLayout *layout = new QVBoxLayout(this);

    QLabel *title = new QLabel("旋转转盘破解凯撒密码", this);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("color:#e4b45f; font-size:18px;");

    cipherLabel = new QLabel(cipherText, this);
    cipherLabel->setAlignment(Qt::AlignCenter);
    cipherLabel->setStyleSheet("font-size:24px; color:#ffd58a;");

    dial = new QDial(this);
    dial->setRange(1, 25);
    dial->setValue(1);
    dial->setNotchesVisible(true);

    resultLabel = new QLabel("等待旋转...", this);
    resultLabel->setAlignment(Qt::AlignCenter);
    resultLabel->setStyleSheet("font-size:20px; color:#ffffff;");

    layout->addWidget(title);
    layout->addWidget(cipherLabel);
    layout->addWidget(dial);
    layout->addWidget(resultLabel);

    connect(dial, &QDial::valueChanged,
            this, &SpinWheelDialog::onOffsetChanged);

    updateUI(1);
}
QString SpinWheelDialog::decrypt(int offset)
{
    QString result;

    for (QChar c : cipherText)
    {
        if (!c.isLetter()) {
            result.append(c);
            continue;
        }

        int base = 'A';
        int pos = c.toLatin1() - base;
        int newPos = (pos - offset + 26) % 26;

        result.append(QChar(base + newPos));
    }

    return result;
}
void SpinWheelDialog::updateUI(int offset)
{
    QString result = decrypt(offset);

    resultLabel->setText(result);

    // ✔ 如果完全正确 → 亮
    if (result == plainText)
    {
        setStyleSheet("background-color:#2b2a1f; color:#ffffff;");
        resultLabel->setStyleSheet("color:#7CFC00; font-size:22px; font-weight:bold;");
    }
    else
    {
        setStyleSheet("background-color:#151515; color:#888;");
        resultLabel->setStyleSheet("color:#aaaaaa; font-size:20px;");
    }
}
void SpinWheelDialog::onOffsetChanged(int value)
{
    updateUI(value);
}
