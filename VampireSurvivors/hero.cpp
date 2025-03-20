#include "hero.h"

Hero::Hero(QObject *parent) : QObject(parent), my_HP(100), my_attack(10), my_speed(5), my_x(0), my_y(0) {
    //初始化英雄属性
}

Hero::~Hero() {
    //析构函数
}

int Hero::getHP() const {
    return my_HP;
}

void Hero::setHP(int HP) {
    my_HP = HP;
}

void Hero::move(int dx, int dy) {
    // 实现英雄的移动逻辑
}

int Hero::getX() const {
    return my_x;
}

int Hero::getY() const {
    return my_y;
}

void Hero::attack() {
    // 实现攻击逻辑
}
