#ifndef VIDEOCONTROLLER_H
#define VIDEOCONTROLLER_H

#include <QObject>
#include <functional>

class QWidget;

class VideoController : public QObject {
    Q_OBJECT

public:
    explicit VideoController(QObject *parent = nullptr);

    void playVideo(const QString &path, QWidget *parent, std::function<void()> onFinished);

private:
    QWidget *videoWidget = nullptr; // 内部管理窗口
};

#endif
