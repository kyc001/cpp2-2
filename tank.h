#ifndef TANK_H
#define TANK_H

#include <QGraphicsPixmapItem>

class Tank : public QGraphicsPixmapItem
{
public:
    Tank(QGraphicsItem *parent = nullptr);
    void moveForward();
    void turnLeft();
    void turnRight();
    void fire();

private:
    int speed;
};

#endif // TANK_H
