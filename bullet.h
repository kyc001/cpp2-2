#ifndef BULLET_H
#define BULLET_H

#include <QGraphicsPixmapItem>

class Bullet : public QGraphicsPixmapItem {
public:
    Bullet(QGraphicsItem *parent = nullptr);
    void move();
};

#endif // BULLET_H