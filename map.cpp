#include "map.h"

Map::Map(QGraphicsItem *parent) : QGraphicsScene(parent) {
    // 使用线条绘制地图
    QPainterPath path;
    path.moveTo(0, 0);
    path.lineTo(800, 0);
    path.lineTo(800, 600);
    path.lineTo(0, 600);
    path.lineTo(0, 0);
    addPath(path);
}

void Map::updateMap() {
    // 更新地图状态
    // 这里可以添加地图状态更新的代码，例如更新障碍物的位置、状态等
}
