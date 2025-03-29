#include "../../include/entities/hero.h"
#include "../../include/core/gamestate.h"
#include "../../include/core/weapon.h"
#include "../../include/core/gamemap.h"
#include "../../include/utils/resourcemanager.h"
#include <QWidget>
#include <QKeyEvent>
#include <QRandomGenerator>
#include <algorithm>
#include <QProgressBar>
#include <QLabel>
#include <QPair>
#include <QPainter>
#include <QCursor>
#include <cmath>

Hero::Hero() : QObject(), is_alive(true), level(1), pickup_range(1), control_type(0) {
    // Default constructor
}

Hero::Hero(int style, QWidget* parent, GameMap* map_parent, GameState* state) 
    : QObject(parent), hero_style(style), game_map(map_parent), game_state(state), 
      is_alive(true), level(1), pickup_range(1), control_type(0) {
    
    // Initialize character based on style
    initCharacter();
    
    // Create UI elements
    HP_bar = new QProgressBar(parent);
    EXP_bar = new QProgressBar(parent);
    HP_label = new QLabel(parent);
    EXP_label = new QLabel(parent);
    level_label = new QLabel(parent);
    
    // Configure UI elements
    HP_bar->setRange(0, HP_MAX);
    HP_bar->setValue(my_HP);
    HP_bar->setGeometry(10, 10, 200, 20);
    HP_bar->setStyleSheet("QProgressBar { border: 2px solid #555; border-radius: 5px; background-color: #333; } "
                         "QProgressBar::chunk { background-color: #e33c3c; }");
    HP_bar->setTextVisible(false);
    
    EXP_bar->setRange(0, EXP_MAX);
    EXP_bar->setValue(my_EXP);
    EXP_bar->setGeometry(10, 40, 200, 20);
    EXP_bar->setStyleSheet("QProgressBar { border: 2px solid #555; border-radius: 5px; background-color: #333; } "
                           "QProgressBar::chunk { background-color: #4c96e8; }");
    EXP_bar->setTextVisible(false);
    
    HP_label->setText(QString("血量: %1/%2").arg(my_HP).arg(HP_MAX));
    HP_label->setGeometry(220, 10, 100, 20);
    HP_label->setStyleSheet("color: #ff5555; font-size: 14px; font-weight: bold;");
    
    EXP_label->setText(QString("经验: %1/%2").arg(my_EXP).arg(EXP_MAX));
    EXP_label->setGeometry(220, 40, 100, 20);
    EXP_label->setStyleSheet("color: #55aaff; font-size: 14px; font-weight: bold;");
    
    level_label->setText(QString("等级: %1").arg(level));
    level_label->setGeometry(220, 70, 100, 20);
    level_label->setStyleSheet("color: #ffcc00; font-size: 16px; font-weight: bold;");
    
    // Set starting position
    abspos = qMakePair(game_map->getWidth() / 2, game_map->getHeight() / 2);
    realpos = qMakePair(abspos.first * 1.0, abspos.second * 1.0);
    
    // Create weapon
    my_weapon = new Weapon(weapon_type, this, game_state);
    
    // 加载英雄资源
    ResourceManager::getInstance().loadHeroResources(hero_style);
    
    // 初始化动画帧计数器
    animation_frame = 0;
    animation_time = 0;
    is_attacking = false;
    is_moving = false;
}

Hero::~Hero() {
    delete HP_bar;
    delete EXP_bar;
    delete HP_label;
    delete EXP_label;
    delete level_label;
    delete my_weapon;
    
    // 清理升级选项
    for (auto upgrade : available_upgrades) {
        delete upgrade;
    }
    available_upgrades.clear();
}

