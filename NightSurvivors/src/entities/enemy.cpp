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
#include <cmath>

Enemy::Enemy(int type, int x, int y, GameMap *map, GameState *state, QObject *parent)
    : QObject(parent),
      enemy_type(type),
      my_x(x),
      my_y(y),
      real_x(x),
      real_y(y),
      my_HP(100),
      my_attack(10),
      my_speed(2.0),
      attack_range(50),
      attack_cooldown(1000), // 1秒攻击冷却
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
      is_moving(false)
{

    // Initialize enemy based on type
    initEnemy();

    // Set random direction for fixed movement type
    if (movement_type == EnemyMovement::FIXED_DIRECTION)
    {
        // 随机选择一个方向
        int direction = QRandomGenerator::global()->bounded(8);

        switch (direction)
        {
        case 0:
            direction_x = 0;
            direction_y = -1;
            break; // 上
        case 1:
            direction_x = 1;
            direction_y = -1;
            break; // 右上
        case 2:
            direction_x = 1;
            direction_y = 0;
            break; // 右
        case 3:
            direction_x = 1;
            direction_y = 1;
            break; // 右下
        case 4:
            direction_x = 0;
            direction_y = 1;
            break; // 下
        case 5:
            direction_x = -1;
            direction_y = 1;
            break; // 左下
        case 6:
            direction_x = -1;
            direction_y = 0;
            break; // 左
        case 7:
            direction_x = -1;
            direction_y = -1;
            break; // 左上
        }
    }

    // Load enemy resources
    ResourceManager::getInstance().loadEnemyResourcesByType(enemy_type);
}

Enemy::~Enemy()
{
    // Destructor
}

void Enemy::initEnemy()
{
    // Initialize enemy parameters based on type
    switch (enemy_type)
    {
    case 0: // Basic melee enemy
        my_HP = 30;
        my_attack = 5;
        my_speed = 2.0;
        attack_range = 50;
        attack_cooldown = 1000; // 1秒
        enemy_attack_type = EnemyType::MELEE;
        movement_type = EnemyMovement::FOLLOW_PLAYER;
        break;
    case 1: // Fast melee enemy
        my_HP = 20;
        my_attack = 3;
        my_speed = 3.5;
        attack_range = 40;
        attack_cooldown = 800; // 0.8秒
        enemy_attack_type = EnemyType::MELEE;
        movement_type = EnemyMovement::FOLLOW_PLAYER;
        break;
    case 2: // Ranged enemy
        my_HP = 80;
        my_attack = 8;
        my_speed = 1.0;
        attack_range = 60;
        attack_cooldown = 1500; // 1.5秒
        enemy_attack_type = EnemyType::MELEE;
        movement_type = EnemyMovement::FOLLOW_PLAYER;
        break;
    case 3: // Tank melee enemy
        my_HP = 25;
        my_attack = 7;
        my_speed = 1.5;
        attack_range = 200;
        attack_cooldown = 1200; // 1.2秒
        enemy_attack_type = EnemyType::RANGED;
        movement_type = EnemyMovement::FIXED_DIRECTION;
        break;
    case 4: // Boss enemy
        my_HP = 120;
        my_attack = 12;
        my_speed = 2.0;
        attack_range = 70;
        attack_cooldown = 1000; // 1秒
        enemy_attack_type = EnemyType::MELEE;
        movement_type = EnemyMovement::FOLLOW_PLAYER;
        break;
    case 5: // BOSS enemy
        my_HP = 500;
        my_attack = 20;
        my_speed = 1.8;
        attack_range = 100;
        attack_cooldown = 2000; // 2秒
        enemy_attack_type = EnemyType::MELEE;
        movement_type = EnemyMovement::FOLLOW_PLAYER;
        break;
    default: // Default enemy
        my_HP = 30;
        my_attack = 5;
        my_speed = 2.0;
        attack_range = 50;
        attack_cooldown = 1000; // 1秒
        enemy_attack_type = EnemyType::MELEE;
        movement_type = EnemyMovement::FOLLOW_PLAYER;
    }
}

int Enemy::getX() const
{
    return my_x;
}

int Enemy::getY() const
{
    return my_y;
}

int Enemy::getHP() const
{
    return my_HP;
}

void Enemy::setHP(int HP)
{
    my_HP = HP;
    if (my_HP <= 0)
    {
        my_HP = 0;
        is_alive = false;
        is_active = false;
    }
}

int Enemy::getAttack() const
{
    return my_attack;
}

