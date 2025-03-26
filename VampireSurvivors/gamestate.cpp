#include "bin/gamestate.h"
#include "bin/hero.h"
#include "bin/enemy.h"
#include "bin/gamemap.h"
#include <QRandomGenerator>
#include <QWidget>

GameState::GameState(QObject *parent) : QObject(parent), 
    my_hero(nullptr), my_map(nullptr), game_running(false), game_over(false),
    game_time(0), game_score(0), total_kills(0), selected_character(0),
    spawn_rate(3000), max_enemies(50) {
    
    // Initialize screen center
    my_screenCenterX = 400;
    my_screenCenterY = 300;
    
    // Create game timer (updates every 16ms ~ 60fps)
    game_timer = new QTimer(this);
    connect(game_timer, &QTimer::timeout, this, &GameState::update);
    
    // Create spawn timer
    spawn_timer = new QTimer(this);
    connect(spawn_timer, &QTimer::timeout, this, &GameState::spawnEnemies);
}

GameState::~GameState() {
    delete my_hero;
    delete my_map;
    
    // Delete all enemies
    for (Enemy* enemy : my_enemies) {
        delete enemy;
    }
    my_enemies.clear();
    
    // Delete timers
    delete game_timer;
    delete spawn_timer;
}

void GameState::init() {
    // Create game map
    my_map = new GameMap(this);
    
    // Create hero (if not already selected, use default character)
    QWidget* parent_widget = qobject_cast<QWidget*>(parent());
    my_hero = new Hero(selected_character, parent_widget, my_map, this);
    
    // Reset game variables
    game_time = 0;
    game_score = 0;
    total_kills = 0;
    game_running = false;
    game_over = false;
    
    // Clear existing enemies
    for (Enemy* enemy : my_enemies) {
        delete enemy;
    }
    my_enemies.clear();
}

void GameState::start() {
    if (!game_running && !game_over) {
        game_running = true;
        
        // Start the game timer
        game_timer->start(16); // ~60fps
        
        // Start the spawn timer
        spawn_timer->start(spawn_rate);
        
        // Start the elapsed timer
        elapsed_timer.restart();
    }
}

void GameState::pause() {
    if (game_running) {
        game_running = false;
        game_timer->stop();
        spawn_timer->stop();
    }
}

void GameState::resume() {
    if (!game_running && !game_over) {
        game_running = true;
        game_timer->start(16);
        spawn_timer->start(spawn_rate);
    }
}

void GameState::gameOver() {
    game_running = false;
    game_over = true;
    game_timer->stop();
    spawn_timer->stop();
    
    emit gameOverSignal();
}

void GameState::update() {
    if (!game_running || game_over) {
        return;
    }
    
    // Update game time (every second)
    int currentTimeInSeconds = elapsed_timer.elapsed() / 1000;
    if (currentTimeInSeconds > game_time) {
        game_time = currentTimeInSeconds;
        emit timeUpdated(game_time);
    }
    
    // Update hero
    if (my_hero && my_hero->isAlive()) {
        my_hero->update();
    } else {
        gameOver();
        return;
    }
    
    // Update enemies
    for (Enemy* enemy : my_enemies) {
        if (enemy && enemy->isActive()) {
            enemy->update();
        }
    }
    
    // Check collisions
    checkCollisions();
    
    // Clean up dead enemies
    cleanupEnemies();
}

int GameState::getGameTime() const {
    return game_time;
}

int GameState::getScore() const {
    return game_score;
}

Hero* GameState::getHero() {
    return my_hero;
}

GameMap* GameState::getMap() const {
    return my_map;
}

void GameState::addEnemy(Enemy* enemy) {
    if (enemy) {
        my_enemies.append(enemy);
    }
}

void GameState::removeEnemy(Enemy* enemy) {
    my_enemies.removeOne(enemy);
    delete enemy;
}

QVector<Enemy*> GameState::getEnemies() const {
    return my_enemies;
}

void GameState::spawnEnemies() {
    if (!game_running || game_over || my_enemies.size() >= max_enemies) {
        return;
    }
    
    // Increase difficulty over time
    if (game_time > 60 && spawn_rate > 1000) {
        spawn_rate -= 100;
        spawn_timer->setInterval(spawn_rate);
    }
    
    // Determine how many enemies to spawn this round
    int spawn_count = qMin(5, max_enemies - my_enemies.size());
    
    for (int i = 0; i < spawn_count; i++) {
        // Pick a random enemy type
        int enemy_type = QRandomGenerator::global()->bounded(4); // 0-3
        
        // Pick a random spawn position at the edge of the map
        int x, y;
        int edge = QRandomGenerator::global()->bounded(4); // 0-3 for top, right, bottom, left
        
        switch (edge) {
            case 0: // Top
                x = QRandomGenerator::global()->bounded(my_map->getWidth());
                y = 0;
                break;
            case 1: // Right
                x = my_map->getWidth() - 1;
                y = QRandomGenerator::global()->bounded(my_map->getHeight());
                break;
            case 2: // Bottom
                x = QRandomGenerator::global()->bounded(my_map->getWidth());
                y = my_map->getHeight() - 1;
                break;
            case 3: // Left
                x = 0;
                y = QRandomGenerator::global()->bounded(my_map->getHeight());
                break;
        }
        
        // Create and add the enemy
        Enemy* enemy = new Enemy(enemy_type, x, y, my_map, this);
        addEnemy(enemy);
    }
}

bool GameState::isRunning() const {
    return game_running;
}

bool GameState::isGameOver() const {
    return game_over;
}

void GameState::selectCharacter(int character_type) {
    selected_character = character_type;
}

void GameState::cleanupEnemies() {
    QVector<Enemy*> dead_enemies;
    
    // Collect dead enemies
    for (Enemy* enemy : my_enemies) {
        if (enemy && !enemy->isAlive()) {
            dead_enemies.append(enemy);
            game_score += 10;
            total_kills++;
        }
    }
    
    // Remove and delete dead enemies
    for (Enemy* enemy : dead_enemies) {
        my_enemies.removeOne(enemy);
        delete enemy;
    }
    
    if (!dead_enemies.isEmpty()) {
        emit scoreUpdated(game_score);
    }
}

void GameState::checkCollisions() {
    if (!my_hero || !my_hero->isAlive()) {
        return;
    }
    
    // Get hero weapon damage area
    QRect heroCollisionRect = my_hero->getCollisionRect();
    
    // Check for hero-enemy collisions
    for (Enemy* enemy : my_enemies) {
        if (enemy && enemy->isActive()) {
            QRect enemyCollisionRect = enemy->getCollisionRect();
            
            // Check if hero and enemy collide
            if (heroCollisionRect.intersects(enemyCollisionRect)) {
                // Enemy can damage hero
                // (Enemy attack logic is handled in enemy update)
            }
        }
    }
}
