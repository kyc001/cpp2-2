#include "button.h"
#include <QPropertyAnimation>

button::button(QString normalImg) : QPushButton(nullptr) {
    normalImgPath = normalImg;
    QPixmap pix;
    if (!pix.load(normalImg)) {
        qDebug() << "图片加载失败" << normalImg;
        return;
    }
    setupButton(pix);
}

void button::setupButton(const QPixmap& pix) {
    this->setFixedSize(pix.width(), pix.height());
    this->setStyleSheet("QPushButton{border:0px;}");
    this->setIcon(pix);
    this->setIconSize(QSize(pix.width(), pix.height()));
}

void button::pop1() {
    createAnimation(0, 10);
}

void button::pop2() {
    createAnimation(10, 0);
}

void button::createAnimation(int startYOffset, int endYOffset) {
    QPropertyAnimation* animation = new QPropertyAnimation(this, "geometry");
    animation->setDuration(200);
    animation->setStartValue(QRect(this->x(), this->y() + startYOffset, this->width(), this->height()));
    animation->setEndValue(QRect(this->x(), this->y() + endYOffset, this->width(), this->height()));
    animation->setEasingCurve(QEasingCurve::OutBounce);
    animation->start();
}