void Enemy::move()
{
    if (!is_alive || !is_active)
    {
        return;
    }

    is_moving = true;

    if (movement_type == EnemyMovement::FOLLOW_PLAYER)
    {
        followPlayer();
    }
    else
    {
        moveFixedDirection();
    }

    // Update the grid position
    my_x = static_cast<int>(real_x);
    my_y = static_cast<int>(real_y);
}

void Enemy::followPlayer()
{
    if (!game_state || !game_state->getHero())
    {
        return;
    }

    Hero *hero = game_state->getHero();

    int hero_x = hero->getX();
    int hero_y = hero->getY();

    // Calculate direction to player
    double dx = hero_x - real_x;
    double dy = hero_y - real_y;

    // Calculate distance
    double distance = std::sqrt(dx * dx + dy * dy);

    // If distance is less than attack range and attack type is melee, stop moving
    if (distance <= attack_range && enemy_attack_type == EnemyType::MELEE)
    {
        is_moving = false;
        return;
    }

    // If distance is greater than minimum attack distance and attack type is ranged, try to maintain distance
    if (distance <= attack_range * 0.7 && enemy_attack_type == EnemyType::RANGED)
    {
        // Ranged enemy tries to maintain distance
        dx = -dx;
        dy = -dy;
    }

    // Normalize direction vector
    if (distance > 0)
    {
        dx /= distance;
        dy /= distance;
    }

    // Calculate new position
    double new_x = real_x + dx * my_speed;
    double new_y = real_y + dy * my_speed;

    // Check for barriers
    if (game_map && !game_map->isBarrier(static_cast<int>(new_x), static_cast<int>(new_y)))
    {
        real_x = new_x;
        real_y = new_y;
    }
    else
    {
        // If hit a barrier, try horizontal or vertical movement
        if (!game_map->isBarrier(static_cast<int>(real_x + dx * my_speed), static_cast<int>(real_y)))
        {
            real_x += dx * my_speed;
        }
        else if (!game_map->isBarrier(static_cast<int>(real_x), static_cast<int>(real_y + dy * my_speed)))
        {
            real_y += dy * my_speed;
        }
    }
}

void Enemy::moveFixedDirection()
{
    // Calculate new position
    double new_x = real_x + direction_x * my_speed;
    double new_y = real_y + direction_y * my_speed;

    // Check for barriers or map boundaries
    if (game_map && !game_map->isBarrier(static_cast<int>(new_x), static_cast<int>(new_y)))
    {
        real_x = new_x;
        real_y = new_y;
    }
    else
    {
        // Change direction when hitting a barrier
        // Randomly select a new direction but don't select the opposite direction
        int new_direction;
        do
        {
            new_direction = QRandomGenerator::global()->bounded(8);
        } while ((new_direction + 4) % 8 == direction_x * 3 + direction_y + 1);

        switch (new_direction)
        {
        case 0:
            direction_x = 0;
            direction_y = -1;
            break; // 上
        case 1:
            direction_x = 1;
            direction_y = -1;
            break; // 右上
        case 2:
            direction_x = 1;
            direction_y = 0;
            break; // 右
        case 3:
            direction_x = 1;
            direction_y = 1;
            break; // 右下
        case 4:
            direction_x = 0;
            direction_y = 1;
            break; // 下
        case 5:
            direction_x = -1;
            direction_y = 1;
            break; // 左下
        case 6:
            direction_x = -1;
            direction_y = 0;
            break; // 左
        case 7:
            direction_x = -1;
            direction_y = -1;
            break; // 左上
        }
    }
}

void Enemy::attack(Hero *hero)
{
    if (!is_alive || !is_active || !hero || current_cooldown > 0)
    {
        return;
    }

    // Calculate distance to hero
    int hero_x = hero->getX();
    int hero_y = hero->getY();

    double dx = hero_x - real_x;
    double dy = hero_y - real_y;
    double distance = std::sqrt(dx * dx + dy * dy);

    // If in attack range and attack cooldown has ended
    if (distance <= attack_range)
    {
        // Set attack state
        is_attacking = true;

        // Apply damage to hero
        hero->takeDamage(my_attack);

        // Reset attack cooldown
        current_cooldown = attack_cooldown;
    }
}

void Enemy::update()
{
    if (!is_alive || !is_active)
    {
        return;
    }

    // Update cooldown
    if (current_cooldown > 0)
    {
        current_cooldown -= 33; // Assuming 30FPS, each frame is about 33ms
        if (current_cooldown < 0)
        {
            current_cooldown = 0;
        }
    }

    // Move the enemy
    move();

    // Attack the player if possible
    if (game_state && game_state->getHero())
    {
        attack(game_state->getHero());
    }

    // Update animation
    updateAnimation();
}

