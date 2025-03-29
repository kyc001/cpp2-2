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
#include <QTimer>

Hero::Hero() : QObject(), level(1), pickup_range(1), is_alive(true), control_type(0)
{
    // 默认构造函数，实际不使用
}

Hero::Hero(int style, QWidget *parent, GameMap *map_parent, GameState *state)
    : QObject(parent),
      hero_style(style),
      HP_MAX(100),
      my_HP(100),
      EXP_MAX(100),
      my_EXP(0),
      my_attack(10),
      my_speed(5.0f),
      weapon_type(style), // 武器类型与角色类型对应
      level(1),
      pickup_range(100),
      is_alive(true),
      control_type(0),
      game_map(map_parent),
      game_state(state),
      my_weapon(nullptr),
      animation_frame(0),
      animation_time(0),
      is_attacking(false),
      is_moving(false)
{
    // 初始化角色位置（地图中心）
    if (game_map)
    {
        int map_width = game_map->getWidth();
        int map_height = game_map->getHeight();
        abspos = qMakePair(map_width / 2, map_height / 2);
        realpos = qMakePair(abspos.first * 1.0, abspos.second * 1.0);
    }
    else
    {
        abspos = qMakePair(0, 0);
        realpos = qMakePair(0.0, 0.0);
    }

    // 根据角色风格初始化属性
    initCharacter();

    // 创建武器
    my_weapon = new Weapon(weapon_type, this, game_state);
}

Hero::~Hero()
{
    if (my_weapon)
    {
        delete my_weapon;
        my_weapon = nullptr;
    }
}

void Hero::initCharacter()
{
    // 根据角色类型设置不同的初始属性
    switch (hero_style)
    {
    case 0: // 战士
        HP_MAX = 120;
        my_HP = HP_MAX;
        my_attack = 15;
        my_speed = 4.5f;
        break;

    case 1: // 法师
        HP_MAX = 80;
        my_HP = HP_MAX;
        my_attack = 20;
        my_speed = 4.0f;
        break;

    case 2: // 弓箭手
        HP_MAX = 90;
        my_HP = HP_MAX;
        my_attack = 12;
        my_speed = 5.0f;
        break;

    case 3: // 盗贼
        HP_MAX = 100;
        my_HP = HP_MAX;
        my_attack = 10;
        my_speed = 6.0f;
        break;

    default:
        // 默认战士属性
        HP_MAX = 120;
        my_HP = HP_MAX;
        my_attack = 15;
        my_speed = 4.5f;
        break;
    }

    // 武器类型与角色类型对应
    weapon_type = hero_style;
}

int Hero::getHP() const
{
    return my_HP;
}

int Hero::getMaxHP() const
{
    return HP_MAX;
}

void Hero::setHP(int HP)
{
    my_HP = qBound(0, HP, HP_MAX);

    // 检查是否死亡
    if (my_HP <= 0)
    {
        is_alive = false;
    }
}

int Hero::getEXP() const
{
    return my_EXP;
}

void Hero::addEXP(int exp)
{
    my_EXP += exp;

    // 检查是否升级
    if (my_EXP >= EXP_MAX)
    {
        levelUp();
    }
}

int Hero::getAttack() const
{
    return my_attack;
}

void Hero::addAttack(int attack)
{
    my_attack += attack;
}

int Hero::getSpeed() const
{
    return static_cast<int>(my_speed);
}

void Hero::addSpeed(float speed)
{
    my_speed += speed;
}

int Hero::getWeaponType() const
{
    return weapon_type;
}

int Hero::getLevel() const
{
    return level;
}

int Hero::getPickupRange() const
{
    return pickup_range;
}

void Hero::addPickupRange(int range)
{
    pickup_range += range;
}

void Hero::levelUp()
{
    // 升级
    level++;

    // 设置新的经验值和上限
    my_EXP = my_EXP - EXP_MAX;
    EXP_MAX = 100 + level * 20; // 每级增加20点经验上限

    // 生成升级选项
    generateUpgradeOptions();

    // 发送升级信号
    emit leveledUp();
}

