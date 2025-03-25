#ifndef ENEMY_H
#define ENEMY_H

#include "hero.h"
#include <QObject>

class Enemy : public QObject{
    Q_OBJECT
public:
    Enemy(int x, int y, int HP, int attack, QObject *parent = nullptr);
    ~Enemy() override;

    //获取敌人的位置
    int getX() const;
    int getY() const;

    //获取生命值
    int getHP() const;
    //设置生命值
    void setHP(int HP);
    //获取伤害
    int getAttack() const;
    //移动
    void move(int dx, int dy);
    //攻击英雄
    void attack(Hero *hero);

private:
    int my_x;
    int my_y;
    int my_HP;
    int my_attack;
};

#endif // ENEMY_H
