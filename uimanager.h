#ifndef UIMANAGER_H
#define UIMANAGER_H

#include <QObject>
#include <QString>

class QWidget;

class UIManager : public QObject {
    Q_OBJECT

public:
    explicit UIManager(QObject *parent = nullptr);

    static void showInfoDialog(const QString &title, const QString &message, QWidget *parent);
    static void showErrorDialog(const QString &title, const QString &message, QWidget *parent);

    // 死亡界面
    int showDeathDialog(int level, QWidget *parent);

    // 提示信息（成功/失败）
    void showInfo(const QString &title, const QString &message, QWidget *parent);
};

#endif
