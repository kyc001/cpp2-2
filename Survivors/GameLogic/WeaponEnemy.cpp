#include "Enemy.h"
#include "Weapon.h"
#include "GameMap.h"
#include "Hero.h"
#include <cmath>
#include <iostream>

void WeaponEnemy::tick() {
    if(direction) {
        setRealPosition(real_pos.first + speed, real_pos.second);
        if(real_pos.first > x_move_range.second) {
            real_pos.first = x_move_range.second;
            direction = false;
        }
    } else {
        setRealPosition(real_pos.first - speed, real_pos.second);
        if(real_pos.first < x_move_range.first) {
            real_pos.first = x_move_range.first;
            direction = true;
        }
    }
    _weapon->tick();
}

std::vector<PaintInfo> WeaponEnemy::paint() {
    std::vector<PaintInfo> buffer;
    
    // 创建静态QPixmap对象作为备用
    static QPixmap defaultPixmapBlue(40, 40);
    static QPixmap defaultPixmapMagenta(40, 40);
    static bool initialized = false;
    
    if (!initialized) {
        defaultPixmapBlue.fill(Qt::blue);
        defaultPixmapMagenta.fill(Qt::magenta);
        initialized = true;
    }
    
    // 首先添加基类的绘制内容
    if (!_image.isNull()) {
        static QPixmap pixmap;
        pixmap = QPixmap::fromImage(_image);
        
        if (!pixmap.isNull()) {
            buffer.push_back(PaintInfo(pixmap, absolute_pos.first, absolute_pos.second));
        } else {
            std::cout << "警告：武器敌人的QPixmap为空" << std::endl;
            buffer.push_back(PaintInfo(defaultPixmapBlue, absolute_pos.first, absolute_pos.second));
        }
    } else {
        std::cout << "警告：武器敌人的图像为空" << std::endl;
        buffer.push_back(PaintInfo(defaultPixmapMagenta, absolute_pos.first, absolute_pos.second));
    }
    
    // 然后添加武器的绘制内容
    if (_weapon) {
        std::vector<PaintInfo> weaponPaint = _weapon->paint();
        for(auto& item : weaponPaint) {
            buffer.push_back(item);
        }
    }
    
    return buffer;
} 