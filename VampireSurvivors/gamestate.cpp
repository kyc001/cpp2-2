#include "bin/gamestate.h"
#include "bin/hero.h"
#include "bin/enemy.h"
#include "bin/gamemap.h"
#include "bin/drop.h"
#include "bin/savefile.h"
#include <QRandomGenerator>
#include <QWidget>
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>

GameState::GameState(QObject *parent) : QObject(parent), 
    my_hero(nullptr), my_map(nullptr), game_running(false), game_over(false),
    game_time(0), game_score(0), total_kills(0), selected_character(0),
    game_coins(0), total_coins(0), coin_multiplier(1.0), save_file(nullptr),
    spawn_rate(3000), max_enemies(50) {
    
    // Initialize screen center
    my_screenCenterX = 400;
    my_screenCenterY = 300;
    
    // 初始化全局升级等级
    global_upgrades.resize(5, 0); // 5种全局升级，初始等级为0
    
    // 初始化角色解锁状态
    unlocked_characters.resize(5, false); // 预留5个角色
    unlocked_characters[0] = true; // 默认解锁第一个角色
    
    // 创建存档对象
    save_file = new SaveFile();
    
    // Create game timer (updates every 16ms ~ 60fps)
    game_timer = new QTimer(this);
    connect(game_timer, &QTimer::timeout, this, &GameState::update);
    
    // Create spawn timer
    spawn_timer = new QTimer(this);
    connect(spawn_timer, &QTimer::timeout, this, &GameState::spawnEnemies);
    
    // 加载游戏存档
    loadGame();
}

GameState::~GameState() {
    delete my_hero;
    delete my_map;
    
    // Delete all enemies
    for (Enemy* enemy : my_enemies) {
        delete enemy;
    }
    my_enemies.clear();
    
    // 删除所有掉落物
    for (Drop* drop : my_drops) {
        delete drop;
    }
    my_drops.clear();
    
    // 删除存档对象
    delete save_file;
    
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
    
    // 应用全局升级
    applyGlobalUpgradesToGame();
    
    // 连接英雄升级信号
    connect(my_hero, &Hero::leveledUp, this, &GameState::onHeroLeveledUp);
    
    // Reset game variables
    game_time = 0;
    game_score = 0;
    game_coins = 0;
    total_kills = 0;
    game_running = false;
    game_over = false;
    
    // Clear existing enemies
    for (Enemy* enemy : my_enemies) {
        delete enemy;
    }
    my_enemies.clear();
    
    // 清理掉落物
    for (Drop* drop : my_drops) {
        delete drop;
    }
    my_drops.clear();
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
    
    // 游戏结束时保存金币
    total_coins += game_coins;
    saveGame();
    
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
    
    // Check drop pickup
    checkDropPickup();
    
    // Clean up dead enemies
    cleanupEnemies();
    
    // Clean up inactive drops
    cleanupDrops();
}

int GameState::getGameTime() const {
    return game_time;
}

int GameState::getScore() const {
    return game_score;
}

void GameState::addCoins(int amount) {
    // 应用金币获取倍率
    amount = static_cast<int>(amount * coin_multiplier);
    game_coins += amount;
    emit coinsUpdated(game_coins);
}

int GameState::getCoins() const {
    return game_coins;
}

