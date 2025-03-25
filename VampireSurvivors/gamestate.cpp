#include "bin/gamestate.h"

GameState::GameState(QObject *parent) : QObject(parent) {
    my_hero = new Hero(this);
    //设置屏幕大小
    my_screenCenterX = 400;
    my_screenCenterY = 300;
}

GameState::~GameState() {
    delete my_hero;
}

void GameState::update() {
    //更新英雄的位置
    my_hero -> move(0, 0);//根据输入更新
    //计算地图偏移
    int offsetX = my_hero -> getX() - my_screenCenterX;
    int offsetY = my_hero -> getY() - my_screenCenterY;
    //更新显示位置
}

Hero* GameState::getHero() {
    return my_hero;
}
