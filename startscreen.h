#ifndef STARTSCREEN_H
#define STARTSCREEN_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

class StartScreen : public QWidget {
    Q_OBJECT
public:
    explicit StartScreen(QWidget *parent = nullptr);

signals:
    void startClicked(); // 点击开始游戏时发出的信号

private slots:
    void showHowToPlay(); // 显示玩法说明
    void showReleaseNotes(); // 显示发行说明

private:
    // 成员变量，方便后期动态调整
    QVBoxLayout *mainLayout;
    QLabel *titleLabel;
    QPushButton *startBtn;
    QPushButton *howToPlayBtn;
    QPushButton *releaseBtn;

    void applyStyle(); // 统一设置样式的地方
};

#endif
