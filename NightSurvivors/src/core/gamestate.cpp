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
    game_time(0), max_game_time(0), game_score(0), total_kills(0), selected_character(0),
    game_coins(0), total_coins(0), coin_multiplier(1.0f), save_file(nullptr),
    spawn_rate(5), max_enemies(100), my_screenCenterX(0), my_screenCenterY(0) {
    
    // Initialize screen center
    my_screenCenterX = 400;
    my_screenCenterY = 300;
    
    // Initialize global upgrades
    global_upgrades.resize(5);
    for (int i = 0; i < global_upgrades.size(); i++) {
        global_upgrades[i] = 0;
    }
    
    // Initialize character unlock status
    unlocked_characters.resize(4);
    unlocked_characters[0] = true;  // Default unlock warrior
    for (int i = 1; i < unlocked_characters.size(); i++) {
        unlocked_characters[i] = false;
    }
    
    // Create save file object
    save_file = new SaveFile(this);
    
    // Create game timer (updates every 16ms ~ 60fps)
    game_timer = new QTimer(this);
    connect(game_timer, &QTimer::timeout, this, &GameState::increaseTime);
    
    // Create spawn timer
    spawn_timer = new QTimer(this);
    connect(spawn_timer, &QTimer::timeout, this, &GameState::spawnEnemies);
    
    // Create game map
    my_map = new GameMap(this);
    
    // Load game save
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
    
    // Delete all drops
    for (Drop* drop : my_drops) {
        delete drop;
    }
    my_drops.clear();
    
    // Delete save file object
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
    
    // Apply global upgrades
    applyGlobalUpgradesToGame();
    
    // Connect hero level up signal
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
    
    // Clear drops
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
    
    // Save coins when game ends
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
    // Apply coin multiplier
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
    // Generate random drop
    int dropRand = QRandomGenerator::global()->bounded(100);
    
    if (dropRand < 15) { // 15% chance for health
        Drop* drop = new Drop(DropType::HEALTH, 20, x, y, this, this);
        addDrop(drop);
    } else if (dropRand < 40) { // 25% chance for experience
        Drop* drop = new Drop(DropType::EXPERIENCE, 10, x, y, this, this);
        addDrop(drop);
    } else if (dropRand < 70) { // 30% chance for coins
        Drop* drop = new Drop(DropType::COIN, 1, x, y, this, this);
        addDrop(drop);
    } else if (dropRand < 80) { // 10% chance for temporary power-up
        Drop* drop = new Drop(DropType::POWER_UP, 5, x, y, this, this);
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
        // Pick a random enemy type (0-3)
        int enemy_type = QRandomGenerator::global()->bounded(4);
        
        // Pick a random spawn position at the edge of the map
        int x, y;
        int edge = QRandomGenerator::global()->bounded(4); // 0-3 for top, right, bottom, left
        
        int mapWidth = 800;
        int mapHeight = 600;
        
        // 使用固定值代替map_width和map_height
        if (my_map) {
            mapWidth = my_map->getWidth();
            mapHeight = my_map->getHeight();
        }
        
        switch (edge) {
            case 0: // Top
                x = QRandomGenerator::global()->bounded(mapWidth);
                y = 0;
                break;
            case 1: // Right
                x = mapWidth - 1;
                y = QRandomGenerator::global()->bounded(mapHeight);
                break;
            case 2: // Bottom
                x = QRandomGenerator::global()->bounded(mapWidth);
                y = mapHeight - 1;
                break;
            case 3: // Left
                x = 0;
                y = QRandomGenerator::global()->bounded(mapHeight);
                break;
        }
        
        // 简单创建敌人，不依赖资源文件
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
        
        // Apply upgrade effect
        if (my_hero) {
            switch (type) {
                case 0: // Maximum health
                    my_hero->setMaxHealth(100 + level * 20);
                    break;
                case 1: // Base attack
                    my_hero->setBaseDamage(5 + level * 2);
                    break;
                // Other upgrade types can be added here...
            }
        }
        
        // Save game
        saveGame();
    }
}

