#include "../../include/core/gamemap.h"
#include <QRandomGenerator>
#include <QPainter>

GameMap::GameMap(QObject *parent) : QObject(parent), width(50), height(50)
{
    generateMap();
}

GameMap::~GameMap()
{
}

void GameMap::generateMap()
{
    // 生成一个简单的50x50地图
    barriers.resize(height);
    for (int i = 0; i < height; i++) {
        barriers[i].resize(width);
        for (int j = 0; j < width; j++) {
            // 边界设置为墙
            if (i == 0 || j == 0 || i == height - 1 || j == width - 1) {
                barriers[i][j] = 1; // 墙
            }
            // 随机生成一些障碍
            else if (QRandomGenerator::global()->bounded(100) < 8) {
                barriers[i][j] = QRandomGenerator::global()->bounded(2) + 1; // 1=墙，2=岩石
            }
            else {
                barriers[i][j] = 0; // 空白
            }
        }
    }
    
    // 确保中心区域是空的（玩家初始位置）
    int centerX = width / 2;
    int centerY = height / 2;
    for (int i = centerY - 3; i <= centerY + 3; i++) {
        for (int j = centerX - 3; j <= centerX + 3; j++) {
            if (i >= 0 && i < height && j >= 0 && j < width) {
                barriers[i][j] = 0;
            }
        }
    }
}

bool GameMap::isBarrier(int x, int y) const
{
    if (x < 0 || y < 0 || x >= width || y >= height) {
        return true; // 边界外视为障碍
    }
    return barriers[y][x] > 0;
}

int GameMap::getWidth() const
{
    return width;
}

int GameMap::getHeight() const
{
    return height;
}

QVector<QVector<int>> GameMap::getBarries() const
{
    return barriers;
}

// 计算路径（简单的直线路径）
QVector<QPair<int, int>> GameMap::calculatePath(int startX, int startY, int endX, int endY) const
{
    QVector<QPair<int, int>> path;
    
    // 如果起点和终点相同，返回空路径
    if (startX == endX && startY == endY) {
        return path;
    }
    
    // 如果终点是障碍，不可到达
    if (isBarrier(endX, endY)) {
        return path;
    }
    
    // 简单的直线路径
    path.append(QPair<int, int>(startX, startY));
    path.append(QPair<int, int>(endX, endY));
    
    return path;
}
