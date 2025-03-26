#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <QObject>
#include <QTimer>
#include <QVector>
#include <QElapsedTimer>

class Hero;
class GameMap;
class Enemy;
class Weapon;

class GameState : public QObject {
    Q_OBJECT

public:
    explicit GameState(QObject *parent = nullptr);
    ~GameState() override;

    // Game initialization and state
    void init();
    void start();
    void pause();
    void resume();
    void gameOver();
    
    // Game update loop
    void update();
    
    // Game time
    int getGameTime() const;
    
    // Game score
    int getScore() const;
    
    // Hero access
    Hero* getHero();
    
    // Map access
    GameMap* getMap() const;
    
    // Enemy management
    void addEnemy(Enemy* enemy);
    void removeEnemy(Enemy* enemy);
    QVector<Enemy*> getEnemies() const;
    
    // Spawn enemies
    void spawnEnemies();
    
    // Check game state
    bool isRunning() const;
    bool isGameOver() const;
    
    // Select character
    void selectCharacter(int character_type);
    
signals:
    void gameOverSignal();
    void scoreUpdated(int score);
    void timeUpdated(int time);

private:
    // Game objects
    Hero* my_hero;
    GameMap* my_map;
    QVector<Enemy*> my_enemies;
    
    // Game state
    bool game_running;
    bool game_over;
    int game_time;      // Game time in seconds
    int game_score;     // Player score
    int total_kills;    // Total enemies killed
    int selected_character; // Selected character type
    
    // Timers
    QTimer* game_timer;     // Main game timer
    QTimer* spawn_timer;    // Enemy spawn timer
    QElapsedTimer elapsed_timer;    // Time tracking
    
    // Enemy spawn parameters
    int spawn_rate;         // Base spawn rate
    int max_enemies;        // Maximum enemies
    
    // Screen center (for rendering)
    int my_screenCenterX;
    int my_screenCenterY;
    
    // Clean up dead enemies
    void cleanupEnemies();
    
    // Check collisions
    void checkCollisions();
};

#endif // GAMESTATE_H
