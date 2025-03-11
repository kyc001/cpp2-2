#ifndef MAP_H
#define MAP_H

#include <QGraphicsItem>

class Map : public QGraphicsItem {
public:
    Map();
    // 绘制地图的方法
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    // 获取地图的边界矩形
    QRectF boundingRect() const override;
};

#endif // MAP_H