void Hero::initCharacter() {
    // Initialize based on character style
    switch(hero_style) {
        case 0: // Warrior
            HP_MAX = 200;
            my_HP = HP_MAX;
            EXP_MAX = 100;
            my_EXP = 0;
            my_attack = 15;
            my_speed = 3;
            weapon_type = 0; // Sword
            break;
        case 1: // Mage
            HP_MAX = 120;
            my_HP = HP_MAX;
            EXP_MAX = 100;
            my_EXP = 0;
            my_attack = 20;
            my_speed = 4;
            weapon_type = 1; // Magic wand
            break;
        case 2: // Archer
            HP_MAX = 150;
            my_HP = HP_MAX;
            EXP_MAX = 100;
            my_EXP = 0;
            my_attack = 12;
            my_speed = 5;
            weapon_type = 2; // Bow
            break;
        default: // Default
            HP_MAX = 150;
            my_HP = HP_MAX;
            EXP_MAX = 100;
            my_EXP = 0;
            my_attack = 10;
            my_speed = 4;
            weapon_type = 0;
    }
}

int Hero::getHP() const {
    return my_HP;
}

int Hero::getMaxHP() const {
    return HP_MAX;
}

void Hero::setHP(int HP) {
    my_HP = HP;
    if (my_HP > HP_MAX) {
        my_HP = HP_MAX;
    }
    if (my_HP <= 0) {
        my_HP = 0;
        is_alive = false;
    }
    
    // Update UI
    HP_bar->setValue(my_HP);
    HP_label->setText(QString("血量: %1/%2").arg(my_HP).arg(HP_MAX));
}

int Hero::getEXP() const {
    return my_EXP;
}

void Hero::addEXP(int exp) {
    my_EXP += exp;
    
    // Update UI
    EXP_bar->setValue(my_EXP);
    EXP_label->setText(QString("经验: %1/%2").arg(my_EXP).arg(EXP_MAX));
    
    // 检查是否升级
    if (my_EXP >= EXP_MAX) {
        levelUp();
    }
}

int Hero::getAttack() const {
    return my_attack;
}

void Hero::addAttack(int attack) {
    my_attack += attack;
}

int Hero::getSpeed() const {
    return my_speed;
}

void Hero::addSpeed(float speed) {
    my_speed += speed;
}

int Hero::getWeaponType() const {
    return weapon_type;
}

int Hero::getLevel() const {
    return level;
}

int Hero::getPickupRange() const {
    return pickup_range;
}

void Hero::addPickupRange(int range) {
    pickup_range += range;
}

void Hero::setMaxHealth(int maxHP) {
    HP_MAX = maxHP;
    if (my_HP > HP_MAX) {
        my_HP = HP_MAX;
    }
}

void Hero::setBaseDamage(int damage) {
    my_attack = damage;
}

void Hero::setPickupRange(int range) {
    pickup_range = range;
}

void Hero::levelUp() {
    // 升级
    level++;
    
    // 重置经验值
    my_EXP = 0;
    
    // 提高升级所需经验
    EXP_MAX = 100 + (level - 1) * 20;
    
    // 更新UI
    EXP_bar->setRange(0, EXP_MAX);
    EXP_bar->setValue(my_EXP);
    EXP_label->setText(QString("经验: %1/%2").arg(my_EXP).arg(EXP_MAX));
    level_label->setText(QString("等级: %1").arg(level));
    
    // 生成升级选项
    generateUpgradeOptions();
}

void Hero::generateUpgradeOptions() {
    // 清理之前的升级选项
    for (auto upgrade : available_upgrades) {
        delete upgrade;
    }
    available_upgrades.clear();
    
    // 生成三个随机升级选项
    for (int i = 0; i < 3; i++) {
        int optionType = QRandomGenerator::global()->bounded(5); // 0-4，5种不同的升级选项
        UpgradeType upgradeType;
        int value = 0;
        QString description;
        
        switch (optionType) {
            case 0: // 增加最大生命值
                upgradeType = UpgradeType::HEALTH;
                value = 20;
                description = "增加20点最大生命值";
                break;
            case 1: // 增加攻击力
                upgradeType = UpgradeType::ATTACK;
                value = 5;
                description = "增加5点攻击力";
                break;
            case 2: // 增加移动速度
                upgradeType = UpgradeType::SPEED;
                value = 1; // 实际值会乘以0.5
                description = "增加移动速度";
                break;
            case 3: // 增加拾取范围
                upgradeType = UpgradeType::PICKUP_RANGE;
                value = 1;
                description = "增加拾取范围";
                break;
            case 4: // 提升武器等级
                upgradeType = UpgradeType::WEAPON_LEVEL;
                value = 1;
                description = "提升武器等级";
                break;
        }
        
        available_upgrades.push_back(new UpgradeOption(upgradeType, value, description));
    }
}