int GameState::getTotalCoins() const {
    return total_coins;
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

void GameState::addDrop(Drop* drop) {
    if (drop) {
        my_drops.append(drop);
    }
}

void GameState::removeDrop(Drop* drop) {
    my_drops.removeOne(drop);
    delete drop;
}

QVector<Drop*> GameState::getDrops() const {
    return my_drops;
}

void GameState::spawnDrop(int x, int y) {
    // 生成随机掉落物
    int dropRand = QRandomGenerator::global()->bounded(100);
    
    if (dropRand < 15) { // 15% 概率掉落生命值
        Drop* drop = new Drop(DropType::HEALTH, 20, x, y, this);
        addDrop(drop);
    } else if (dropRand < 40) { // 25% 概率掉落经验值
        Drop* drop = new Drop(DropType::EXPERIENCE, 10, x, y, this);
        addDrop(drop);
    } else if (dropRand < 70) { // 30% 概率掉落金币
        Drop* drop = new Drop(DropType::COIN, 1, x, y, this);
        addDrop(drop);
    } else if (dropRand < 80) { // 10% 概率掉落临时增强
        Drop* drop = new Drop(DropType::POWER_UP, 5, x, y, this);
        addDrop(drop);
    }
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

void GameState::applyGlobalUpgrade(int type, int level) {
    if (type >= 0 && type < global_upgrades.size()) {
        global_upgrades[type] = level;
        
        // 应用升级效果
        if (my_hero) {
            switch (type) {
                case 0: // 最大生命值
                    my_hero->setMaxHealth(100 + level * 20);
                    break;
                case 1: // 基础攻击力
                    my_hero->setBaseDamage(5 + level * 2);
                    break;
                // 其他升级类型可以在此添加...
            }
        }
        
        // 保存游戏
        saveGame();
    }
}

void GameState::applyGlobalUpgradesToGame() {
    // 将全局升级效果应用到游戏中
    if (my_hero) {
        // 最大生命值
        int max_health_level = global_upgrades[0];
        my_hero->setMaxHealth(100 + max_health_level * 20);
        
        // 基础攻击力
        int attack_level = global_upgrades[1];
        my_hero->setBaseDamage(5 + attack_level * 2);
        
        // 拾取范围 (第4个升级)
        if (global_upgrades.size() > 3) {
            int pickup_range_level = global_upgrades[3];
            my_hero->setPickupRange(30 + pickup_range_level * 10);
        }
        
        // 金币获取倍率 (第3个升级)
        if (global_upgrades.size() > 2) {
            int coin_level = global_upgrades[2];
            coin_multiplier = 1.0f + (coin_level * 0.1f);
        }
    }
}

int GameState::getUpgradeLevel(int type) const {
    if (type >= 0 && type < global_upgrades.size()) {
        return global_upgrades[type];
    }
    return 0;
}

bool GameState::saveGame() {
    if (!save_file) {
        return false;
    }
    
    // 保存总金币
    save_file->setTotalCoins(total_coins);
    
    // 保存全局升级等级
    save_file->setGlobalUpgrades(global_upgrades);
    
    // 保存角色解锁状态
    save_file->setUnlockedCharacters(unlocked_characters);
    
    // 保存到文件
    QString saveDir = QDir::homePath() + "/.vampiresurvivors";
    QDir().mkpath(saveDir);
    
    return save_file->saveToFile(saveDir + "/save.dat");
}

bool GameState::loadGame() {
    if (!save_file) {
        return false;
    }
    
    // 尝试从文件加载
    QString saveDir = QDir::homePath() + "/.vampiresurvivors";
    bool result = save_file->loadFromFile(saveDir + "/save.dat");
    
    if (result) {
        // 读取总金币
        total_coins = save_file->getTotalCoins();
        
        // 读取全局升级等级
        global_upgrades = save_file->getGlobalUpgrades();
        
        // 读取角色解锁状态
        unlocked_characters = save_file->getUnlockedCharacters();
        
        // 确保数组大小一致
        if (global_upgrades.size() < 5) {
            global_upgrades.resize(5, 0);
        }
        
        if (unlocked_characters.size() < 5) {
            unlocked_characters.resize(5, false);
            unlocked_characters[0] = true; // 确保第一个角色解锁
        }
        
        // 应用全局升级
        if (my_hero) {
            applyGlobalUpgradesToGame();
        }
        
        return true;
    }
    
    return false;
}

bool GameState::isCharacterUnlocked(int character_id) const {
    if (character_id >= 0 && character_id < unlocked_characters.size()) {
        return unlocked_characters[character_id];
    }
    return false;
}

void GameState::unlockCharacter(int character_id) {
    if (character_id >= 0 && character_id < unlocked_characters.size()) {
        unlocked_characters[character_id] = true;
        saveGame();
    }
}

void GameState::onHeroLeveledUp() {
    // 发送信号通知UI显示升级选项
    emit levelUpOptionsReady();
}

void GameState::cleanupEnemies() {
    QVector<Enemy*> dead_enemies;
    
    // Collect dead enemies
    for (Enemy* enemy : my_enemies) {
        if (enemy && !enemy->isAlive()) {
            dead_enemies.append(enemy);
            game_score += 10;
            total_kills++;
            
            // 概率生成掉落物
            spawnDrop(enemy->getX(), enemy->getY());
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

void GameState::cleanupDrops() {
    QVector<Drop*> inactive_drops;
    
    // 收集非活跃掉落物
    for (Drop* drop : my_drops) {
        if (drop && !drop->isActive()) {
            inactive_drops.append(drop);
        }
    }
    
    // 移除并删除非活跃掉落物
    for (Drop* drop : inactive_drops) {
        my_drops.removeOne(drop);
        delete drop;
    }
}

void GameState::checkDropPickup() {
    if (!my_hero || !my_hero->isAlive()) {
        return;
    }
    
    int heroX = my_hero->getX();
    int heroY = my_hero->getY();
    int pickupRange = my_hero->getPickupRange();
    
    for (Drop* drop : my_drops) {
        if (drop && drop->isActive()) {
            int dropX = drop->getX();
            int dropY = drop->getY();
            
            // 检查是否在拾取范围内
            if (abs(heroX - dropX) <= pickupRange && abs(heroY - dropY) <= pickupRange) {
                // 应用掉落物效果
                drop->applyEffect(my_hero);
            }
        }
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

int GameState::getSelectedCharacter() const {
    return selected_character;
}

void GameState::setTotalCoins(int amount) {
    total_coins = amount;
}
