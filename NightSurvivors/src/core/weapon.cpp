#include "../../include/core/weapon.h"
#include "../../include/entities/hero.h"
#include "../../include/entities/enemy.h"
#include "../../include/core/gamestate.h"
#include <QTimer>
#include <QRandomGenerator>

Weapon::Weapon(int type, Hero* owner, GameState* state, QObject* parent)
    : QObject(parent),
      weapon_type(type),
      weapon_damage(10),
      cooldown_time(1000), // 1秒冷却
      duration_time(500),  // 0.5秒持续时间
      range(100),          // 攻击范围
      level(1),            // 初始等级
      owner(owner),
      game_state(state),
      is_active(false),
      is_cooling_down(false)
{
    // 初始化武器属性
    initWeapon();
    
    // 设置计时器
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
}

Weapon::~Weapon()
{
    cooldown_timer->stop();
    active_timer->stop();
}

void Weapon::initWeapon()
{
    // 根据武器类型初始化属性
    switch (weapon_type) {
        case 0: // 剑
            weapon_damage = 15;
            cooldown_time = 1000; // 1秒
            duration_time = 500;  // 0.5秒
            range = 120;
            break;
            
        case 1: // 魔杖
            weapon_damage = 20;
            cooldown_time = 1500; // 1.5秒
            duration_time = 300;  // 0.3秒
            range = 200;
            break;
            
        case 2: // 弓箭
            weapon_damage = 12;
            cooldown_time = 800;  // 0.8秒
            duration_time = 400;  // 0.4秒
            range = 250;
            break;
            
        case 3: // 匕首
            weapon_damage = 10;
            cooldown_time = 500;  // 0.5秒
            duration_time = 300;  // 0.3秒
            range = 100;
            break;
            
        default:
            weapon_damage = 15;
            cooldown_time = 1000; // 1秒
            cooldown_time = 500;  // 0.5秒
            range = 120;
            break;
    }
    
    // 应用英雄的攻击加成
    if (owner) {
        weapon_damage += owner->getAttack();
    }
}

int Weapon::getDamage() const
{
    return weapon_damage;
}

int Weapon::getCooldown() const
{
    return cooldown_time;
}

int Weapon::getDuration() const
{
    return duration_time;
}

int Weapon::getType() const
{
    return weapon_type;
}

void Weapon::increaseDamage(double percentage)
{
    weapon_damage = static_cast<int>(weapon_damage * (1.0 + percentage / 100.0));
}

void Weapon::decreaseCooldown(double percentage)
{
    cooldown_time = static_cast<int>(cooldown_time * (1.0 - percentage / 100.0));
    if (cooldown_time < 100) { // 最低冷却时间限制
        cooldown_time = 100;
    }
}

void Weapon::increaseDuration(double percentage)
{
    duration_time = static_cast<int>(duration_time * (1.0 + percentage / 100.0));
}

void Weapon::increaseRange(double percentage)
{
    range = static_cast<int>(range * (1.0 + percentage / 100.0));
}

void Weapon::levelUp(int levels)
{
    for (int i = 0; i < levels; ++i) {
        level++;
        
        // 随机选择一种属性进行提升
        int upgrade_type = QRandomGenerator::global()->bounded(4);
        
        switch (upgrade_type) {
            case 0: // 提升伤害
                increaseDamage(10.0); // 10%伤害提升
                break;
                
            case 1: // 减少冷却时间
                decreaseCooldown(5.0); // 5%冷却减少
                break;
                
            case 2: // 提升持续时间
                increaseDuration(10.0); // 10%持续时间提升
                break;
                
            case 3: // 提升攻击范围
                increaseRange(5.0); // 5%范围提升
                break;
        }
    }
}

void Weapon::attack()
{
    if (is_cooling_down || !owner) {
        return;
    }
    
    // 标记武器为活跃状态
    is_active = true;
    is_cooling_down = true;
    
    // 启动计时器
    active_timer->start(duration_time);
    cooldown_timer->start(cooldown_time);
    
    // 获取敌人列表
    if (!game_state) {
        return;
    }
    
    QVector<Enemy*> enemies = game_state->getEnemies();
    QRect damageArea = getDamageArea();
    
    // 对范围内的敌人造成伤害
    for (Enemy* enemy : enemies) {
        if (enemy && enemy->isAlive() && enemy->isActive()) {
            QRect enemyRect = enemy->getCollisionRect();
            
            if (damageArea.intersects(enemyRect)) {
                enemy->takeDamage(weapon_damage);
            }
        }
    }
    
    // 通知英雄进入攻击状态（用于动画）
    if (owner) {
        owner->setAttacking(true);
    }
}

bool Weapon::canAttack() const
{
    return !is_cooling_down;
}

void Weapon::update()
{
    // 自动攻击系统（类似吸血鬼幸存者）
    if (canAttack() && owner && game_state) {
        QVector<Enemy*> enemies = game_state->getEnemies();
        
        // 如果有敌人，自动攻击
        if (!enemies.isEmpty()) {
            attack();
        }
    }
}

QRect Weapon::getDamageArea() const
{
    if (!owner) {
        return QRect();
    }
    
    int heroX = owner->getX();
    int heroY = owner->getY();
    
    // 根据武器类型返回不同的攻击范围
    switch (weapon_type) {
        case 0: // 剑 - 矩形区域
            return QRect(heroX - range/2, heroY - range/2, range, range);
            
        case 1: // 魔杖 - 圆形区域（用矩形近似）
            return QRect(heroX - range/2, heroY - range/2, range, range);
            
        case 2: // 弓箭 - 直线攻击
            {
                // 假设向右攻击
                return QRect(heroX, heroY - 10, range, 20);
            }
            
        case 3: // 匕首 - 小范围矩形
            return QRect(heroX - range/3, heroY - range/3, range*2/3, range*2/3);
            
        default:
            return QRect(heroX - range/2, heroY - range/2, range, range);
    }
}

bool Weapon::isActive() const
{
    return is_active;
} 