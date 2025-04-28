#include "Weapon.h"
#include "Enemy.h"
#include "Bullet.h"
#include "GameMap.h"
#include <iostream>
#include <cassert>

// 构造函数实现
EnemyDynamicWeapon::EnemyDynamicWeapon(GameMap *map_p, Enemy *user, Hero *target, int CD, int damage) :
    Weapon(3, damage, nullptr, map_p), CD(CD), cdn(CD), _user(user), target(target) {
    bullet_tot = DYNAMIC_BULLETS_MAX;
    curr_bullet_cnt = 0;
    for(int i = 0; i < bullet_tot; i++) {
        bullet_buffer.push_back(nullptr);
    }
}

// tick方法实现
void EnemyDynamicWeapon::tick() {
    if(cdn <= 0) {
        addBullet();
        cdn = CD;
    } else {
        cdn--;
    }

    for(auto & each_b : bullet_buffer) {
        if(each_b && each_b->isEnabled()) {
            try {
                each_b->tick();
            } catch (OutOfRange &) {
                curr_bullet_cnt--;
            } catch (TargetHit &) {
                curr_bullet_cnt--;
            }
        }
    }
}

// addBullet方法实现
void EnemyDynamicWeapon::addBullet() {
    if(curr_bullet_cnt >= bullet_tot) {
        return;
    }
    for(auto & space: bullet_buffer) {
        if(space == nullptr) {
            enableNullSpace(space);
            return;
        } else if(!space->isEnabled()) {
            enableUsedSpace(space);
            return;
        }
    }
}

// enableNullSpace方法实现
void EnemyDynamicWeapon::enableNullSpace(Bullet *&space) {
    assert(space == nullptr);
    space = new EnemyDynamicBullet(map_parent, target, _user, damage);
    space->enable();
    curr_bullet_cnt++;
}

// enableUsedSpace方法实现
void EnemyDynamicWeapon::enableUsedSpace(Bullet *&space) {
    assert(space);
    space->enable();
    curr_bullet_cnt++;
}

// paint方法实现
std::vector<PaintInfo> EnemyDynamicWeapon::paint() {
    std::vector<PaintInfo> buffer;
    for(auto & item : bullet_buffer) {
        if(item && item->isEnabled()) {
            buffer.push_back(item->paint());
        }
    }
    return buffer;
} 