// 应用升级选项
void Hero::applyUpgrade(int upgradeIndex) {
    if (upgradeIndex < 0 || upgradeIndex >= available_upgrades.size()) {
        return;
    }
    
    UpgradeOption* selectedUpgrade = available_upgrades[upgradeIndex];
    
    switch (selectedUpgrade->getType()) {
        case UpgradeType::HEALTH: // 增加最大生命值
            HP_MAX += selectedUpgrade->getValue();
            setHP(my_HP + selectedUpgrade->getValue()); // 同时恢复部分血量
            break;
        case UpgradeType::ATTACK: // 增加攻击力
            addAttack(selectedUpgrade->getValue());
            break;
        case UpgradeType::SPEED: // 增加移动速度
            addSpeed(selectedUpgrade->getValue() * 0.5);
            break;
        case UpgradeType::PICKUP_RANGE: // 增加拾取范围
            addPickupRange(selectedUpgrade->getValue() * 10);
            break;
        case UpgradeType::WEAPON_LEVEL: // 提升武器等级
            if (my_weapon) {
                my_weapon->levelUp(selectedUpgrade->getValue());
            }
            break;
    }
    
    // 清理升级选项
    for (auto upgrade : available_upgrades) {
        delete upgrade;
    }
    available_upgrades.clear();
}

bool Hero::hasLeveledUp() const {
    return !available_upgrades.empty();
}

const std::vector<UpgradeOption*>& Hero::getAvailableUpgrades() const {
    return available_upgrades;
}

void Hero::move(int dx, int dy) {
    // Calculate new position
    double new_x = realpos.first + dx * (my_speed / 10.0);
    double new_y = realpos.second + dy * (my_speed / 10.0);
    
    // Calculate grid position
    int new_grid_x = qRound(new_x);
    int new_grid_y = qRound(new_y);
    
    // Check for barriers
    if (!game_map->isBarrier(new_grid_x, new_grid_y)) {
        realpos = qMakePair(new_x, new_y);
        abspos = qMakePair(new_grid_x, new_grid_y);
    }
}

void Hero::attack() {
    if (my_weapon->canAttack()) {
        my_weapon->attack();
    }
}

int Hero::getX() const {
    return abspos.first;
}

int Hero::getY() const {
    return abspos.second;
}

QPair<double, double> Hero::getRealPos() const {
    return realpos;
}

void Hero::processKeyPress(int key) {
    // Add key to pressed keys if not already there
    if (std::find(key_pressed.begin(), key_pressed.end(), key) == key_pressed.end()) {
        key_pressed.push_back(key);
    }
}

void Hero::processKeyRelease(int key) {
    // Remove key from pressed keys
    key_pressed.erase(std::remove(key_pressed.begin(), key_pressed.end(), key), key_pressed.end());
}

void Hero::updateMovement() {
    int dx = 0, dy = 0;
    
    if (control_type == 0) {
        // WASD键盘控制模式
        for (int key : key_pressed) {
            switch (key) {
                case Qt::Key_W: dy -= 1; break;
                case Qt::Key_S: dy += 1; break;
                case Qt::Key_A: dx -= 1; break;
                case Qt::Key_D: dx += 1; break;
            }
        }
    } else if (control_type == 1 && game_state) {
        // 鼠标控制模式 - 获取鼠标位置并向那个方向移动
        QWidget* parent = qobject_cast<QWidget*>(game_state->parent());
        if (parent) {
            QPoint mousePos = parent->mapFromGlobal(QCursor::pos());
            
            // 转换屏幕坐标到游戏坐标
            double mouseX = mousePos.x() / 20.0;  // 假设一个游戏单位是20像素
            double mouseY = mousePos.y() / 20.0;
            
            // 计算方向向量
            double diffX = mouseX - realpos.first;
            double diffY = mouseY - realpos.second;
            
            // 如果鼠标不在角色附近，则移动
            double distanceSquared = diffX * diffX + diffY * diffY;
            if (distanceSquared > 1.0) { // 有一定距离才移动
                // 归一化方向向量
                double length = std::sqrt(distanceSquared);
                dx = (diffX / length) * 2; // 乘以速度因子
                dy = (diffY / length) * 2;
            }
        }
    }
    
    // 应用移动
    if (dx != 0 || dy != 0) {
        move(dx, dy);
    }
}