bool Hero::hasLeveledUp() const
{
    return !available_upgrades.empty();
}

const std::vector<UpgradeOption *> &Hero::getAvailableUpgrades() const
{
    return available_upgrades;
}

void Hero::applyUpgrade(int upgradeIndex)
{
    if (upgradeIndex < 0 || static_cast<size_t>(upgradeIndex) >= available_upgrades.size())
    {
        {
            return;
        }

        // 获取所选升级选项
        UpgradeOption *upgrade = available_upgrades[upgradeIndex];

        // 应用升级效果
        switch (upgrade->getType())
        {
        case UpgradeType::HEALTH:
            HP_MAX += upgrade->getValue();
            my_HP += upgrade->getValue();
            break;

        case UpgradeType::ATTACK:
            my_attack += upgrade->getValue();
            break;

        case UpgradeType::SPEED:
            my_speed += upgrade->getValue() / 10.0f; // 转换为小数增量
            break;

        case UpgradeType::PICKUP_RANGE:
            pickup_range += upgrade->getValue();
            break;

        case UpgradeType::WEAPON_LEVEL:
            if (my_weapon)
            {
                my_weapon->levelUp(upgrade->getValue());
            }
            break;
        }

        // 清理升级选项
        for (auto option : available_upgrades)
        {
            delete option;
        }
        available_upgrades.clear();
    }
}

void Hero::move(int dx, int dy)
{
    if (!is_alive)
    {
        return;
    }

    double newX = realpos.first + dx * my_speed;
    double newY = realpos.second + dy * my_speed;

    // 检查新位置是否有障碍物
    int new_grid_x = static_cast<int>(newX);
    int new_grid_y = static_cast<int>(newY);

    if (game_map && !game_map->isBarrier(new_grid_x, new_grid_y))
    {
        realpos.first = newX;
        realpos.second = newY;

        // 更新整数位置
        abspos.first = static_cast<int>(realpos.first);
        abspos.second = static_cast<int>(realpos.second);
    }
}

void Hero::updateMovement()
{
    if (!is_alive)
    {
        return;
    }

    // 根据当前按下的键移动
    int dx = 0;
    int dy = 0;

    // 处理按下的键
    for (int key : key_pressed)
    {
        switch (key)
        {
        case Qt::Key_W:
        case Qt::Key_Up:
            dy -= 1;
            break;

        case Qt::Key_S:
        case Qt::Key_Down:
            dy += 1;
            break;

        case Qt::Key_A:
        case Qt::Key_Left:
            dx -= 1;
            break;

        case Qt::Key_D:
        case Qt::Key_Right:
            dx += 1;
            break;
        }
    }

    // 对斜向移动进行归一化
    if (dx != 0 && dy != 0)
    {
        double length = sqrt(dx * dx + dy * dy);
        dx = static_cast<int>(dx / length);
        dy = static_cast<int>(dy / length);
    }

    // 如果有移动输入，更新移动状态
    if (dx != 0 || dy != 0)
    {
        is_moving = true;
    }
    else
    {
        is_moving = false;
    }

    // 移动角色
    move(dx, dy);
}

int Hero::getX() const
{
    return abspos.first;
}

int Hero::getY() const
{
    return abspos.second;
}

QPair<double, double> Hero::getRealPos() const
{
    return realpos;
}

void Hero::attack()
{
    if (!is_alive || !my_weapon)
    {
        return;
    }

    // 尝试使用武器攻击
    if (my_weapon->canAttack())
    {
        is_attacking = true;
        my_weapon->attack();
    }
}

QRect Hero::getCollisionRect() const
{
    int size = 40; // 角色碰撞箱大小
    return QRect(abspos.first - size / 2, abspos.second - size / 2, size, size);
}

bool Hero::isAlive() const
{
    return is_alive;
}