bool Enemy::isAlive() const
{
    return is_alive;
}

bool Enemy::isActive() const
{
    return is_active;
}

QRect Enemy::getCollisionRect() const
{
    int size = 40; // Enemy collision box size
    return QRect(my_x - size / 2, my_y - size / 2, size, size);
}

int Enemy::getType() const
{
    return enemy_type;
}

EnemyType Enemy::getEnemyType() const
{
    return enemy_attack_type;
}

EnemyMovement Enemy::getMovementType() const
{
    return movement_type;
}

void Enemy::takeDamage(int damage)
{
    if (!is_alive || !is_active)
    {
        return;
    }

    setHP(my_HP - damage);

    // 受到伤害时闪烁效果可以在这里添加
}

void Enemy::render(QPainter *painter)
{
    if (!painter || !is_alive || !is_active)
        return;

    // Get hero position (screen center)
    Hero *hero = game_state ? game_state->getHero() : nullptr;
    if (!hero)
    {
        return;
    }

    int hero_x = hero->getX();
    int hero_y = hero->getY();

    // Calculate enemy position relative to hero
    int screen_width = painter->device()->width();
    int screen_height = painter->device()->height();

    int screen_x = screen_width / 2 + (my_x - hero_x);
    int screen_y = screen_height / 2 + (my_y - hero_y);

    // Select appropriate enemy sprite
    QString typeName;

    switch (enemy_type)
    {
    case 0:
        typeName = "melee_basic";
        break;
    case 1:
        typeName = "melee_fast";
        break;
    case 2:
        typeName = "tank";
        break;
    case 3:
        typeName = "ranged";
        break;
    case 4:
        typeName = "tank";
        break; // Reuse tank appearance
    case 5:
        typeName = "boss";
        break;
    default:
        typeName = "melee_basic";
        break;
    }

    QString spritePath;

    if (is_attacking)
    {
        spritePath = QString(":/resources/enemies/%1/%1_attack_%2.png")
                         .arg(typeName)
                         .arg(animation_frame % 3); // Attack animation has 3 frames
    }
    else if (is_moving)
    {
        spritePath = QString(":/resources/enemies/%1/%1_walk_%2.png")
                         .arg(typeName)
                         .arg(animation_frame % 4); // Walking animation has 4 frames
    }
    else
    {
        spritePath = QString(":/resources/enemies/%1/%1_idle.png").arg(typeName);
    }

    // Load and draw enemy sprite
    QPixmap sprite(spritePath);
    if (!sprite.isNull())
    {
        int spriteWidth = 64;
        int spriteHeight = 64;

        if (enemy_type == 5)
        { // BOSS is larger
            spriteWidth = 96;
            spriteHeight = 96;
        }
        else if (enemy_type == 2 || enemy_type == 4)
        { // Tank and elite enemies are larger
            spriteWidth = 80;
            spriteHeight = 80;
        }

        painter->drawPixmap(screen_x - spriteWidth / 2, screen_y - spriteHeight / 2,
                            spriteWidth, spriteHeight, sprite);

        // Draw health bar
        int healthBarWidth = spriteWidth;
        int healthBarHeight = 5;

        // Health bar background
        painter->setBrush(Qt::red);
        painter->setPen(Qt::NoPen);
        painter->drawRect(screen_x - healthBarWidth / 2,
                          screen_y - spriteHeight / 2 - 10,
                          healthBarWidth, healthBarHeight);

        // Current health
        int currentHealthWidth = static_cast<int>(healthBarWidth * (my_HP / (my_HP <= 0 ? 1.0 : my_HP)));
        painter->setBrush(Qt::green);
        painter->drawRect(screen_x - healthBarWidth / 2,
                          screen_y - spriteHeight / 2 - 10,
                          currentHealthWidth, healthBarHeight);
    }
}

void Enemy::updateAnimation()
{
    animation_time++;

    // Update attack state
    if (is_attacking && animation_time % 10 == 0)
    { // Update every 10 frames
        animation_frame = (animation_frame + 1) % 3;
        if (animation_frame == 0)
        {
            is_attacking = false; // Attack animation ends
        }
    }

    // Update movement animation
    if (is_moving && animation_time % 15 == 0)
    { // Update every 15 frames
        animation_frame = (animation_frame + 1) % 4;
    }
}