QRect Hero::getCollisionRect() const {
    int x = abspos.first;
    int y = abspos.second;
    return QRect(x - 0.5, y - 0.5, 1, 1); // 1x1 collision box
}

bool Hero::isAlive() const {
    return is_alive;
}

void Hero::takeDamage(int damage) {
    setHP(my_HP - damage);
}

void Hero::update() {
    // 更新移动
    updateMovement();
    
    // 更新动画
    updateAnimation();
    
    // Auto-attack logic
    my_weapon->update();
}

int Hero::getCharacterId() const {
    return hero_style;
}

int Hero::getHealth() const {
    return my_HP;
}

int Hero::getMaxHealth() const {
    return HP_MAX;
}

void Hero::setHealth(int value) {
    setHP(value);
}

int Hero::getExperience() const {
    return my_EXP;
}

void Hero::setExperience(int value) {
    my_EXP = value;
    
    // 更新UI
    EXP_bar->setValue(my_EXP);
    EXP_label->setText(QString("经验: %1/%2").arg(my_EXP).arg(EXP_MAX));
    
    // 检查是否升级
    if (my_EXP >= EXP_MAX) {
        levelUp();
    }
}

void Hero::setLevel(int value) {
    level = value;
    
    // 更新升级所需经验
    EXP_MAX = 100 + (level - 1) * 20;
    
    // 更新UI
    level_label->setText(QString("等级: %1").arg(level));
    EXP_bar->setRange(0, EXP_MAX);
    EXP_label->setText(QString("经验: %1/%2").arg(my_EXP).arg(EXP_MAX));
}

void Hero::setPosition(double x, double y) {
    realpos = qMakePair(x, y);
    abspos = qMakePair(static_cast<int>(x), static_cast<int>(y));
}

void Hero::setControlType(int type) {
    // 0表示WASD控制，1表示鼠标控制
    control_type = type;
}

// 绘制角色
void Hero::render(QPainter* painter) {
    if (!painter || !is_alive) return;
    
    // 获取屏幕位置
    int screen_x = abspos.first * 32;  // 假设单元格大小为32x32
    int screen_y = abspos.second * 32;
    
    QPixmap heroSprite;
    
    // 根据状态选择合适的精灵
    if (is_attacking) {
        // 攻击动画
        heroSprite = ResourceManager::getInstance().getHeroAttackImage(hero_style, animation_frame % 3);
    } else if (is_moving) {
        // 行走动画
        heroSprite = ResourceManager::getInstance().getHeroWalkImage(hero_style, animation_frame % 4);
    } else {
        // 静止状态
        heroSprite = ResourceManager::getInstance().getHeroIdleImage(hero_style);
    }
    
    if (!heroSprite.isNull()) {
        // 绘制英雄精灵
        painter->drawPixmap(screen_x - heroSprite.width()/2, screen_y - heroSprite.height()/2, heroSprite);
    } else {
        // 绘制后备显示（简单的彩色矩形）
        painter->setBrush(QColor(0, 200, 0));
        painter->drawEllipse(QPointF(screen_x, screen_y), 16, 16);
    }
}

// 更新角色动画
void Hero::updateAnimation() {
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
}

// 设置攻击状态
void Hero::setAttacking(bool attacking) {
    is_attacking = attacking;
    if (attacking) {
        // 重置动画帧以便从头开始攻击动画
        animation_frame = 0;
        animation_time = 0;
    }
}

// 设置移动状态
void Hero::setMoving(bool moving) {
    is_moving = moving;
}
