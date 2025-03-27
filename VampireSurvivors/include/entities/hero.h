#ifndef HERO_H
#define HERO_H

#include <QObject>
#include <QWidget>
#include <QProgressBar>
#include <QLabel>
#include <QPair>
#include <vector>
#include "../core/weapon.h"
#include "../core/gamemap.h"
#include "../core/gamestate.h"

// Forward declarations
class GameMap;
class Weapon;
class GameState;

// 升级选项类型
enum class UpgradeType {
    CHARACTER,  // 角色属性升级
    WEAPON      // 武器属性升级
};

// 升级选项类
class UpgradeOption {
public:
    UpgradeOption(QString name, QString description, UpgradeType type, int value)
        : name(name), description(description), type(type), value(value) {}
    
    QString getName() const { return name; }
    QString getDescription() const { return description; }
    UpgradeType getType() const { return type; }
    int getValue() const { return value; }
    
private:
    QString name;
    QString description;
    UpgradeType type;
    int value;
};

class Hero : public QObject {
    Q_OBJECT
public:
    Hero();
    Hero(int style, QWidget* parent, GameMap* map_parent, GameState* state);
    virtual ~Hero() override;
    
    // 基本属性
    int getHP() const;
    int getMaxHP() const;
    void setHP(int HP);
    int getEXP() const;
    void addEXP(int exp);
    int getAttack() const;
    void addAttack(int attack);
    int getSpeed() const;
    void addSpeed(float speed);
    int getWeaponType() const;
    int getLevel() const;
    int getPickupRange() const;
    void addPickupRange(int range);
    
    // 升级系统
    void levelUp();
    bool hasLeveledUp() const;
    const std::vector<UpgradeOption*>& getAvailableUpgrades() const;
    void applyUpgrade(int upgradeIndex);
    
    // 移动和位置
    void move(int dx, int dy);
    void updateMovement();
    int getX() const;
    int getY() const;
    QPair<double, double> getRealPos() const;
    
    // 战斗
    void attack();
    QRect getCollisionRect() const;
    bool isAlive() const;
    void takeDamage(int damage);
    
    // 更新
    void update();
    
    // 键盘输入处理
    void processKeyPress(int key);
    void processKeyRelease(int key);
    
    // 全局升级
    void setMaxHealth(int maxHP);
    void setBaseDamage(int damage);
    void setPickupRange(int range);
    
signals:
    void leveledUp(); // 角色升级信号
    
private:
    // 初始化
    void initCharacter();
    void generateUpgradeOptions();
    
    // 基本属性
    int hero_style;
    int HP_MAX;
    int my_HP;
    int EXP_MAX;
    int my_EXP;
    int my_attack;
    float my_speed;
    int weapon_type;
    int level;
    int pickup_range;
    bool is_alive;
    
    // UI元素
    QProgressBar* HP_bar;
    QProgressBar* EXP_bar;
    QLabel* HP_label;
    QLabel* EXP_label;
    QLabel* level_label;
    
    // 游戏对象
    GameMap* game_map;
    GameState* game_state;
    Weapon* my_weapon;
    
    // 位置
    QPair<int, int> abspos;
    QPair<double, double> realpos;
    
    // 升级系统
    std::vector<UpgradeOption*> available_upgrades;
    
    // 键盘输入
    std::vector<int> key_pressed;
};

#endif // HERO_H
