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
    // 1. 获取基类(Enemy)的绘制信息
    std::vector<PaintInfo> buffer = Enemy::paint(); // 调用基类paint
    
    // 2. 添加武器的绘制信息
    if (_weapon) {
        std::vector<PaintInfo> weaponPaint = _weapon->paint();
        // 将weaponPaint的内容添加到buffer中
        buffer.insert(buffer.end(), weaponPaint.begin(), weaponPaint.end());
    }
    
    return buffer;
} 