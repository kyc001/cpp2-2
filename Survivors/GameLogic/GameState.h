#ifndef SURVIVORS_GAMESTATE_H
#define SURVIVORS_GAMESTATE_H
#include "Hero.h"
#include "Enemy.h"
#include "PaintInfo.h"
#include "EnemyController.h"
#include "GameMap.h"
#include <QtWidgets/QWidget>
#include <vector>
#include <QtGui/QKeyEvent>
#include <utility>
#include "ExpBall.h"
#include "FloatingOrb.h"

class EnemyController;

class GameState {
    QWidget * parent;
    GameMap * _map;
    Hero * player;
    EnemyController * enemy_control;
    std::vector<std::vector<Enemy *>> enemies;
    std::vector<ExpBall *> exp_balls;
    
    // 环绕玩家的悬浮球
    std::vector<FloatingOrb *> floating_orbs;

    int exp_cnt;
    int exp_max;
    QWidget *widget_parent; // 父窗口指针，用于 UI 更新
public:

    GameState();
    explicit GameState(QWidget * parent);
    ~GameState();

    void tick();                        //无条件tick
    void keyPressTick(QKeyEvent * event);       //键盘事件tick
    void keyReleaseTick(QKeyEvent * event);

    std::vector<PaintInfo> paint();

    void initMap(unsigned map_style);
    void initHero(unsigned hero_style);
    void initEnemy(int stage);

    [[nodiscard]] int getEnemyDeathCnt() { return enemy_control->getEnemyDeathCnt(); };
    [[nodiscard]] int getPlayerLevel() const { return player->getLevel(); }

    bool isGameStop() { return player->isGameStop(); }

    void addExpBall(double x, double y, int value);
    void EBEnableNullSpace(double x, double y, int value, ExpBall * &space);
    void EBEnableUsedSpace(double x, double y, int value, ExpBall * &space);

    int countExp(std::pair<double, double> player_pos);

    void upgrade(int type);
    double getHPPercent() { return player->get_hp_percent(); }

    bool isWaiting() { return player->isWaiting(); };

    // 获取英雄对象指针
    [[nodiscard]] Hero* getHero() const { return player; }

    // 检查悬浮球与敌人的碰撞
    void checkOrbEnemyCollisions();

    friend class EnemyController;
    friend class Bullet;
    friend class Hero;
    friend class HeroStaticAOEBullet;
    friend class HeroDynamicWeapon;
    friend class FloatingOrb;
    friend class GameMain;

    void gameHalt();


private:
    void judgeDamageEnemies();
};


#endif //VSCPROJECT_GAMESTATE_H

