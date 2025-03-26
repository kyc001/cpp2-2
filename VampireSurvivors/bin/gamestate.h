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
class Drop;
class SaveFile;

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
    
    // 金币系统
    void addCoins(int amount);
    int getCoins() const;
    int getTotalCoins() const;
    void setTotalCoins(int amount);
    
    // Hero access
    Hero* getHero();
    
    // Map access
    GameMap* getMap() const;
    
    // Enemy management
    void addEnemy(Enemy* enemy);
    void removeEnemy(Enemy* enemy);
    QVector<Enemy*> getEnemies() const;
    
    // 掉落物管理
    void addDrop(Drop* drop);
    void removeDrop(Drop* drop);
    QVector<Drop*> getDrops() const;
    void spawnDrop(int x, int y);
    
    // Spawn enemies
    void spawnEnemies();
    
    // Check game state
    bool isRunning() const;
    bool isGameOver() const;
    
    // Select character
    void selectCharacter(int character_type);
    int getSelectedCharacter() const;
    
    // 全局强化系统
    void applyGlobalUpgrade(int type, int level);
    int getUpgradeLevel(int type) const;
    
    // 存档系统
    bool saveGame();
    bool loadGame();
    
    // 解锁角色
    bool isCharacterUnlocked(int character_id) const;
    void unlockCharacter(int character_id);
    
signals:
    void gameOverSignal();
    void scoreUpdated(int score);
    void timeUpdated(int time);
    void coinsUpdated(int coins);
    void levelUpOptionsReady();
    
public slots:
    void onHeroLeveledUp();

private:
    // Game objects
    Hero* my_hero;
    GameMap* my_map;
    QVector<Enemy*> my_enemies;
    QVector<Drop*> my_drops;
    
    // Game state
    bool game_running;
    bool game_over;
    int game_time;      // Game time in seconds
    int game_score;     // Player score
    int total_kills;    // Total enemies killed
    int selected_character; // Selected character type
    
    // 金币系统
    int game_coins;     // 本局获得金币
    int total_coins;    // 总金币（跨游戏）
    float coin_multiplier; // 金币获取倍率
    
    // 全局强化系统
    QVector<int> global_upgrades;
    
    // 角色解锁状态
    QVector<bool> unlocked_characters;
    
    // 存档
    SaveFile* save_file;
    
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
    
    // Clean up dead enemies and handle drops
    void cleanupEnemies();
    void cleanupDrops();
    
    // Check collisions
    void checkCollisions();
    void checkDropPickup();
    
    // 将全局升级应用到游戏中
    void applyGlobalUpgradesToGame();
};

#endif // GAMESTATE_H
