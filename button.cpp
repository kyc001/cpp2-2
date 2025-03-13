#include "button.h"
#include <QPropertyAnimation>

button::button(QString normalImg) : QPushButton(nullptr) {
    normalImgPath = normalImg;
    QPixmap pix;
    bool ret = pix.load(normalImg);
    if (!ret) {
        qDebug() << "图片加载失败" << normalImg;
        return;
    }
    this->setFixedSize(pix.width(), pix.height());
    this->setStyleSheet("QPushButton{border:0px;}");
    this->setIcon(pix);
    this->setIconSize(QSize(pix.width(), pix.height()));
}

void button::pop1() {
    // 创建动态属性动画
    QPropertyAnimation* animation = new QPropertyAnimation(this, "geometry");
    // 设置动画时间间隔
    animation->setDuration(200);
    // 设置动画起始位置
    animation->setStartValue(QRect(this->x(), this->y(), this->width(), this->height()));
    // 设置动画结束位置
    animation->setEndValue(QRect(this->x(), this->y() + 10, this->width(), this->height()));
    // 设置动画曲线
    animation->setEasingCurve(QEasingCurve::OutBounce);
    // 开始动画
    animation->start();
}

void button::pop2() {
    QPropertyAnimation* animation = new QPropertyAnimation(this, "geometry");
    animation->setDuration(200);
    animation->setStartValue(QRect(this->x(), this->y() + 10, this->width(), this->height()));
    animation->setEndValue(QRect(this->x(), this->y(), this->width(), this->height()));
    animation->setEasingCurve(QEasingCurve::OutBounce);
    animation->start();
}
