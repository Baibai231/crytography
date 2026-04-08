#include "VideoController.h"

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QTimer>

VideoController::VideoController(QObject *parent)
    : QObject(parent) {}

void VideoController::playVideo(const QString &path, QWidget *parent, std::function<void()> onFinished) {
    Q_UNUSED(path);

    // ===== 1. 创建黑屏窗口 =====
    videoWidget = new QWidget(parent);
    videoWidget->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    videoWidget->setStyleSheet("background-color: black;");
    videoWidget->resize(800, 600);

    QVBoxLayout *layout = new QVBoxLayout(videoWidget);
    layout->setContentsMargins(40, 40, 40, 40);

    QLabel *label = new QLabel(videoWidget);
    label->setAlignment(Qt::AlignCenter);
    label->setWordWrap(true);

    // ===== 2. 风格（重点优化） =====
    label->setStyleSheet(R"(
        color: #d8c39a;              /* 暗金色，更符合解谜氛围 */
        font-size: 22px;
        font-family: "Consolas", "Courier New", "Microsoft YaHei";
        letter-spacing: 2px;
    )");

    layout->addWidget(label);

    videoWidget->show();
    videoWidget->raise();
    videoWidget->activateWindow();

    // ===== 3. 打字机文本 =====
    QString fullText =
        "石门上的尘土正在落下...\n"
        "你感受到一股古老的力量正在苏醒。";

    int interval = 100; // 每个字间隔(ms) —— 控制打字速度
    int *index = new int(0);

    QTimer *typeTimer = new QTimer(this);

    connect(typeTimer, &QTimer::timeout, this, [=]() mutable {
        if (*index < fullText.length()) {
            label->setText(label->text() + fullText[*index]);
            (*index)++;
        } else {
            typeTimer->stop();

            // ===== 4. 停留一小段时间再结束 =====
            QTimer::singleShot(1000, this, [this, onFinished]() {

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
    });

    typeTimer->start(interval);
}
