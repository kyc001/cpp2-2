#ifndef HERO_H
#define HERO_H

#include<QObject>
#include<QVector>
#include<QLabel>
#include<QProgressBar>

class Hero : public QObject {
    Q_OBJECT

private:
    int HP_MAX;
    int my_HP;
    int EXP_MAX;
    int my_EXP;
    int my_attack;
    int my_speed;
    bool is_alive;
    QProgressBar* HP_bar;
    QProgressBar* EXP_bar;
    QLabel* HP_label;
    QLabel* EXP_label;
    pair<int, int> abspos;
    pair<double, double> realpos

    //武器信息
    int weapon_type;
    Weapon *kweapon;

    //按键信息
    list<int> key_pressed;
    gamestate *state;

public:
    Hero()
    explicit Hero(int hero_style, QWidget *wdparent, GameMap * maparent);
    ~Hero() override;

    int getHP() const;
    void setHP(int HP);
    void move(int dx, int dy);
    void attack();
    int getX() const;
    int getY() const;
    //碰撞箱
    QRect absolute_rect;
    QRect real_rect;
};

#endif // HERO_H
