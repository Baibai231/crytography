#ifndef TERRAINBLOCK_H
#define TERRAINBLOCK_H

#include <QGraphicsRectItem>
#include <QBrush>
#include <QPen> // 必须包含此头文件以处理 setPen(Qt::NoPen)

class TerrainBlock : public QGraphicsRectItem {
public:
    TerrainBlock(int x, int y, int size) {
        setRect(0, 0, size, size);
        setPos(x, y);
        setBrush(QBrush(Qt::darkGray));
        // 修正：Qt::NoPen 需要 QPen 的完整定义支持
        setPen(Qt::NoPen);
        setData(0, "Ground");
    }
};

#endif
