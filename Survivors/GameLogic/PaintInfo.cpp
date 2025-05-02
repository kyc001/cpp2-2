#include "PaintInfo.h"
#include <iostream>

PaintInfo::PaintInfo(const QPixmap &target, int mX, int mY) : target(target), m_X(mX), m_Y(mY) {
    // 检查拷贝后的 target 是否为空
    if (this->target.isNull()) {
        std::cout << "警告: 在位置(" << mX << "," << mY << ")处创建PaintInfo时传入了空QPixmap或拷贝失败" << std::endl;
        // 如果传入的 target 为空，创建一个备用的红色 QPixmap
        this->target = QPixmap(20, 20);
        this->target.fill(Qt::red);
    }
}

const QPixmap &PaintInfo::getTarget() const {
    return target;
}

int PaintInfo::getMX() const {
    return m_X;
}

int PaintInfo::getMY() const {
    return m_Y;
}
