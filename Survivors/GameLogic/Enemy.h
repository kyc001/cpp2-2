#ifndef ENEMY_H
#define ENEMY_H

#include <QtGui/QImage>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QWidget>
#include <utility>
#include <vector>
#include "GameMap.h"
#include "PaintInfo.h"
// #include "EnemyDynamicWeapon.h" // 移除重复和不当位置的包含
// #include "EnemyDynamicWeapon.h"

class Hero;
class EnemyController;
class EnemyDynamicWeapon; // 使用前向声明

class Enemy {
public:
    Enemy() = delete;
    Enemy(int enemy_style, QWidget *w_parent, EnemyController *controller, GameMap *m_parent, Hero *target, double real_X, double real_Y);
    virtual ~Enemy() { 
        if (hp_bar) { 
            hp_bar->hide(); 
            hp_bar->deleteLater();
            hp_bar = nullptr; 
        } 
    }
    virtual void tick() = 0;
    virtual std::vector<PaintInfo> paint();
    void setRealPosition(double x, double y);
    void damage(int d);
    void die();
    void enable();
    void disable();
    void healthChange();
    void setHpBarPosition();
    
    // 新增的方法
    void createHpBar();
    void loadEnemyImage(int enemy_style, const QSize& size);

    bool isEnabled() { return enabled; }
    bool isAlive() { return alive; }
    int getType() { return enemy_type; }

    int HP_MAX;
    int hp;
    double speed;
    QRect real_rect;
    std::pair<double, double> real_pos;
    std::pair<int, int> absolute_pos;
    QRect absolute_rect;
    GameMap *map_parent;
    QWidget *widget_parent;
    QProgressBar *hp_bar;
    Hero *target;
    bool enabled;
    bool alive;
    EnemyController *controller;
    int enemy_type;
    QImage _image;
};

class NoWeaponEnemy : public Enemy {
public:
    NoWeaponEnemy() = delete;
    NoWeaponEnemy(int enemy_style, QWidget *w_parent, EnemyController *controller, GameMap *m_parent, Hero *target,
                  double real_X, double real_Y);
    virtual void tick() override;
    virtual bool judgeDamage();
    std::pair<double, double> getDirectionVector();

    int power = 0;
    int CD = 0;
    int cdn = 0;
};

class NoWeaponEnemyGround : public NoWeaponEnemy {
public:
    NoWeaponEnemyGround() = delete;
    NoWeaponEnemyGround(int enemy_style, QWidget *w_parent, EnemyController *controller, GameMap *m_parent,
                         Hero *target, double real_X, double real_Y);
    void tick() override;

    bool init_interact = true;

protected:
    bool attemptMove(double x_bias, double y_bias);
};

class WeaponEnemy : public NoWeaponEnemy {
public:
    WeaponEnemy() = delete;
    WeaponEnemy(int enemy_style, QWidget *w_parent, EnemyController *controller, GameMap *m_parent, Hero *target,
                double real_X, double real_Y);
    virtual ~WeaponEnemy(); // 实现移到cpp文件
    void tick() override;
    std::vector<PaintInfo> paint() override;

    // #include "EnemyDynamicWeapon.h" // 移除这里的包含
    EnemyDynamicWeapon *_weapon; // 使用前向声明的类型指针
    std::pair<double, double> x_move_range;
    bool direction;
};

#endif
