#include "../../include/entities/enemy.h"
#include "../../include/entities/hero.h"
#include "../../include/core/gamestate.h"
#include "../../include/core/gamemap.h"
#include <QtMath>
#include <cstdlib>
#include <ctime>

Enemy::Enemy(int type, int x, int y, GameMap* map, GameState* state, QObject* parent)
    : QObject(parent), enemy_type(type), my_x(x), my_y(y), real_x(x), real_y(y),
      game_map(map), game_state(state), is_alive(true), is_active(true), current_cooldown(0) {
    
    // Initialize enemy based on type
    initEnemy();
    
    // Set random direction for fixed movement type
    if (movement_type == EnemyMovement::FIXED_DIRECTION) {
        // Random direction (-1, 0, 1) for both x and y, but not both 0
        do {
            direction_x = (std::rand() % 3) - 1;  // -1, 0, or 1
            direction_y = (std::rand() % 3) - 1;  // -1, 0, or 1
        } while (direction_x == 0 && direction_y == 0);
    }
}

Enemy::~Enemy() {
    // Destructor
}

void Enemy::initEnemy() {
    // Initialize enemy parameters based on type
    switch(enemy_type) {
        case 0: // Basic melee enemy
            my_HP = 30;
            my_attack = 10;
            my_speed = 2;
            attack_range = 1; // Melee range
            attack_cooldown = 1000; // 1 second
            enemy_attack_type = EnemyType::MELEE;
            movement_type = EnemyMovement::FOLLOW_PLAYER;
            break;
        case 1: // Fast melee enemy
            my_HP = 20;
            my_attack = 8;
            my_speed = 3;
            attack_range = 1; // Melee range
            attack_cooldown = 800; // 0.8 seconds
            enemy_attack_type = EnemyType::MELEE;
            movement_type = EnemyMovement::FOLLOW_PLAYER;
            break;
        case 2: // Ranged enemy
            my_HP = 25;
            my_attack = 15;
            my_speed = 2;
            attack_range = 5; // Ranged attack
            attack_cooldown = 1500; // 1.5 seconds
            enemy_attack_type = EnemyType::RANGED;
            movement_type = EnemyMovement::FIXED_DIRECTION;
            break;
        case 3: // Tank melee enemy
            my_HP = 60;
            my_attack = 20;
            my_speed = 1;
            attack_range = 1; // Melee range
            attack_cooldown = 1200; // 1.2 seconds
            enemy_attack_type = EnemyType::MELEE;
            movement_type = EnemyMovement::FIXED_DIRECTION;
            break;
        default: // Default enemy
            my_HP = 30;
            my_attack = 10;
            my_speed = 2;
            attack_range = 1;
            attack_cooldown = 1000;
            enemy_attack_type = EnemyType::MELEE;
            movement_type = EnemyMovement::FOLLOW_PLAYER;
    }
}

int Enemy::getX() const {
    return my_x;
}

int Enemy::getY() const {
    return my_y;
}

int Enemy::getHP() const {
    return my_HP;
}

void Enemy::setHP(int HP) {
    my_HP = HP;
    if (my_HP <= 0) {
        my_HP = 0;
        is_alive = false;
        is_active = false;
    }
}

int Enemy::getAttack() const {
    return my_attack;
}

void Enemy::move() {
    if (!is_alive || !is_active) {
        return;
    }
    
    // Use the appropriate movement strategy
    if (movement_type == EnemyMovement::FOLLOW_PLAYER) {
        followPlayer();
    } else {
        moveFixedDirection();
    }
    
    // Update the grid position
    my_x = qRound(real_x);
    my_y = qRound(real_y);
}

void Enemy::followPlayer() {
    // Get player position
    Hero* hero = game_state->getHero();
    if (!hero || !hero->isAlive()) {
        return;
    }
    
    int player_x = hero->getX();
    int player_y = hero->getY();
    
    // Calculate direction to player
    double dx = 0, dy = 0;
    
    if (player_x > my_x) dx = 1;
    else if (player_x < my_x) dx = -1;
    
    if (player_y > my_y) dy = 1;
    else if (player_y < my_y) dy = -1;
    
    // Calculate new position
    double new_x = real_x + dx * (my_speed / 10.0);
    double new_y = real_y + dy * (my_speed / 10.0);
    
    // Check for barriers
    int new_grid_x = qRound(new_x);
    int new_grid_y = qRound(new_y);
    
    if (!game_map->isBarrier(new_grid_x, new_grid_y)) {
        real_x = new_x;
        real_y = new_y;
    }
}

void Enemy::moveFixedDirection() {
    // Calculate new position
    double new_x = real_x + direction_x * (my_speed / 10.0);
    double new_y = real_y + direction_y * (my_speed / 10.0);
    
    // Check for barriers or map boundaries
    int new_grid_x = qRound(new_x);
    int new_grid_y = qRound(new_y);
    
    if (!game_map->isBarrier(new_grid_x, new_grid_y)) {
        real_x = new_x;
        real_y = new_y;
    } else {
        // Change direction when hitting a barrier
        direction_x = -direction_x;
        direction_y = -direction_y;
    }
}

void Enemy::attack(Hero* hero) {
    if (!is_alive || !is_active || current_cooldown > 0) {
        return;
    }
    
    if (!hero || !hero->isAlive()) {
        return;
    }
    
    // Check if hero is in range
    int distance = qSqrt(qPow(hero->getX() - my_x, 2) + qPow(hero->getY() - my_y, 2));
    
    if (distance <= attack_range) {
        // Apply damage to hero
        hero->takeDamage(my_attack);
        current_cooldown = attack_cooldown;
    }
}

void Enemy::update() {
    if (!is_alive || !is_active) {
        return;
    }
    
    // Update cooldown
    if (current_cooldown > 0) {
        current_cooldown -= 16; // Assuming 60 fps (16ms per frame)
        if (current_cooldown < 0) {
            current_cooldown = 0;
        }
    }
    
    // Move the enemy
    move();
    
    // Attack the player if possible
    attack(game_state->getHero());
}

bool Enemy::isAlive() const {
    return is_alive;
}

bool Enemy::isActive() const {
    return is_active;
}

QRect Enemy::getCollisionRect() const {
    return QRect(my_x - 0.5, my_y - 0.5, 1, 1); // 1x1 collision box
}

int Enemy::getType() const {
    return enemy_type;
}

EnemyType Enemy::getEnemyType() const {
    return enemy_attack_type;
}

EnemyMovement Enemy::getMovementType() const {
    return movement_type;
}

void Enemy::takeDamage(int damage) {
    setHP(my_HP - damage);
}
