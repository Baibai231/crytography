#ifndef ARROW_H
#define ARROW_H

#include <QGraphicsPixmapItem>
#include <QObject>

class Arrow : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
public:
    // x, y 为发射起点，dir 为方向（1向右，-1向左）
    Arrow(qreal x, qreal y, qreal dir);
    void advance(int phase) override;

private:
    qreal speed = 7.0;
    qreal direction;
};

#endif