void Hero::takeDamage(int damage)
{
    if (!is_alive)
    {
        return;
    }

    // 应用伤害
    setHP(my_HP - damage);
}

void Hero::update()
{
    if (!is_alive)
    {
        return;
    }

    // 更新移动
    updateMovement();

    // 更新武器
    if (my_weapon)
    {
        my_weapon->update();
    }

    // 自动攻击（类似吸血鬼幸存者）
    attack();

    // 更新动画
    updateAnimation();
}

void Hero::processKeyPress(int key)
{
    // 添加按键
    if (!key_pressed.empty() && std::find(key_pressed.begin(), key_pressed.end(), key) == key_pressed.end())
    {
        key_pressed.push_back(key);
    }
    else if (key_pressed.empty())
    {
        key_pressed.push_back(key);
    }
}

void Hero::processKeyRelease(int key)
{
    // 移除按键
    auto it = std::find(key_pressed.begin(), key_pressed.end(), key);
    if (it != key_pressed.end())
    {
        key_pressed.erase(it);
    }
}

void Hero::setMaxHealth(int maxHP)
{
    HP_MAX = maxHP;
    if (my_HP > HP_MAX)
    {
        my_HP = HP_MAX;
    }
}

void Hero::setBaseDamage(int damage)
{
    my_attack = damage;
}

void Hero::setPickupRange(int range)
{
    pickup_range = range;
}

int Hero::getCharacterId() const
{
    return hero_style;
}

int Hero::getHealth() const
{
    return my_HP;
}

int Hero::getMaxHealth() const
{
    return HP_MAX;
}

void Hero::setHealth(int value)
{
    setHP(value);
}

int Hero::getExperience() const
{
    return my_EXP;
}

void Hero::setExperience(int value)
{
    my_EXP = value;
    if (my_EXP >= EXP_MAX)
    {
        levelUp();
    }
}

void Hero::setLevel(int value)
{
    level = value;
    EXP_MAX = 100 + level * 20; // 更新经验上限
}

void Hero::setPosition(double x, double y)
{
    realpos.first = x;
    realpos.second = y;
    abspos.first = static_cast<int>(x);
    abspos.second = static_cast<int>(y);
}

void Hero::setControlType(int type)
{
    control_type = type;
}

void Hero::render(QPainter *painter)
{
    if (!painter || !is_alive)
    {
        return;
    }

    // 根据角色类型和状态选择适当的精灵
    QString spritePath;

    // 角色类型名称
    QStringList characterTypes = {"warrior", "mage", "archer", "rogue"};
    QString typeName = characterTypes[qBound(0, hero_style, 3)];

    // 根据动画状态选择精灵
    if (is_attacking)
    {
        spritePath = QString(":/resources/heroes/%1/%1_attack_%2.png")
                         .arg(typeName)
                         .arg(animation_frame % 3); // 攻击动画有3帧
    }
    else if (is_moving)
    {
        spritePath = QString(":/resources/heroes/%1/%1_walk_%2.png")
                         .arg(typeName)
                         .arg(animation_frame % 4); // 行走动画有4帧
    }
    else
    {
        spritePath = QString(":/resources/heroes/%1/%1_idle.png").arg(typeName);
    }

    // 加载并绘制精灵
    QPixmap sprite(spritePath);
    if (!sprite.isNull())
    {
        int spriteWidth = 64;
        int spriteHeight = 64;

        // 在屏幕中央绘制玩家，周围的世界相对移动
        int drawX = painter->device()->width() / 2 - spriteWidth / 2;
        int drawY = painter->device()->height() / 2 - spriteHeight / 2;

        painter->drawPixmap(drawX, drawY, spriteWidth, spriteHeight, sprite);
    }
}

