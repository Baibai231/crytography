#include "Arrow.h"
#include <QGraphicsScene>

#include <QPainter>
#include <QPen>

Arrow::Arrow(qreal x, qreal y, qreal dir) : direction(dir) {
    // 绘制一支简单的箭（如果你有图片可以用 QPixmap）
    QPixmap pix(30, 10);
    pix.fill(Qt::transparent);
    QPainter painter(&pix);
    painter.setPen(QPen(Qt::black, 2));
    painter.drawLine(0, 5, 30, 5); // 箭身
    painter.drawLine(25, 0, 30, 5); // 箭头
    painter.drawLine(25, 10, 30, 5);

    // 如果向左射，翻转图片
    if (direction < 0) {
        setPixmap(pix.transformed(QTransform().scale(-1, 1)));
    } else {
        setPixmap(pix);
    }

    setPos(x, y);
    setData(0, "Arrow"); // 标记为箭
}

void Arrow::advance(int phase) {
    if (!phase) return;

    setPos(x() + speed * direction, y());

    // 只有速度不为0时才移动
    if (speed != 0) {
        setPos(x() + speed, y());
    }

    // 走出屏幕外自动销毁，释放内存
    if (x() < -100 || x() > 900) {
        scene()->removeItem(this);
        delete this;
    }
}
