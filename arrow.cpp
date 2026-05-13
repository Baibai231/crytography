#include "Arrow.h"
#include <QGraphicsScene>
#include <QPixmap>
#include <QTransform>
#include <QDebug>

Arrow::Arrow(qreal x, qreal y, qreal dir) : direction(dir) {

    // 加载图片
    QPixmap pix(":/images/arrow.png");

    if (pix.isNull()) {
        qDebug() << "箭图片加载失败！";
    }

    // 放大一点
    pix = pix.scaled(100, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // 根据方向翻转
    if (direction < 0) {
        pix = pix.transformed(QTransform().scale(-1, 1));
    }

    setPixmap(pix);

    // 设置位置
    setPos(x, y);

    // 标记类型
    setData(0, "Arrow");
}

void Arrow::advance(int phase) {
    if (!phase) return;

    setPos(x() + speed * direction, y());

    if (x() < -100 || x() > 900) {
        scene()->removeItem(this);
        delete this;
    }
}

QPainterPath Arrow::shape() const {
    QPainterPath path;
    path.addRect(boundingRect()); // 用矩形保证命中
    return path;
}
