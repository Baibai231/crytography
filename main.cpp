#include <QApplication>
#include "GameWindow.h"
#include "StartScreen.h"
#include "GameProgress.h"


int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    // 实例化两个界面
    StartScreen start; // 开始界面
    GameWindow w;      // 游戏窗口

    // 从开始菜单进入游戏
    // 当 start 发出 startClicked 信号时，隐藏菜单，显示 w
    QObject::connect(&start, &StartScreen::startClicked, [&]() {
        start.hide();
        w.show();
        w.startNewGame(GameProgress::getMaxUnlockedLevel());
    });

    // 从开始菜单选择关卡
    QObject::connect(&start, &StartScreen::levelSelected, [&](int level) {
        start.hide();
        w.show();
        w.startNewGame(level);
    });

    // 从游戏窗口回到菜单
    w.setBackToMenuHandler([&]() {
        w.hide();
        start.show();
    });

    // 显示开始界面
    start.show();

    return a.exec();
}

