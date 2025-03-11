#include "tank.h"

Tank::Tank(QGraphicsItem *parent) : QGraphicsPixmapItem(parent)
{
    // 使用图片绘制坦克
setPixmap(QPixmap(":/images/tank.png"));
    speed = 5;
}

void Tank::moveForward() {
    QPointF newPos = QPointF(x(), y() - speed);
    if (!checkCollision(newPos)) {
        setPos(newPos);
    }
}

void Tank::moveBackward() {
    QPointF newPos = QPointF(x(), y() + speed);
    if (!checkCollision(newPos)) {
        setPos(newPos);
    }
}

bool Tank::checkCollision(const QPointF &newPos) {
    QRectF newRect = QRectF(newPos.x(), newPos.y(), 50, 30);
    QList<QGraphicsItem*> collidingItems = scene()->items(newRect);
    for (QGraphicsItem* item : collidingItems) {
        if (item != this && item->type() == QGraphicsPathItem::Type) {
            return true;
        }
    }
    return false;
}

void Tank::turnLeft() {
    setRotation(rotation() - 10);
}

void Tank::turnRight() {
    setRotation(rotation() + 10);
}

void Tank::fire() {
    Bullet* bullet = new Bullet();
// 使用圆点代替子弹
bullet->setRect(0, 0, 5, 5);
    bullet->setPos(x(), y());
    bullet->setRotation(rotation());
    scene()->addItem(bullet);
}

