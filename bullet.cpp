#include "bullet.h"

Bullet::Bullet(QGraphicsItem *parent) : QGraphicsPixmapItem(parent) {
    setPixmap(QPixmap(":/images/bullet.png"));
    speed = 10;
}

void Bullet::move() {
    setPos(x(), y() - speed);
}
