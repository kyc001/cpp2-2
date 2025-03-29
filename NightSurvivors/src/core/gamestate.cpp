#include "../../include/core/gamestate.h"
#include "../../include/entities/hero.h"
#include "../../include/entities/enemy.h"
#include "../../include/entities/drop.h"
#include "../../include/core/weapon.h"
#include "../../include/core/gamemap.h"
#include "../../include/utils/savefile.h"
#include <QRandomGenerator>
#include <QWidget>
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QDateTime>
#include <QSettings>
#include <QStandardPaths>

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

int GameState::getGlobalUpgradeLevel(int type) const {
    if (type >= 0 && type < global_upgrades.size()) {
        return global_upgrades[type];
    }
    return 0;
}

void GameState::setUpgradeLevel(int type, int level) {
    if (type >= 0 && type < global_upgrades.size()) {
        global_upgrades[type] = level;
    }
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
    QString saveDir = QDir::homePath() + "/.nightsurvivors";
    QDir().mkpath(saveDir);
    
    return save_file->saveToFile(saveDir + "/save.dat");
}

bool GameState::loadGame() {
    if (!save_file) {
        return false;
    }
    
    // 尝试从文件加载
    QString saveDir = QDir::homePath() + "/.nightsurvivors";
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

bool GameState::saveGameToSlot(int slot)
{
    // 获取当前游戏状态信息
    int current_score = getScore();
    int current_level = 1;
    QString character_name = "未知";
    
    if (my_hero) {
        current_level = my_hero->getLevel();
        switch (my_hero->getCharacterId()) {
            case 0: character_name = "战士"; break;
            case 1: character_name = "法师"; break;
            case 2: character_name = "弓箭手"; break;
            case 3: character_name = "盗贼"; break;
            default: character_name = "未知";
        }
    }
    
    // 保存到指定槽位
    QSettings settings("NightSurvivors", "Saves");
    QString save_key = QString("save_%1").arg(slot);
    
    settings.setValue(save_key + "/timestamp", QDateTime::currentDateTime());
    settings.setValue(save_key + "/score", current_score);
    settings.setValue(save_key + "/level", current_level);
    settings.setValue(save_key + "/character", character_name);
    settings.setValue(save_key + "/totalCoins", total_coins);
    
    // 保存详细游戏状态到文件
    return saveGameData(getSaveSlotFilename(slot));
}

bool GameState::loadGameFromSlot(int slot)
{
    // 从设置检查槽位是否存在
    QSettings settings("NightSurvivors", "Saves");
    QString save_key = QString("save_%1").arg(slot);
    
    if (!settings.contains(save_key + "/timestamp")) {
        return false; // 槽位为空
    }
    
    // 加载详细游戏状态
    return loadGameData(getSaveSlotFilename(slot));
}

bool GameState::saveGameData(const QString &filename)
{
    // 创建一个JSON保存游戏状态
    QJsonObject gameData;
    
    // 保存基本游戏信息
    gameData["score"] = game_score;
    gameData["gameTime"] = game_time;
    gameData["totalCoins"] = total_coins;
    
    // 保存英雄信息
    if (my_hero) {
        QJsonObject heroData;
        heroData["characterId"] = my_hero->getCharacterId();
        heroData["level"] = my_hero->getLevel();
        heroData["health"] = my_hero->getHealth();
        heroData["maxHealth"] = my_hero->getMaxHealth();
        heroData["experience"] = my_hero->getExperience();
        heroData["x"] = my_hero->getX();
        heroData["y"] = my_hero->getY();
        
        gameData["hero"] = heroData;
    }
    
    // 保存全局升级
    QJsonArray upgradesArray;
    for (int level : global_upgrades) {
        upgradesArray.append(level);
    }
    gameData["upgrades"] = upgradesArray;
    
    // 保存角色解锁状态
    QJsonArray charactersArray;
    for (bool unlocked : unlocked_characters) {
        charactersArray.append(unlocked);
    }
    gameData["characters"] = charactersArray;
    
    // 保存到文件
    QJsonDocument doc(gameData);
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning("无法打开存档文件进行写入");
        return false;
    }
    
    file.write(doc.toJson());
    file.close();
    
    return true;
}

bool GameState::loadGameData(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("无法打开存档文件进行读取");
        return false;
    }
    
    QByteArray saveData = file.readAll();
    file.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(saveData);
    if (doc.isNull() || !doc.isObject()) {
        qWarning("存档文件格式错误");
        return false;
    }
    
    QJsonObject gameData = doc.object();
    
    // 重置当前游戏状态
    pause();
    
    // 加载基本游戏信息
    game_score = gameData["score"].toInt();
    game_time = gameData["gameTime"].toInt();
    total_coins = gameData["totalCoins"].toInt();
    
    // 加载英雄信息
    if (gameData.contains("hero") && gameData["hero"].isObject()) {
        QJsonObject heroData = gameData["hero"].toObject();
        int characterId = heroData["characterId"].toInt();
        
        // 选择角色并初始化
        selectCharacter(characterId);
        init();
        
        // 设置英雄属性
        if (my_hero) {
            my_hero->setLevel(heroData["level"].toInt());
            my_hero->setHealth(heroData["health"].toInt());
            my_hero->setMaxHealth(heroData["maxHealth"].toInt());
            my_hero->setExperience(heroData["experience"].toInt());
            my_hero->setPosition(heroData["x"].toDouble(), heroData["y"].toDouble());
        }
    }
    
    // 加载全局升级
    if (gameData.contains("upgrades") && gameData["upgrades"].isArray()) {
        QJsonArray upgradesArray = gameData["upgrades"].toArray();
        global_upgrades.clear();
        for (int i = 0; i < upgradesArray.size(); i++) {
            global_upgrades.append(upgradesArray[i].toInt());
        }
    }
    
    // 加载角色解锁状态
    if (gameData.contains("characters") && gameData["characters"].isArray()) {
        QJsonArray charactersArray = gameData["characters"].toArray();
        unlocked_characters.clear();
        for (int i = 0; i < charactersArray.size(); i++) {
            unlocked_characters.append(charactersArray[i].toBool());
        }
    }
    
    // 发出信号更新UI
    emit scoreUpdated(game_score);
    emit timeUpdated(game_time);
    emit coinsUpdated(game_coins);
    
    return true;
}

QString GameState::getSaveSlotFilename(int slot) const
{
    // 获取应用程序数据目录
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dataPath);
    
    // 确保目录存在
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    
    // 返回完整的文件路径
    return dir.filePath(QString("save_slot_%1.json").arg(slot));
}

void GameState::reset() {
    // 停止当前游戏
    pause();
    game_over = false;
    
    // 清除当前游戏状态
    delete my_hero;
    my_hero = nullptr;
    
    delete my_map;
    my_map = nullptr;
    
    // 清除敌人
    for (Enemy* enemy : my_enemies) {
        delete enemy;
    }
    my_enemies.clear();
    
    // 清除掉落物
    for (Drop* drop : my_drops) {
        delete drop;
    }
    my_drops.clear();
    
    // 重置游戏变量
    game_time = 0;
    game_score = 0;
    game_coins = 0;
    total_kills = 0;
}
