#include "Obstacle.h"

Obstacle::Obstacle(int x, int y, int width, int height, QObject *parent) : QObject(parent), my_x(x), my_y(y), my_width(width), my_height(height) {
    // 初始化障碍物的属性
}

Obstacle::~Obstacle() {
    // 析构函数
}

int Obstacle::getX() const {
    return my_x;
}

int Obstacle::getY() const {
    return my_y;
}

int Obstacle::getWidth() const {
    return my_width;
}

int Obstacle::getHeight() const {
    return my_height;
}
