#ifndef UIMANAGER_H
#define UIMANAGER_H

#include <QObject>

class QWidget;

class UIManager : public QObject {
    Q_OBJECT

public:
    explicit UIManager(QObject *parent = nullptr);

    // 死亡界面
    int showDeathDialog(int level, QWidget *parent);

    // 提示信息（成功/失败）
    void showInfo(const QString &title, const QString &message, QWidget *parent);
};

#endif
