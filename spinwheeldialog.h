#ifndef SPINWHEELDIALOG_H
#define SPINWHEELDIALOG_H

#include <QDialog>

class QLabel;
class QDial;

class SpinWheelDialog : public QDialog
{
    Q_OBJECT

public:
    SpinWheelDialog(const QString &cipher,
                    const QString &plain,
                    int correctOffset,
                    QWidget *parent = nullptr);

private slots:
    void onOffsetChanged(int value);

private:
    QString cipherText;
    QString plainText;
    int correctOffset;

    QDial *dial;
    QLabel *cipherLabel;
    QLabel *resultLabel;

    QString decrypt(int offset);
    void updateUI(int offset);
};

#endif
