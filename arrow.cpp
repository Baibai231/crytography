#include "Arrow.h"
#include <QGraphicsScene>
#include <QPixmap>
#include <QTransform>
#include <QDebug>

Arrow::Arrow(qreal x, qreal y, qreal dir) : direction(dir) {

    // ✅ 1. 加载图片（改成你的路径）
    QPixmap pix(":/images/arrow.png");   // ⭐ 推荐用资源路径
    // 如果你不是用资源系统：
    // QPixmap pix("arrow.png");

    if (pix.isNull()) {
        qDebug() << "箭图片加载失败！";
    }

    // ✅ 2. 放大一点（避免太小检测不到）
    pix = pix.scaled(100, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // ✅ 3. 根据方向翻转
    if (direction < 0) {
        pix = pix.transformed(QTransform().scale(-1, 1));
    }

    setPixmap(pix);

    // ✅ 4. 设置位置（稍微往中间对齐）
    setPos(x, y);

    // ✅ 5. 标记类型（非常重要）
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
    path.addRect(boundingRect()); // ✅ 用矩形保证命中
    return path;
}
