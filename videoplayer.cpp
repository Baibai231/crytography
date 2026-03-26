#include "VideoPlayer.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QTimer>
#include <QApplication>

VideoPlayer::VideoPlayer(const QString &videoPath, QWidget *parent)
    : QWidget(parent), m_path(videoPath)
{
    // 1. UI 表现：全屏黑底
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setStyleSheet("background-color: black;");
    resize(800, 600);

    // 2. 占位文字
    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel *label = new QLabel(this);
    label->setText("【 过场动画 】\n\n石门上的尘土正在落下...\n你感受到一股古老的力量正在苏醒。\n\n(模拟视频播放中...)");
    label->setStyleSheet("color: #ecf0f1; font-size: 24px; font-family: 'Microsoft YaHei';");
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);

    // 3. 逻辑模拟：3秒后发送结束信号
    // 使用 QTimer::singleShot 是一种非常优雅的模拟异步操作的方法
    QTimer::singleShot(3000, this, [this]() {
        emit finished();
    });
}

void VideoPlayer::play() {
    this->show();
    this->raise();      // 确保窗口在最顶层
    this->activateWindow();
}

void VideoPlayer::closeEvent(QCloseEvent *event) {
    // 可以在这里停止任何正在运行的模拟逻辑
    QWidget::closeEvent(event);
}
