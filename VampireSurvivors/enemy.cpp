#include "enemy.h"

Enemy::Enemy(int x, int y, int HP, int attack, QObject *parent) : QObject(parent), my_x(x), my_y(y), my_HP(HP), my_attack(attack) {
    // 初始化敌人的属性
}

Enemy::~Enemy() {
    //析构函数
}

int Enemy::getX() const {
    return my_x;
}

int Enemy::getY() const {
    return my_y;
}

int Enemy::getHP() const {
    return my_HP;
}

void Enemy::setHP(int HP) {
    my_HP = HP;
}

int Enemy::getAttack() const {
    return my_attack;
}

void Enemy::move(int dx, int dy) {
    // 实现敌人的移动逻辑
    my_x += dx;
    my_y += dy;
}

void Enemy::attack(Hero *hero) {
    // 实现敌人的攻击逻辑
    hero->setHP(hero->getHP() - my_attack);
}
