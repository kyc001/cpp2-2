#include "mapdrawer.h"
#include <QPainter>

MapDrawer::MapDrawer(QObject *parent) : QObject(parent) {
    // 加载墙的图片资源
    wall1Pixmap.load("://res/wall1.jpg");
    wall2Pixmap.load("://res/wall2.jpg");
}

MapDrawer::~MapDrawer() {}

void MapDrawer::drawMap(QPainter& painter, const std::vector<std::vector<int>>& map) {
    for (int i = 0; i < map.size(); ++i) {
        for (int j = 0; j < map[i].size(); ++j) {
            if (map[i][j] == 1) {
                // 绘制不可打碎的墙
                painter.drawPixmap(j * 25, i * 25, 25, 25, wall1Pixmap);
            } else if (map[i][j] == 2) {
                // 绘制可打碎的墙
                painter.drawPixmap(j * 25, i * 25, 25, 25, wall2Pixmap);
            }
        }
    }
}
