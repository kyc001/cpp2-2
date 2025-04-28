#include "PaintInfo.h"
#include <iostream>

PaintInfo::PaintInfo(QPixmap &target, int mX, int mY) : target(target), m_X(mX), m_Y(mY) {
    // 确保目标QPixmap不为空
    if (target.isNull()) {
        std::cout << "警告: 在位置(" << mX << "," << mY << ")处创建PaintInfo时传入了空QPixmap" << std::endl;
        // 创建一个新的QPixmap对象而不是修改引用
        static QPixmap fallbackPixmap(20, 20);
        static bool initialized = false;
        
        if (!initialized) {
            fallbackPixmap.fill(Qt::red);
            initialized = true;
        }
        
        // 由于我们使用的是引用，只能让目标指向一个已经存在的对象
        this->target = fallbackPixmap;
    }
}

QPixmap &PaintInfo::getTarget() const {
    return target;
}

int PaintInfo::getMX() const {
    return m_X;
}

int PaintInfo::getMY() const {
    return m_Y;
}
