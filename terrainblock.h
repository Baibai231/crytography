#ifndef TERRAINBLOCK_H
#define TERRAINBLOCK_H

#include <QGraphicsRectItem>
#include <QString>

class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;

class TerrainBlock : public QGraphicsRectItem {
public:
    explicit TerrainBlock(int x, int y, int size)
        : QGraphicsRectItem(0, 0, size, size), blockSize(size)
    {
        setPos(x, y);
        setData(0, "Ground");
    }

    void setBlockType(const QString &type)
    {
        blockType = type;
        setData(0, type);
        update();
    }

protected:
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;

private:
    int blockSize = 0;
    QString blockType = "Ground";
};

#endif
