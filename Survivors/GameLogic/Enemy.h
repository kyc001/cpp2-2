#ifndef ENEMY_H
#define ENEMY_H

#include <QProgressBar>
#include <QImage>
#include <QWidget>
#include <QRect>
#include <utility>
#include <vector>

#include "config.h"
#include "GameMap.h"

class Hero;
class EnemyController;
class Weapon;
class EnemyDynamicWeapon;

class Enemy {
public:
    Enemy(int enemy_style, QWidget *w_parent, EnemyController *controller, GameMap *m_parent, Hero *target, double real_X, double real_Y);
    
    virtual void tick() = 0;
    void damage(int d);
    void setRealPosition(double x, double y);
    virtual std::vector<PaintInfo> paint();
    void enable();
    void disable();
    void die();
    [[nodiscard]] bool isEnabled() const { return enabled; }
    
    int enemy_type;
    int HP_MAX;
    int hp;
    
    std::pair<double, double> real_pos;
    std::pair<int, int> absolute_pos;
    
    QRect real_rect;
    QRect absolute_rect;
    
    QImage _image;
    
    double speed;
    bool enabled;
    bool alive;
    
    QWidget *widget_parent;
    GameMap *map_parent;
    Hero *target;
    EnemyController *controller;
    QProgressBar *hp_bar;
    
protected:
    void healthChange();
    void setHpBarPosition();
};

class NoWeaponEnemy : public Enemy {
public:
    NoWeaponEnemy(int enemy_style, QWidget *w_parent, EnemyController *controller, GameMap *m_parent, Hero *target, double real_X, double real_Y);
    virtual void tick() override;
    bool judgeDamage();
    std::pair<double, double> getDirectionVector();
    
    int power;
    int CD;
    int cdn;
};

class NoWeaponEnemyGround : public NoWeaponEnemy {
public:
    NoWeaponEnemyGround(int enemy_style, QWidget *w_parent, EnemyController *controller, GameMap *m_parent, Hero *target, double real_X, double real_Y);
    virtual void tick() override;
    
protected:
    bool attemptMove(double x_bias, double y_bias);
};

class WeaponEnemy : public NoWeaponEnemy {
public:
    WeaponEnemy(int enemy_style, QWidget *w_parent, EnemyController *controller, GameMap *m_parent, Hero *target, double real_X, double real_Y);
    virtual void tick() override;
    virtual std::vector<PaintInfo> paint() override;
    
protected:
    EnemyDynamicWeapon *_weapon;
    std::pair<double, double> x_move_range;
    bool direction;
};

#endif
