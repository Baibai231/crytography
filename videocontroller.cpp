#include "VideoController.h"

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QTimer>

VideoController::VideoController(QObject *parent)
    : QObject(parent) {}

void VideoController::playVideo(const QString &path, QWidget *parent, std::function<void()> onFinished) {
    Q_UNUSED(path); // 你现在是模拟视频，可以先不用

    // 创建窗口
    videoWidget = new QWidget(parent);
    videoWidget->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    videoWidget->setStyleSheet("background-color: black;");
    videoWidget->resize(800, 600);

    // UI
    QVBoxLayout *layout = new QVBoxLayout(videoWidget);

    QLabel *label = new QLabel(videoWidget);
    label->setText("\n\n石门上的尘土正在落下...\n你感受到一股古老的力量正在苏醒。");
    label->setStyleSheet("color: #ecf0f1; font-size: 24px; font-family: 'Microsoft YaHei';");
    label->setAlignment(Qt::AlignCenter);

    layout->addWidget(label);

    // 显示
    videoWidget->show();
    videoWidget->raise();
    videoWidget->activateWindow();

    // 3秒后结束
    QTimer::singleShot(3000, this, [this, onFinished]() {

        if (videoWidget) {
            videoWidget->close();
            videoWidget->deleteLater();
            videoWidget = nullptr;
        }

        if (onFinished) {
            onFinished();
        }
    });
}
