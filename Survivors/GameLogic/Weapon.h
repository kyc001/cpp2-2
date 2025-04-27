#ifndef SURVIVORS_WEAPON_H
#define SURVIVORS_WEAPON_H
// #include "Bullet.h"  // 移除，使用前向声明
#include <vector>
#include "PaintInfo.h"
#include <exception>
#include "GameMap.h"  // 添加包含GameMap头文件
#include "Bullet.h"   // 直接包含Bullet.h来解决不完整类型问题

class Hero;
class Enemy;
// class Bullet;  // 不需要前向声明，因为已经包含了头文件
class GameState;

class Weapon {
protected:
    GameMap * map_parent = nullptr;
    unsigned bullet_style;
    Hero * user;
    int bullet_tot;
    std::vector<Bullet *> bullet_buffer;
    int damage;
public:
    Weapon(unsigned bullet_style, int damage, Hero * user, GameMap * map_p);
    virtual void tick() = 0;
    bool judgeDamage(Enemy *);
    virtual void upgrade();
    virtual int getDamage() { return damage; }
    virtual int getCD() { return 0; }
    std::vector<PaintInfo> paint();
    [[nodiscard]] Hero * getUser() const { return user; }
};

class HeroStaticAOEWeapon : public Weapon{
    bool bullet_enabled = false;
    int range;
public:
    HeroStaticAOEWeapon(GameMap * map_p, Hero * user, int range, unsigned bullet_style, int damage);

    int getCD() override { return bullet_buffer[0]->getCD(); }
    void tick() override;
    void upgrade() override;
};

class HeroDynamicWeapon : public Weapon{
    int curr_bullet_cnt;
    int CD;
    int cdn;
    GameState * _game;

public:
    HeroDynamicWeapon(GameMap * map_p, Hero * user, int CD, unsigned bullet_style, int damage);

    void tick() override;

    int getCD() override { return CD; }
    void upgrade() override;

private:
    class TargetNotFoundError : public std::exception{
        int type;
    public:
        explicit TargetNotFoundError(int t) { type = t; }
    };

    Enemy * findTarget();
    void addBullet();
    void enableNullSpace(Bullet * &space);
    void enableUsedSpace(Bullet * &space);
};

class EnemyDynamicWeapon : public Weapon{
    int curr_bullet_cnt;
    int CD;
    int cdn;
    Enemy * _user;
    Hero * target;
public:
    EnemyDynamicWeapon(GameMap * map_p, Enemy * user, Hero * target, int CD, int damage);

    void tick() override;

    void upgrade() override {};
private:
    void addBullet();
    void enableNullSpace(Bullet * &space);
    void enableUsedSpace(Bullet * &space);
};


#endif //VSCPROJECT_WEAPON_H
