#include "bin/bullet.h"

bullet::bullet(int x, int y, int speed, int damage, QObject *parent) : QObject(parent), my_x(x), my_y(y), my_speed(speed), my_damage(damage) {
    //初始化子弹属性
}

bullet::~bullet() {
    //析构函数
}

int bullet::getX() const {
    return my_x;
}

int bullet::getY() const {
    return my_y;
}

int bullet::getSpeed() const {
    return my_speed;
}

int bullet::getDamage() const {
    return my_damage;
}

void bullet::move() {
    //实现子弹移动逻辑
    my_x += my_speed;
}
