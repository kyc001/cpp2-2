#include "bin/drop.h"
#include "bin/hero.h"
#include "bin/gamestate.h"

Drop::Drop(DropType type, int value, int x, int y, GameState* state, QObject* parent)
    : QObject(parent), drop_type(type), drop_value(value), x_pos(x), y_pos(y),
      is_active(true), game_state(state) {
}

Drop::~Drop() {
}

int Drop::getX() const {
    return x_pos;
}

int Drop::getY() const {
    return y_pos;
}

bool Drop::isActive() const {
    return is_active;
}

void Drop::setActive(bool active) {
    is_active = active;
}

DropType Drop::getType() const {
    return drop_type;
}

int Drop::getValue() const {
    return drop_value;
}

QRect Drop::getCollisionRect() const {
    return QRect(x_pos-0.5, y_pos-0.5, 1, 1); // 1x1 碰撞盒
}

void Drop::applyEffect(Hero* hero) {
    if (!hero || !is_active) {
        return;
    }
    
    switch (drop_type) {
        case DropType::HEALTH:
            // 回复生命值
            hero->setHP(hero->getHP() + drop_value);
            break;
            
        case DropType::EXPERIENCE:
            // 获得经验值
            hero->addEXP(drop_value);
            break;
            
        case DropType::COIN:
            // 获得金币，通知GameState增加金币
            if (game_state) {
                game_state->addCoins(drop_value);
            }
            break;
            
        case DropType::POWER_UP:
            // 临时增强效果
            hero->addAttack(drop_value);
            break;
    }
    
    // 物品被拾取后失效
    is_active = false;
} 