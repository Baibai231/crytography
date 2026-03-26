#ifndef PASSWORDINPUTDIALOG_H
#define PASSWORDINPUTDIALOG_H

#include <QDialog>

class QLineEdit;

class PasswordInputDialog : public QDialog {
    Q_OBJECT

public:
    explicit PasswordInputDialog(const QString &title,
                                 const QString &hint,
                                 QWidget *parent = nullptr);

    QString getInput() const;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QLineEdit *inputEdit;
};

#endif
