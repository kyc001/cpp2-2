#include "../../include/entities/enemy.h"
#include "../../include/entities/hero.h"
#include "../../include/core/gamestate.h"
#include "../../include/core/gamemap.h"
#include "../../include/utils/resourcemanager.h"
#include <QtMath>
#include <cstdlib>
#include <ctime>
#include <QRandomGenerator>
#include <QPainter>

Enemy::Enemy(int type, int x, int y, GameMap* map, GameState* state, QObject* parent)
    : QObject(parent), 
      enemy_type(type), 
      my_x(x), 
      my_y(y), 
      real_x(x), 
      real_y(y),
      my_HP(50),
      my_attack(10),
      my_speed(1),
      attack_range(1),
      attack_cooldown(30),
      current_cooldown(0),
      is_alive(true),
      is_active(true),
      enemy_attack_type(EnemyType::MELEE),
      movement_type(EnemyMovement::FOLLOW_PLAYER),
      direction_x(0),
      direction_y(0),
      game_map(map), 
      game_state(state),
      animation_frame(0),
      animation_time(0),
      is_attacking(false),
      is_moving(false) {
    
    // Initialize enemy based on type
    initEnemy();
    
    // Set random direction for fixed movement type
    if (movement_type == EnemyMovement::FIXED_DIRECTION) {
        direction_x = QRandomGenerator::global()->bounded(-1, 2); // -1, 0, 1
        direction_y = QRandomGenerator::global()->bounded(-1, 2); // -1, 0, 1
        
        // Ensure at least one direction is not 0
        if (direction_x == 0 && direction_y == 0) {
            direction_x = 1;
        }
    }
    
    // Load enemy resources
    ResourceManager::getInstance().loadEnemyResources(enemy_type);
}

Enemy::~Enemy() {
    // Destructor
}

void Enemy::initEnemy() {
    // Initialize enemy parameters based on type
    switch(enemy_type) {
        case 0: // Basic melee enemy
            my_HP = 50;
            my_attack = 10;
            my_speed = 1;
            attack_range = 1; // Melee range
            attack_cooldown = 30; // Attack cooldown frames
            enemy_attack_type = EnemyType::MELEE;
            movement_type = EnemyMovement::FOLLOW_PLAYER;
            break;
        case 1: // Fast melee enemy
            my_HP = 30;
            my_attack = 8;
            my_speed = 2;
            attack_range = 1; // Melee range
            attack_cooldown = 20; // 0.8 seconds
            enemy_attack_type = EnemyType::MELEE;
            movement_type = EnemyMovement::FOLLOW_PLAYER;
            break;
        case 2: // Ranged enemy
            my_HP = 40;
            my_attack = 15;
            my_speed = 1;
            attack_range = 5; // Ranged attack
            attack_cooldown = 60; // 1.5 seconds
            enemy_attack_type = EnemyType::RANGED;
            movement_type = EnemyMovement::FIXED_DIRECTION;
            break;
        case 3: // Tank melee enemy
            my_HP = 150;
            my_attack = 20;
            my_speed = 0.5;
            attack_range = 1; // Melee range
            attack_cooldown = 45; // 1.2 seconds
            enemy_attack_type = EnemyType::MELEE;
            movement_type = EnemyMovement::FOLLOW_PLAYER;
            break;
        case 4: // Boss enemy
            my_HP = 500;
            my_attack = 30;
            my_speed = 0.8;
            attack_range = 2;
            attack_cooldown = 60; // 1.5 seconds
            enemy_attack_type = EnemyType::MELEE;
            movement_type = EnemyMovement::FOLLOW_PLAYER;
            break;
        default: // Default enemy
            my_HP = 50;
            my_attack = 10;
            my_speed = 1;
            attack_range = 1;
            attack_cooldown = 30;
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
    
    // Update animation
    updateAnimation();
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
    
    // 受到伤害时闪烁效果可以在这里添加
}

void Enemy::render(QPainter* painter) {
    if (!painter || !is_alive || !is_active) return;
    
    // 获取屏幕位置
    int screen_x = my_x * 32;  // 假设单元格大小为32x32
    int screen_y = my_y * 32;
    
    QPixmap enemySprite;
    
    // 根据状态选择合适的精灵
    if (is_attacking) {
        // 攻击动画
        enemySprite = ResourceManager::getInstance().getEnemyAttackImage(enemy_type, animation_frame % 3);
    } else if (is_moving) {
        // 行走动画
        enemySprite = ResourceManager::getInstance().getEnemyWalkImage(enemy_type, animation_frame % 4);
    } else {
        // 静止状态
        enemySprite = ResourceManager::getInstance().getEnemyIdleImage(enemy_type);
    }
    
    if (!enemySprite.isNull()) {
        // 绘制敌人精灵
        painter->drawPixmap(screen_x - enemySprite.width()/2, screen_y - enemySprite.height()/2, enemySprite);
    } else {
        // 绘制后备显示（简单的彩色矩形）
        painter->setBrush(QColor(200, 0, 0));
        painter->drawEllipse(QPointF(screen_x, screen_y), 16, 16);
    }
}

void Enemy::updateAnimation() {
    // 更新动画计时器
    animation_time++;
    
    // 每10帧更新一次动画帧
    if (animation_time >= 10) {
        animation_time = 0;
        animation_frame++;
        
        // 如果是攻击动画且完成了周期，则结束攻击状态
        if (is_attacking && animation_frame % 3 == 0) {
            is_attacking = false;
        }
    }
    
    // 判断敌人是否正在移动
    is_moving = true;  // 敌人通常总是在移动，可以根据速度判断
}
