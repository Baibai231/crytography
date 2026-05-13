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
    void startClicked(); // 点击开始游戏时发出的信号 (starts from level 1 or last saved)
    void levelSelected(int level); // 点击选择关卡信号

private slots:
    void showHowToPlay(); // 显示玩法说明
    void showReleaseNotes(); // 显示发行说明
    void showLevelSelect(); // 选择关卡槽函数

private:
    // 成员变量
    QVBoxLayout *mainLayout;
    QLabel *titleLabel;
    QPushButton *startBtn;
    QPushButton *howToPlayBtn;
    QPushButton *releaseBtn;
    QPushButton *selectLevelBtn; // 新增

    void applyStyle(); // 统一设置样式
};

#endif
