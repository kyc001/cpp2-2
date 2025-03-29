#ifndef ENEMY_H
#define ENEMY_H

#include <QObject>
#include <QRect>

class Hero;
class GameMap;
class GameState;
class QPainter;

enum class EnemyMovement
{
    FOLLOW_PLAYER,  // Move towards player
    FIXED_DIRECTION // Move in a fixed direction, change on collision
};

enum class EnemyType
{
    MELEE, // Close range attack
    RANGED // Range attack
};

class Enemy : public QObject
{
    Q_OBJECT
    // 在 Enemy 类的 public: 部分添加
public:
    int getMaxHP() const { return maxHP; }

private:
    int maxHP; // 确保这个成员变量存在
public:
    Enemy(int type, int x, int y, GameMap *map, GameState *state, QObject *parent = nullptr);
    virtual ~Enemy() override;

    // Position getters
    int getX() const;
    int getY() const;

    // Health and attack getters/setters
    int getHP() const;
    void setHP(int HP);
    int getAttack() const;

    // Movement and actions
    void move();
    void attack(Hero *hero);

    // Update method
    void update();

    // State queries
    bool isAlive() const;
    bool isActive() const;

    // Collision detection
    QRect getCollisionRect() const;

    // Enemy specific properties
    int getType() const;
    EnemyType getEnemyType() const;
    EnemyMovement getMovementType() const;

    // Damage methods
    void takeDamage(int damage);

    // 渲染和动画
    void render(QPainter *painter);
    void updateAnimation();

private:
    int enemy_type;
    int my_x;
    int my_y;
    double real_x;
    double real_y;
    int my_HP;
    int my_attack;
    double my_speed;
    int attack_range;
    int attack_cooldown;
    int current_cooldown;

    bool is_alive;
    bool is_active;

    EnemyType enemy_attack_type;
    EnemyMovement movement_type;

    // Direction for fixed movement
    int direction_x;
    int direction_y;

    GameMap *game_map;
    GameState *game_state;

    // 动画相关
    int animation_frame; // 当前动画帧
    int animation_time;  // 动画计时器
    bool is_attacking;   // 是否正在攻击
    bool is_moving;      // 是否正在移动

    // Initialize enemy based on type
    void initEnemy();

    // Follow player movement
    void followPlayer();

    // Fixed direction movement
    void moveFixedDirection();
};

#endif // ENEMY_H