void Hero::updateAnimation()
{
    animation_time++;

    // 更新攻击状态
    if (is_attacking && animation_time % 10 == 0)
    { // 每10帧更新一次
        animation_frame = (animation_frame + 1) % 3;
        if (animation_frame == 0)
        {
            is_attacking = false; // 攻击动画结束
        }
    }

    // 更新移动动画
    if (is_moving && animation_time % 15 == 0)
    { // 每15帧更新一次
        animation_frame = (animation_frame + 1) % 4;
    }
}

void Hero::setAttacking(bool attacking)
{
    is_attacking = attacking;
    if (attacking)
    {
        animation_frame = 0; // 重置动画帧
    }
}

void Hero::setMoving(bool moving)
{
    is_moving = moving;
    if (!moving)
    {
        animation_frame = 0; // 重置动画帧
    }
}

void Hero::generateUpgradeOptions()
{
    // 清理旧的升级选项
    for (auto option : available_upgrades)
    {
        delete option;
    }
    available_upgrades.clear();

    // 升级选项数量
    const int NUM_OPTIONS = 3;

    // 可能的升级选项列表
    std::vector<UpgradeOption *> possibleUpgrades;

    // 添加健康选项
    possibleUpgrades.push_back(new UpgradeOption(
        UpgradeType::HEALTH,
        10 + level * 2,
        QString("增加最大生命值: +%1").arg(10 + level * 2)));

    // 添加攻击选项
    possibleUpgrades.push_back(new UpgradeOption(
        UpgradeType::ATTACK,
        2 + level,
        QString("增加攻击力: +%1").arg(2 + level)));

    // 添加速度选项
    possibleUpgrades.push_back(new UpgradeOption(
        UpgradeType::SPEED,
        5,
        QString("增加移动速度: +%1%").arg(5)));

    // 添加拾取范围选项
    possibleUpgrades.push_back(new UpgradeOption(
        UpgradeType::PICKUP_RANGE,
        20,
        QString("增加拾取范围: +%1").arg(20)));

    // 添加武器等级选项
    possibleUpgrades.push_back(new UpgradeOption(
        UpgradeType::WEAPON_LEVEL,
        1,
        QString("提升武器等级: +%1").arg(1)));

    // 随机选择NUM_OPTIONS个选项
    while (available_upgrades.size() < NUM_OPTIONS && !possibleUpgrades.empty())
    {
        int index = QRandomGenerator::global()->bounded(possibleUpgrades.size());
        available_upgrades.push_back(possibleUpgrades[index]);
        possibleUpgrades.erase(possibleUpgrades.begin() + index);
    }
}

// 生命值恢复方法
void Hero::heal(int amount)
{
    if (!is_alive)
        return;

    my_HP += amount;
    if (my_HP > HP_MAX)
    {
        my_HP = HP_MAX;
    }

    // 更新UI
    if (HP_bar)
    {
        HP_bar->setValue(my_HP);
    }

    if (HP_label)
    {
        HP_label->setText(QString("%1/%2").arg(my_HP).arg(HP_MAX));
    }
}

// 添加经验值方法
void Hero::addExperience(int amount)
{
    if (!is_alive)
        return;

    my_EXP += amount;

    // 更新UI
    if (EXP_bar)
    {
        EXP_bar->setValue(my_EXP);
    }

    if (EXP_label)
    {
        EXP_label->setText(QString("%1/%2").arg(my_EXP).arg(EXP_MAX));
    }

    // 检查是否升级
    if (my_EXP >= EXP_MAX)
    {
        my_EXP -= EXP_MAX;
        levelUp();
    }
}

// 临时能力提升方法
void Hero::applyPowerUp(int amount)
{
    if (!is_alive)
        return;

    // 临时提升攻击力（持续10秒）
    addAttack(amount);

    // 创建一个定时器在10秒后恢复原始攻击力
    QTimer *powerupTimer = new QTimer(this);
    powerupTimer->setSingleShot(true);
    connect(powerupTimer, &QTimer::timeout, [this, amount]()
            {
                addAttack(-amount); // 减去之前增加的攻击力
            });
    powerupTimer->start(10000); // 10秒后结束
}
