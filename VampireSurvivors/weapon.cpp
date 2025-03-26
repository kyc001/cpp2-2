#include "bin/weapon.h"
#include "bin/hero.h"
#include "bin/enemy.h"
#include "bin/gamestate.h"

Weapon::Weapon(int type, Hero* owner, GameState* state, QObject* parent)
    : QObject(parent), weapon_type(type), owner(owner), game_state(state), 
      is_active(false), is_cooling_down(false) {
    
    // Initialize cooldown and active timers
    cooldown_timer = new QTimer(this);
    active_timer = new QTimer(this);
    
    connect(cooldown_timer, &QTimer::timeout, this, [this]() {
        is_cooling_down = false;
        cooldown_timer->stop();
    });
    
    connect(active_timer, &QTimer::timeout, this, [this]() {
        is_active = false;
        active_timer->stop();
    });
    
    // Initialize weapon properties based on type
    initWeapon();
}

Weapon::~Weapon() {
    delete cooldown_timer;
    delete active_timer;
}

void Weapon::initWeapon() {
    // Different weapon types initialization
    switch(weapon_type) {
        case 0: // Sword - Close range, fast cooldown
            weapon_damage = 20;
            cooldown_time = 800;  // 0.8 seconds
            duration_time = 300;  // 0.3 seconds
            range = 2;            // Close range
            break;
        case 1: // Magic wand - Medium range, medium cooldown
            weapon_damage = 15;
            cooldown_time = 1200; // 1.2 seconds
            duration_time = 400;  // 0.4 seconds
            range = 5;            // Medium range
            break;
        case 2: // Bow - Long range, slow cooldown
            weapon_damage = 30;
            cooldown_time = 2000; // 2 seconds
            duration_time = 200;  // 0.2 seconds
            range = 8;            // Long range
            break;
        default:
            weapon_damage = 10;
            cooldown_time = 1000;
            duration_time = 300;
            range = 3;
    }
}

int Weapon::getDamage() const {
    return weapon_damage;
}

int Weapon::getCooldown() const {
    return cooldown_time;
}

int Weapon::getDuration() const {
    return duration_time;
}

int Weapon::getType() const {
    return weapon_type;
}

void Weapon::attack() {
    if (is_cooling_down) {
        return; // Cannot attack while cooling down
    }
    
    // Activate the weapon
    is_active = true;
    active_timer->start(duration_time);
    
    // Start cooldown after attack
    is_cooling_down = true;
    cooldown_timer->start(cooldown_time);
}

bool Weapon::canAttack() const {
    return !is_cooling_down;
}

bool Weapon::isActive() const {
    return is_active;
}

void Weapon::update() {
    // Auto-attack logic can be implemented here for survival mode
    if (canAttack()) {
        attack();
    }
}

QRect Weapon::getDamageArea() const {
    if (!is_active) {
        return QRect();
    }
    
    // Calculate damage area based on weapon type and owner position
    int x = owner->getX();
    int y = owner->getY();
    
    switch(weapon_type) {
        case 0: // Sword - melee in front of player
            return QRect(x - range, y - range, range * 2, range * 2);
        case 1: // Magic wand - circular area
            return QRect(x - range, y - range, range * 2, range * 2);
        case 2: // Bow - directional in front of player
            return QRect(x - range, y - range, range * 2, range * 2);
        default:
            return QRect(x - range, y - range, range * 2, range * 2);
    }
} 