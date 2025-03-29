#ifndef BULLET_H
#define BULLET_H

#include <QObject>

class bullet : public QObject{
    Q_OBJECT
public:
    bullet(int x, int y, int speed, int damage, QObject *parent = nullptr);
    virtual ~bullet() override;

    //获取子弹的位置
    int getX() const;
    int getY() const;

    //获取子弹的速度
    int getSpeed() const;
    //获取子弹的伤害
    int getDamage() const;
    //子弹移动
    void move();

private:
    int my_x;
    int my_y;
    int my_speed;
    int my_damage;

};

#endif // BULLET_H
