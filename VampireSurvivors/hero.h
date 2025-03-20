#ifndef HERO_H
#define HERO_H

#include<QObject>

class Hero : public QObject {
    Q_OBJECT
public:
    explicit Hero(QObject *parent = nullptr);
    ~Hero() override;

    int getHP() const;
    void setHP(int HP);
    void move(int dx, int dy);
    void attack();
    int getX() const;
    int getY() const;
private:
    int my_HP;
    int my_attack;
    int my_speed;
    int my_x;
    int my_y;
};

#endif // HERO_H
