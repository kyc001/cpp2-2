#ifndef WEAPON_H
#define WEAPON_H

#include <QObject>
#include <QRect>
#include <QTimer>

class Hero;
class Enemy;
class GameState;

class Weapon : public QObject {
    Q_OBJECT
public:
    explicit Weapon(int type, Hero* owner, GameState* state, QObject* parent = nullptr);
    ~Weapon() override;
    
    // Weapon properties
    int getDamage() const;
    int getCooldown() const;
    int getDuration() const;
    int getType() const;
    
    // Weapon actions
    void attack();
    bool canAttack() const;
    void update();
    
    // Get damage area
    QRect getDamageArea() const;
    
    // Check if weapon is active
    bool isActive() const;
    
private:
    int weapon_type;      // Type of weapon
    int weapon_damage;    // Base damage
    int cooldown_time;    // Cooldown between attacks in ms
    int duration_time;    // Duration of attack in ms
    int range;            // Range/radius of damage area
    
    Hero* owner;          // Owner of the weapon
    GameState* game_state;
    
    QTimer* cooldown_timer; // Timer for cooldown
    QTimer* active_timer;   // Timer for active duration
    
    bool is_active;       // Whether weapon is currently active
    bool is_cooling_down; // Whether weapon is cooling down
    
    // Initialize weapon based on type
    void initWeapon();
};

#endif // WEAPON_H 