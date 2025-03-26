#include "bin/hero.h"
#include "bin/weapon.h"
#include "bin/gamemap.h"
#include "bin/gamestate.h"
#include <QWidget>
#include <QKeyEvent>

Hero::Hero() : QObject(), is_alive(true) {
    // Default constructor
}

Hero::Hero(int style, QWidget* parent, GameMap* map_parent, GameState* state) 
    : QObject(parent), hero_style(style), game_map(map_parent), game_state(state), is_alive(true) {
    
    // Initialize character based on style
    initCharacter();
    
    // Create UI elements
    HP_bar = new QProgressBar(parent);
    EXP_bar = new QProgressBar(parent);
    HP_label = new QLabel(parent);
    EXP_label = new QLabel(parent);
    
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
    delete my_weapon;
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
    if (my_EXP >= EXP_MAX) {
        my_EXP = 0;
        // Level up logic would go here
    }
    
    // Update UI
    EXP_bar->setValue(my_EXP);
    EXP_label->setText(QString("EXP: %1/%2").arg(my_EXP).arg(EXP_MAX));
}

int Hero::getAttack() const {
    return my_attack;
}

int Hero::getSpeed() const {
    return my_speed;
}

int Hero::getWeaponType() const {
    return weapon_type;
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
