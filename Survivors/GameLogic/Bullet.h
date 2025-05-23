#ifndef SURVIVORS_BULLET_H
#define SURVIVORS_BULLET_H
// #include "GameMap.h"  // 使用前向声明
#include <utility>
#include <QtGui/QPixmap>
#include <exception>
#include "PaintInfo.h"
#include "config.h" // 添加config.h的包含

class Hero;
class Enemy;
class GameMap; // 前向声明

class Bullet
{

    bool enabled = false;
    unsigned bullet_style;

public:
    Bullet(unsigned bullet_style, int damage, GameMap *map_p);
    virtual void tick() = 0;
    virtual void enable() { enabled = true; }
    virtual void disable() { enabled = false; }
    virtual bool judge_damage(Enemy *e) = 0;
    virtual bool judge_damage(Hero *h) = 0;
    virtual void enable(Enemy *e) { enabled = true; }
    virtual void upgrade() {};
    virtual int getCD() { return 0; }

    virtual void setRealPosition(double x, double y);

    void setDamage(int d) { damage = d; }

    PaintInfo paint();

    [[nodiscard]] double getRealX() const { return real_pos.first; }
    [[nodiscard]] double getRealY() const { return real_pos.second; }
    [[nodiscard]] int getAbsoluteX() const { return absolute_pos.first; }
    [[nodiscard]] int getAbsoluteY() const { return absolute_pos.second; }
    [[nodiscard]] bool isEnabled() const { return enabled; }

protected:
    std::pair<double, double> real_pos;
    GameMap *map_parent = nullptr;
    std::pair<int, int> absolute_pos;
    int damage;
    QPixmap _image;
};

class HeroStaticAOEBullet : public Bullet
{
    int range;
    int CD;
    int cds;
    std::pair<double, double> center_real_pos;
    Hero *user;

public:
    HeroStaticAOEBullet(GameMap *map_parent, Hero *user, int range, unsigned bullet_style, int damage);
    void tick() override;
    bool judge_damage(Enemy *) override;
    bool judge_damage(Hero *) override { return false; }
    int getCD() override { return CD; }

    void upgrade() override
    {
        CD = (int)((double)CD / (WEAPON_INC_RATE + 0.2));
        if (CD <= 15)
        {
            CD = 15;
        }
        range = (int)(range * 1.2);
    }

private:
    void update_pos();
};

class HeroDynamicBullet : public Bullet
{
    QRect real_rect;
    Enemy *target;
    int speed;
    Hero *user;

public:
    HeroDynamicBullet(GameMap *map_parent, Hero *user, Enemy *target, unsigned bullet_style, int damage);
    void tick() override;
    void setRealPosition(double x, double y) override;
    void enable(Enemy *e) override;
    bool judge_damage(Enemy *) override;
    bool judge_damage(Hero *) override { return false; }

private:
    std::pair<double, double> getDirectionVector();
};

class TargetLossError : public std::exception
{
    int type;

public:
    explicit TargetLossError(int t) { type = t; };
};

class TargetHit : public std::exception
{
    int type;

public:
    explicit TargetHit(int t) { type = t; };
};

class EnemyDynamicBullet : public Bullet
{
    QRect real_rect;
    Hero *target;
    std::pair<double, double> direction_vector;
    int speed;
    Enemy *user;

public:
    EnemyDynamicBullet(GameMap *map_parent, Hero *target, Enemy *user, int damage);
    void tick() override;
    void setRealPosition(double x, double y) override;
    void enable() override;
    bool judge_damage(Enemy *) override { return false; };
    bool judge_damage(Hero *) override;
};

class OutOfRange : public std::exception
{
    int type;

public:
    explicit OutOfRange(int t) { type = t; };
};

#endif
