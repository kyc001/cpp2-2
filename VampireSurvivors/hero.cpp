#include "bin/hero.h"
#include "bin/weapon.h"
#include "bin/gamemap.h"
#include "bin/gamestate.h"
#include <QWidget>
#include <QKeyEvent>
#include <QRandomGenerator>

Hero::Hero() : QObject(), is_alive(true), level(1), pickup_range(1) {
    // Default constructor
}

Hero::Hero(int style, QWidget* parent, GameMap* map_parent, GameState* state) 
    : QObject(parent), hero_style(style), game_map(map_parent), game_state(state), 
      is_alive(true), level(1), pickup_range(1) {
    
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
    
    EXP_bar->setRange(0, EXP_MAX);
    EXP_bar->setValue(my_EXP);
    EXP_bar->setGeometry(10, 40, 200, 20);
    
    HP_label->setText(QString("HP: %1/%2").arg(my_HP).arg(HP_MAX));
    HP_label->setGeometry(220, 10, 100, 20);
    
    EXP_label->setText(QString("EXP: %1/%2").arg(my_EXP).arg(EXP_MAX));
    EXP_label->setGeometry(220, 40, 100, 20);
    
    level_label->setText(QString("LV: %1").arg(level));
    level_label->setGeometry(220, 70, 100, 20);
    level_label->setStyleSheet("color: white; font-size: 16px;");
    
    // Set starting position
    abspos = qMakePair(game_map->getWidth() / 2, game_map->getHeight() / 2);
    realpos = qMakePair(abspos.first * 1.0, abspos.second * 1.0);
    
    // Create weapon
    my_weapon = new Weapon(weapon_type, this, game_state);
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
    HP_label->setText(QString("HP: %1/%2").arg(my_HP).arg(HP_MAX));
}

int Hero::getEXP() const {
    return my_EXP;
}

void Hero::addEXP(int exp) {
    my_EXP += exp;
    
    // Update UI
    EXP_bar->setValue(my_EXP);
    EXP_label->setText(QString("EXP: %1/%2").arg(my_EXP).arg(EXP_MAX));
    
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
    EXP_label->setText(QString("EXP: %1/%2").arg(my_EXP).arg(EXP_MAX));
    level_label->setText(QString("LV: %1").arg(level));
    
    // 生成升级选项
    generateUpgradeOptions();
    
    // 发出升级信号
    emit leveledUp();
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
        
        switch (optionType) {
            case 0: // 增加最大生命值
                available_upgrades.push_back(new UpgradeOption(
                    "生命值+20",
                    "增加20点最大生命值",
                    UpgradeType::CHARACTER,
                    optionType
                ));
                break;
            case 1: // 增加攻击力
                available_upgrades.push_back(new UpgradeOption(
                    "攻击力+5",
                    "增加5点攻击力",
                    UpgradeType::CHARACTER,
                    optionType
                ));
                break;
            case 2: // 增加移动速度
                available_upgrades.push_back(new UpgradeOption(
                    "移动速度+0.5",
                    "增加移动速度",
                    UpgradeType::CHARACTER,
                    optionType
                ));
                break;
            case 3: // 增加武器伤害
                available_upgrades.push_back(new UpgradeOption(
                    "武器伤害+10%",
                    "增加武器伤害10%",
                    UpgradeType::WEAPON,
                    optionType
                ));
                break;
            case 4: // 减少武器冷却时间
                available_upgrades.push_back(new UpgradeOption(
                    "武器冷却-10%",
                    "减少武器冷却时间10%",
                    UpgradeType::WEAPON,
                    optionType
                ));
                break;
        }
    }
}

// 应用升级选项
void Hero::applyUpgrade(int upgradeIndex) {
    if (upgradeIndex < 0 || upgradeIndex >= available_upgrades.size()) {
        return;
    }
    
    UpgradeOption* selectedUpgrade = available_upgrades[upgradeIndex];
    
    switch (selectedUpgrade->getValue()) {
        case 0: // 增加最大生命值
            HP_MAX += 20;
            setHP(my_HP + 20); // 同时恢复部分血量
            break;
        case 1: // 增加攻击力
            addAttack(5);
            break;
        case 2: // 增加移动速度
            addSpeed(0.5);
            break;
        case 3: // 增加武器伤害
            if (my_weapon) {
                my_weapon->increaseDamage(0.1); // 增加10%伤害
            }
            break;
        case 4: // 减少武器冷却时间
            if (my_weapon) {
                my_weapon->decreaseCooldown(0.1); // 减少10%冷却时间
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
    key_pressed.remove(key);
}

void Hero::updateMovement() {
    int dx = 0, dy = 0;
    
    // Process movement based on pressed keys
    for (int key : key_pressed) {
        switch (key) {
            case Qt::Key_W: dy -= 1; break;
            case Qt::Key_S: dy += 1; break;
            case Qt::Key_A: dx -= 1; break;
            case Qt::Key_D: dx += 1; break;
        }
    }
    
    // Apply movement if there's a direction
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
    // Update movement
    updateMovement();
    
    // Auto-attack logic
    my_weapon->update();
}
