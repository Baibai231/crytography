#include <QApplication>
#include "GameWindow.h"
#include "StartScreen.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    // 1. 实例化两个界面
    StartScreen start; // 开始界面
    GameWindow w;      // 游戏窗口

    // 2. 关联：从开始菜单进入游戏
    // 当 start 发出 startClicked 信号时，隐藏菜单，显示 w
    QObject::connect(&start, &StartScreen::startClicked, [&]() {
        start.hide();
        w.show();
        w.startNewGame();
    });

    // 3. 关联：从游戏窗口回到菜单
    w.setBackToMenuHandler([&]() {
        w.hide();
        start.show();
    });

    // 4. 程序启动：首先显示开始界面
    start.show();

    return a.exec();
}

