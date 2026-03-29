#include "TerrainBlock.h"

#include <QColor>
#include <QLinearGradient>
#include <QPainter>
#include <QPainterPath>
#include <QPen>

namespace {

QColor topColorForType(const QString &type)
{
    if (type == "Trap") {
        return QColor(205, 146, 61);
    }
    if (type == "Door") {
        return QColor(72, 134, 133);
    }
    return QColor(122, 106, 88);
}

QColor baseColorForType(const QString &type)
{
    if (type == "Trap") {
        return QColor(118, 72, 28);
    }
    if (type == "Door") {
        return QColor(35, 82, 82);
    }
    return QColor(82, 68, 56);
}

QColor lineColorForType(const QString &type)
{
    if (type == "Trap") {
        return QColor(255, 210, 120, 140);
    }
    if (type == "Door") {
        return QColor(140, 225, 216, 120);
    }
    return QColor(230, 212, 180, 80);
}

}

void TerrainBlock::paint(QPainter *painter,
                         const QStyleOptionGraphicsItem *option,
                         QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setRenderHint(QPainter::Antialiasing, true);

    QRectF blockRect = rect();
    QRectF outerRect = blockRect.adjusted(1.0, 1.0, -1.0, -1.0);

    QPainterPath outerPath;
    outerPath.addRoundedRect(outerRect, 4.0, 4.0);

    QLinearGradient fillGradient(outerRect.topLeft(), outerRect.bottomLeft());
    fillGradient.setColorAt(0.0, topColorForType(blockType));
    fillGradient.setColorAt(1.0, baseColorForType(blockType));
    painter->fillPath(outerPath, fillGradient);

    painter->setPen(QPen(QColor(255, 243, 220, 65), 1.0));
    painter->drawLine(QPointF(outerRect.left() + 4.0, outerRect.top() + 5.0),
                      QPointF(outerRect.right() - 4.0, outerRect.top() + 5.0));

    painter->setPen(QPen(QColor(0, 0, 0, 70), 1.0));
    painter->drawLine(QPointF(outerRect.left() + 4.0, outerRect.bottom() - 4.0),
                      QPointF(outerRect.right() - 4.0, outerRect.bottom() - 4.0));

    painter->setPen(QPen(lineColorForType(blockType), 1.0));
    const qreal midY = outerRect.top() + outerRect.height() * 0.52;
    painter->drawLine(QPointF(outerRect.left() + 3.0, midY),
                      QPointF(outerRect.right() - 3.0, midY));

    const qreal leftX = outerRect.left() + outerRect.width() * 0.34;
    const qreal rightX = outerRect.left() + outerRect.width() * 0.68;
    painter->drawLine(QPointF(leftX, outerRect.top() + 3.0),
                      QPointF(leftX, midY - 1.0));
    painter->drawLine(QPointF(rightX, midY + 1.0),
                      QPointF(rightX, outerRect.bottom() - 3.0));

    painter->setPen(QPen(QColor(18, 14, 10, 95), 1.0));
    painter->drawRoundedRect(outerRect, 4.0, 4.0);

    if (blockType == "Trap") {
        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor(244, 192, 78, 185));

        const qreal spikeWidth = blockSize / 5.0;
        for (int i = 0; i < 4; ++i) {
            QPainterPath spike;
            const qreal x = 4.0 + i * spikeWidth;
            spike.moveTo(x, outerRect.bottom() - 3.0);
            spike.lineTo(x + spikeWidth * 0.5, outerRect.top() + 6.0);
            spike.lineTo(x + spikeWidth, outerRect.bottom() - 3.0);
            spike.closeSubpath();
            painter->drawPath(spike);
        }
    }

    if (blockType == "Door") {
        QRectF glowRect = outerRect.adjusted(8.0, 6.0, -8.0, -6.0);
        QLinearGradient glow(glowRect.topLeft(), glowRect.bottomRight());
        glow.setColorAt(0.0, QColor(130, 232, 220, 125));
        glow.setColorAt(1.0, QColor(64, 165, 154, 185));
        painter->fillRect(glowRect, glow);

        painter->setPen(QPen(QColor(214, 255, 248, 170), 1.2));
        painter->drawRect(glowRect);
    }
}
