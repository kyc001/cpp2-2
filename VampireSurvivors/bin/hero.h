#ifndef HERO_H
#define HERO_H

#include <QObject>
#include <QVector>
#include <QLabel>
#include <QProgressBar>
#include <QRect>
#include <QPair>
#include <list>
#include <vector>

// Forward declarations
class GameMap;
class Weapon;
class GameState;
class UpgradeOption;

// 升级选项类型
enum class UpgradeType {
    CHARACTER,  // 角色属性升级
    WEAPON      // 武器属性升级
};

class Hero : public QObject {
    Q_OBJECT

private:
    // Basic attributes
    int HP_MAX;
    int my_HP;
    int EXP_MAX;
    int my_EXP;
    int my_attack;
    int my_speed;
    bool is_alive;
    
    // 等级相关
    int level;
    
    // 升级相关
    std::vector<UpgradeOption*> available_upgrades;
    
    // 拾取范围（掉落物）
    int pickup_range;
    
    // UI elements
    QProgressBar* HP_bar;
    QProgressBar* EXP_bar;
    QLabel* HP_label;
    QLabel* EXP_label;
    QLabel* level_label;
    
    // Position
    QPair<int, int> abspos;      // Grid position
    QPair<double, double> realpos; // Exact position
    
    // Weapon information
    int weapon_type;
    Weapon* my_weapon;
    
    // Key press information
    std::list<int> key_pressed;
    GameMap* game_map;
    GameState* game_state;
    
    // Character type/style
    int hero_style;

public:
    Hero();
    explicit Hero(int hero_style, QWidget* parent, GameMap* map_parent, GameState* state);
    ~Hero() override;
    
    // Getters and setters
    int getHP() const;
    void setHP(int HP);
    int getMaxHP() const;
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
    void setMaxHealth(int maxHP);
    void setBaseDamage(int damage);
    void setPickupRange(int range);
    
    // Movement and actions
    void move(int dx, int dy);
    void attack();
    
    // Position getters
    int getX() const;
    int getY() const;
    QPair<double, double> getRealPos() const;
    
    // Input handling
    void processKeyPress(int key);
    void processKeyRelease(int key);
    void updateMovement();
    
    // Collision detection
    QRect getCollisionRect() const;
    
    // Special methods
    bool isAlive() const;
    void takeDamage(int damage);
    void update();
    
    // 升级相关方法
    void levelUp();
    void generateUpgradeOptions();
    void applyUpgrade(int upgradeIndex);
    bool hasLeveledUp() const;
    const std::vector<UpgradeOption*>& getAvailableUpgrades() const;
    
    // Character specific
    void initCharacter();

signals:
    void leveledUp(); // 发出升级信号
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

#endif // HERO_H
