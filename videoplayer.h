#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QWidget>

class VideoPlayer : public QWidget {
    Q_OBJECT

public:
    // 构造函数接收一个路径，虽然目前是占位，但保留接口以便后续升级
    explicit VideoPlayer(const QString &videoPath, QWidget *parent = nullptr);
    void play(); // 播放接口

signals:
    void finished(); // 播放完成信号

protected:
    // 即使是占位符，也可以处理一下关闭事件，确保资源释放
    void closeEvent(QCloseEvent *event) override;

private:
    QString m_path;
};

#endif
