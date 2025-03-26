#ifndef HERO_H
#define HERO_H

#include <QObject>
#include <QVector>
#include <QLabel>
#include <QProgressBar>
#include <QRect>
#include <QPair>
#include <list>

// Forward declarations
class GameMap;
class Weapon;
class GameState;

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
    
    // UI elements
    QProgressBar* HP_bar;
    QProgressBar* EXP_bar;
    QLabel* HP_label;
    QLabel* EXP_label;
    
    // Position
    QPair<int, int> abspos;      // Grid position
    QPair<double, double> realpos; // Exact position
    
    // Weapon information
    int weapon_type;
    Weapon* my_weapon;
    
    // Key press information
    std::list<int> key_pressed;
    GameState* game_state;
    GameMap* game_map;
    
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
    int getSpeed() const;
    int getWeaponType() const;
    
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
    
    // Character specific
    void initCharacter();
};

#endif // HERO_H