void GameState::applyGlobalUpgradesToGame() {
    // Apply global upgrade effects to the game
    if (my_hero) {
        // Maximum health
        int max_health_level = global_upgrades[0];
        my_hero->setMaxHealth(100 + max_health_level * 20);
        
        // Base attack
        int attack_level = global_upgrades[1];
        my_hero->setBaseDamage(5 + attack_level * 2);
        
        // Pickup range (4th upgrade)
        if (global_upgrades.size() > 3) {
            int pickup_range_level = global_upgrades[3];
            my_hero->setPickupRange(30 + pickup_range_level * 10);
        }
        
        // Coin multiplier (3rd upgrade)
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
    
    // Save total coins
    save_file->setTotalCoins(total_coins);
    
    // Save global upgrade levels
    save_file->setGlobalUpgrades(global_upgrades);
    
    // Save character unlock status
    save_file->setUnlockedCharacters(unlocked_characters);
    
    // Save to file
    QString saveDir = QDir::homePath() + "/.nightsurvivors";
    QDir().mkpath(saveDir);
    
    return save_file->saveToFile(saveDir + "/save.dat");
}

bool GameState::loadGame() {
    if (!save_file) {
        return false;
    }
    
    // Try to load from file
    QString saveDir = QDir::homePath() + "/.nightsurvivors";
    bool result = save_file->loadFromFile(saveDir + "/save.dat");
    
    if (result) {
        // Read total coins
        total_coins = save_file->getTotalCoins();
        
        // Read global upgrade levels
        global_upgrades = save_file->getGlobalUpgrades();
        
        // Read character unlock status
        unlocked_characters = save_file->getUnlockedCharacters();
        
        // Ensure array size consistency
        if (global_upgrades.size() < 5) {
            global_upgrades.resize(5, 0);
        }
        
        if (unlocked_characters.size() < 4) {
            unlocked_characters.resize(4, false);
            unlocked_characters[0] = true; // Ensure first character is unlocked
        }
        
        // Apply global upgrades
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
    // Send signal to UI to display level up options
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
            
            // Probability for drop generation
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
    
    // Collect inactive drops
    for (Drop* drop : my_drops) {
        if (drop && !drop->isActive()) {
            inactive_drops.append(drop);
        }
    }
    
    // Remove and delete inactive drops
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
            
            // Check if in pickup range
            if (abs(heroX - dropX) <= pickupRange && abs(heroY - dropY) <= pickupRange) {
                // Apply drop effect
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
    // Get current game state information
    int current_score = getScore();
    int current_level = 1;
    QString character_name = "Unknown";
    
    if (my_hero) {
        current_level = my_hero->getLevel();
        switch (my_hero->getCharacterId()) {
            case 0: character_name = "Warrior"; break;
            case 1: character_name = "Mage"; break;
            case 2: character_name = "Archer"; break;
            case 3: character_name = "Thief"; break;
            default: character_name = "Unknown";
        }
    }
    
    // Save to specified slot
    QSettings settings("NightSurvivors", "Saves");
    QString save_key = QString("save_%1").arg(slot);
    
    settings.setValue(save_key + "/timestamp", QDateTime::currentDateTime());
    settings.setValue(save_key + "/score", current_score);
    settings.setValue(save_key + "/level", current_level);
    settings.setValue(save_key + "/character", character_name);
    settings.setValue(save_key + "/totalCoins", total_coins);
    
    // Save detailed game state to file
    return saveGameData(getSaveSlotFilename(slot));
}

bool GameState::loadGameFromSlot(int slot)
{
    // Check if slot exists in settings
    QSettings settings("NightSurvivors", "Saves");
    QString save_key = QString("save_%1").arg(slot);
    
    if (!settings.contains(save_key + "/timestamp")) {
        return false; // Slot is empty
    }
    
    // Load detailed game state
    return loadGameData(getSaveSlotFilename(slot));
}

bool GameState::saveGameData(const QString &filename)
{
    // Create a JSON to save game state
    QJsonObject gameData;
    
    // Save basic game information
    gameData["score"] = game_score;
    gameData["gameTime"] = game_time;
    gameData["totalCoins"] = total_coins;
    
    // Save hero information
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
    
    // Save global upgrades
    QJsonArray upgradesArray;
    for (int level : global_upgrades) {
        upgradesArray.append(level);
    }
    gameData["upgrades"] = upgradesArray;
    
    // Save character unlock status
    QJsonArray charactersArray;
    for (bool unlocked : unlocked_characters) {
        charactersArray.append(unlocked);
    }
    gameData["characters"] = charactersArray;
    
    // Save to file
    QJsonDocument doc(gameData);
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning("Unable to open save file for writing");
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
        qWarning("Unable to open save file for reading");
        return false;
    }
    
    QByteArray saveData = file.readAll();
    file.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(saveData);
    if (doc.isNull() || !doc.isObject()) {
        qWarning("Save file format error");
        return false;
    }
    
    QJsonObject gameData = doc.object();
    
    // Reset current game state
    pause();
    
    // Load basic game information
    game_score = gameData["score"].toInt();
    game_time = gameData["gameTime"].toInt();
    total_coins = gameData["totalCoins"].toInt();
    
    // Load hero information
    if (gameData.contains("hero") && gameData["hero"].isObject()) {
        QJsonObject heroData = gameData["hero"].toObject();
        int characterId = heroData["characterId"].toInt();
        
        // Select character and initialize
        selectCharacter(characterId);
        init();
        
        // Set hero attributes
        if (my_hero) {
            my_hero->setLevel(heroData["level"].toInt());
            my_hero->setHealth(heroData["health"].toInt());
            my_hero->setMaxHealth(heroData["maxHealth"].toInt());
            my_hero->setExperience(heroData["experience"].toInt());
            my_hero->setPosition(heroData["x"].toDouble(), heroData["y"].toDouble());
        }
    }
    
    // Load global upgrades
    if (gameData.contains("upgrades") && gameData["upgrades"].isArray()) {
        QJsonArray upgradesArray = gameData["upgrades"].toArray();
        global_upgrades.clear();
        for (int i = 0; i < upgradesArray.size(); i++) {
            global_upgrades.append(upgradesArray[i].toInt());
        }
    }
    
    // Load character unlock status
    if (gameData.contains("characters") && gameData["characters"].isArray()) {
        QJsonArray charactersArray = gameData["characters"].toArray();
        unlocked_characters.clear();
        for (int i = 0; i < charactersArray.size(); i++) {
            unlocked_characters.append(charactersArray[i].toBool());
        }
    }
    
    // Send signal to update UI
    emit scoreUpdated(game_score);
    emit timeUpdated(game_time);
    emit coinsUpdated(game_coins);
    
    return true;
}

QString GameState::getSaveSlotFilename(int slot) const
{
    // Get application data directory
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dataPath);
    
    // Ensure directory exists
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    
    // Return full file path
    return dir.filePath(QString("save_slot_%1.json").arg(slot));
}

void GameState::reset() {
    // Stop current game
    pause();
    game_over = false;
    
    // Clear current game state
    delete my_hero;
    my_hero = nullptr;
    
    delete my_map;
    my_map = nullptr;
    
    // Clear enemies
    for (Enemy* enemy : my_enemies) {
        delete enemy;
    }
    my_enemies.clear();
    
    // Clear drops
    for (Drop* drop : my_drops) {
        delete drop;
    }
    my_drops.clear();
    
    // Reset game variables
    game_time = 0;
    game_score = 0;
    game_coins = 0;
    total_kills = 0;
}

void GameState::setMaxGameTime(int seconds)
{
    max_game_time = seconds;
    // Set current game time to max (countdown starts)
    game_time = max_game_time;
    emit timeUpdated(game_time);
}

void GameState::increaseTime()
{
    if (game_running && !game_over) {
        // Use countdown
        if (max_game_time > 0) {
            game_time--;
            if (game_time <= 0) {
                // Time ends, game ends
                game_time = 0;
                gameOver();
            }
        } else {
            // No max time set, use timer
            game_time++;
        }
        
        emit timeUpdated(game_time);
        
        // Check level up every 5 seconds
        if (game_time % 5 == 0) {
            checkLevelUp();
        }
        
        // Save every 10 seconds
        if (game_time % 10 == 0) {
            saveGame();
        }
    }
}

SaveFile* GameState::getSaveFile() const 
{
    return save_file;
}

void GameState::checkLevelUp()
{
    // 每30秒给予玩家一次随机增益
    if (game_time > 0 && game_time % 30 == 0 && my_hero && my_hero->isAlive()) {
        // 生成随机增益类型 (0-4)
        int upgradeType = QRandomGenerator::global()->bounded(5);
        
        // 应用不同类型的增益
        switch (upgradeType) {
            case 0: // 增加最大生命值
                my_hero->setMaxHealth(my_hero->getMaxHealth() + 20);
                my_hero->heal(20); // 同时恢复一些生命值
                break;
                
            case 1: // 增加攻击力
                my_hero->addAttack(5);
                break;
                
            case 2: // 增加移动速度
                my_hero->addSpeed(0.2f);
                break;
                
            case 3: // 增加拾取范围
                my_hero->addPickupRange(15);
                break;
                
            case 4: // 临时能力提升
                my_hero->applyPowerUp(10);
                break;
        }
        
        // 显示增益信息
        QString upgradeMsg;
        switch (upgradeType) {
            case 0: upgradeMsg = "最大生命值+20!"; break;
            case 1: upgradeMsg = "攻击力+5!"; break;
            case 2: upgradeMsg = "移动速度+20%!"; break;
            case 3: upgradeMsg = "拾取范围+15!"; break;
            case 4: upgradeMsg = "临时能力提升!"; break;
        }
        
        // 这里是发送消息通知UI显示增益信息
        // 实际实现需要添加相应的信号和槽
        emit messageSignal(upgradeMsg);
    }
}

QVector<int> GameState::getGlobalUpgrades() const {
    return global_upgrades;
}

QVector<bool> GameState::getUnlockedCharacters() const {
    return unlocked_characters;
}
