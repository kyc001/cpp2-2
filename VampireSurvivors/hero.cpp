#include "bin/hero.h"
#include "bin/gamestate"
#include <cmath>
#include <iostream>

hero::hero() {
    // 初始化英雄属性
    _image.load(HERO1_path);
    HP_MAX = 100;
    hp = HP_MAX;
    EXP_MAX = HERO_INIT_EXP;
    exp = 0;
    level = 1;
    speed = 10;
    widget_parent = nullptr;
    HP_bar = new QProgressBar();
    EXP_bar = new QProgressBar();
    QSize pix_size(50, 50);
    _image.scaled(pix_size);
    
    absoulute_pos.first = GAME_WIDTH * 0.5 - _image.width() * 0.5;
    absoulute_pos.second = GAME_HEIGHT * 0.5 - _image.height() * 0.5;
    
    absolute_rect.setWidth(_image.width());
    absolute_rect.setHeight(_image.height());
    absolute_rect.moveTo(absoulute_pos.first, absoulute_pos.second);

    map_parent = nullptr;
    real_pos.first = 0;
    real_pos.second = 0;
    real_rect.setWidth(_image.width());
    real_rect.setHeight(_image.height());
    real_rect.moveTo((int)real_pos.first, (int)real_pos.second);

    map_parent = nullptr;
    real_pos.first = 0;
    real_pos.second = 0;

    real_rect.setWidth(_image.width());
    real_rect.setHeight(_image.height());
    real_rect.moveTo((int)real_pos.first, (int)real_pos.second);

    alive = true;

    _weapon = nullptr;
    weapon_type = 1;

    _game = nullptr;
}

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
