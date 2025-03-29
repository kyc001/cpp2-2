#include "../../include/utils/resourcemanager.h"
#include <QDebug>

// 单例实例获取
ResourceManager& ResourceManager::getInstance() {
    static ResourceManager instance;
    return instance;
}

// 构造函数
ResourceManager::ResourceManager() {
    // 初始化
    qDebug() << "ResourceManager: 初始化...";
}

// 析构函数
ResourceManager::~ResourceManager() {
    // 清理资源
    pixmapCache.clear();
    qDebug() << "ResourceManager: 释放资源...";
}

// 加载英雄资源
bool ResourceManager::loadHeroResources(int heroType) {
    QString heroName = heroTypeToString(heroType);
    if (heroName.isEmpty()) {
        qDebug() << "ResourceManager: 无效的英雄类型:" << heroType;
        return false;
    }
    
    bool success = true;
    
    // 加载静止图像
    QString idlePath = QString(":/heroes/%1/%1_idle.png").arg(heroName);
    success &= loadPixmap(idlePath);
    
    // 加载行走动画帧
    for (int i = 0; i < 4; i++) {
        QString walkPath = QString(":/heroes/%1/%1_walk_%2.png").arg(heroName).arg(i);
        success &= loadPixmap(walkPath);
    }
    
    // 加载攻击动画帧
    for (int i = 0; i < 3; i++) {
        QString attackPath = QString(":/heroes/%1/%1_attack_%2.png").arg(heroName).arg(i);
        success &= loadPixmap(attackPath);
    }
    
    if (success) {
        qDebug() << "ResourceManager: 成功加载英雄资源:" << heroName;
    } else {
        qDebug() << "ResourceManager: 部分英雄资源加载失败:" << heroName;
    }
    
    return success;
}

// 获取英雄静止图像
QPixmap ResourceManager::getHeroIdleImage(int heroType) const {
    QString heroName = heroTypeToString(heroType);
    if (heroName.isEmpty()) {
        qDebug() << "ResourceManager: 无效的英雄类型:" << heroType;
        return QPixmap();
    }
    
    QString idlePath = QString(":/heroes/%1/%1_idle.png").arg(heroName);
    return getPixmap(idlePath);
}

// 获取英雄行走动画帧
QPixmap ResourceManager::getHeroWalkImage(int heroType, int frame) const {
    QString heroName = heroTypeToString(heroType);
    if (heroName.isEmpty()) {
        qDebug() << "ResourceManager: 无效的英雄类型:" << heroType;
        return QPixmap();
    }
    
    // 确保帧号在有效范围内
    frame = qBound(0, frame, 3);
    
    QString walkPath = QString(":/heroes/%1/%1_walk_%2.png").arg(heroName).arg(frame);
    return getPixmap(walkPath);
}

// 获取英雄攻击动画帧
QPixmap ResourceManager::getHeroAttackImage(int heroType, int frame) const {
    QString heroName = heroTypeToString(heroType);
    if (heroName.isEmpty()) {
        qDebug() << "ResourceManager: 无效的英雄类型:" << heroType;
        return QPixmap();
    }
    
    // 确保帧号在有效范围内
    frame = qBound(0, frame, 2);
    
    QString attackPath = QString(":/heroes/%1/%1_attack_%2.png").arg(heroName).arg(frame);
    return getPixmap(attackPath);
}

// 加载敌人资源
bool ResourceManager::loadEnemyResources(int enemyType) {
    QString enemyName = enemyTypeToString(enemyType);
    if (enemyName.isEmpty()) {
        qDebug() << "ResourceManager: 无效的敌人类型:" << enemyType;
        return false;
    }
    
    bool success = true;
    
    // 加载静止图像
    QString idlePath = QString(":/enemies/%1/%1_idle.png").arg(enemyName);
    success &= loadPixmap(idlePath);
    
    // 加载行走动画帧
    for (int i = 0; i < 4; i++) {
        QString walkPath = QString(":/enemies/%1/%1_walk_%2.png").arg(enemyName).arg(i);
        success &= loadPixmap(walkPath);
    }
    
    // 加载攻击动画帧
    for (int i = 0; i < 3; i++) {
        QString attackPath = QString(":/enemies/%1/%1_attack_%2.png").arg(enemyName).arg(i);
        success &= loadPixmap(attackPath);
    }
    
    // 如果是Boss，加载特殊动画帧
    if (enemyName == "boss") {
        for (int i = 0; i < 5; i++) {
            QString specialPath = QString(":/enemies/%1/%1_special_%2.png").arg(enemyName).arg(i);
            success &= loadPixmap(specialPath);
        }
    }
    
    if (success) {
        qDebug() << "ResourceManager: 成功加载敌人资源:" << enemyName;
    } else {
        qDebug() << "ResourceManager: 部分敌人资源加载失败:" << enemyName;
    }
    
    return success;
}

// 获取敌人静止图像
QPixmap ResourceManager::getEnemyIdleImage(int enemyType) const {
    QString enemyName = enemyTypeToString(enemyType);
    if (enemyName.isEmpty()) {
        qDebug() << "ResourceManager: 无效的敌人类型:" << enemyType;
        return QPixmap();
    }
    
    QString idlePath = QString(":/enemies/%1/%1_idle.png").arg(enemyName);
    return getPixmap(idlePath);
}

// 获取敌人行走动画帧
QPixmap ResourceManager::getEnemyWalkImage(int enemyType, int frame) const {
    QString enemyName = enemyTypeToString(enemyType);
    if (enemyName.isEmpty()) {
        qDebug() << "ResourceManager: 无效的敌人类型:" << enemyType;
        return QPixmap();
    }
    
    // 确保帧号在有效范围内
    frame = qBound(0, frame, 3);
    
    QString walkPath = QString(":/enemies/%1/%1_walk_%2.png").arg(enemyName).arg(frame);
    return getPixmap(walkPath);
}

// 获取敌人攻击动画帧
QPixmap ResourceManager::getEnemyAttackImage(int enemyType, int frame) const {
    QString enemyName = enemyTypeToString(enemyType);
    if (enemyName.isEmpty()) {
        qDebug() << "ResourceManager: 无效的敌人类型:" << enemyType;
        return QPixmap();
    }
    
    // 确保帧号在有效范围内
    frame = qBound(0, frame, 2);
    
    QString attackPath = QString(":/enemies/%1/%1_attack_%2.png").arg(enemyName).arg(frame);
    return getPixmap(attackPath);
}

// 获取敌人特殊动画帧（仅限Boss）
QPixmap ResourceManager::getEnemySpecialImage(int enemyType, int frame) const {
    QString enemyName = enemyTypeToString(enemyType);
    if (enemyName != "boss") {
        qDebug() << "ResourceManager: 特殊动画仅适用于Boss";
        return QPixmap();
    }
    
    // 确保帧号在有效范围内
    frame = qBound(0, frame, 4);
    
    QString specialPath = QString(":/enemies/%1/%1_special_%2.png").arg(enemyName).arg(frame);
    return getPixmap(specialPath);
}

// 加载武器资源
bool ResourceManager::loadWeaponResources(int weaponType) {
    QString weaponName = weaponTypeToString(weaponType);
    if (weaponName.isEmpty()) {
        qDebug() << "ResourceManager: 无效的武器类型:" << weaponType;
        return false;
    }
    
    bool success = true;
    
    // 加载武器图像
    QString weaponPath = QString(":/weapons/%1/%1.png").arg(weaponName);
    success &= loadPixmap(weaponPath);
    
    // 加载武器效果动画帧
    for (int i = 0; i < 4; i++) {
        QString effectPath = QString(":/weapons/%1/effects/%1_effect_%2.png").arg(weaponName).arg(i);
        success &= loadPixmap(effectPath);
    }
    
    // 特殊处理弓
    if (weaponName == "bow") {
        QString arrowPath = ":/weapons/bow/arrow.png";
        success &= loadPixmap(arrowPath);
    }
    
    if (success) {
        qDebug() << "ResourceManager: 成功加载武器资源:" << weaponName;
    } else {
        qDebug() << "ResourceManager: 部分武器资源加载失败:" << weaponName;
    }
    
    return success;
}

// 获取武器图像
QPixmap ResourceManager::getWeaponImage(int weaponType) const {
    QString weaponName = weaponTypeToString(weaponType);
    if (weaponName.isEmpty()) {
        qDebug() << "ResourceManager: 无效的武器类型:" << weaponType;
        return QPixmap();
    }
    
    QString weaponPath = QString(":/weapons/%1/%1.png").arg(weaponName);
    return getPixmap(weaponPath);
}

// 获取武器效果动画帧
QPixmap ResourceManager::getWeaponEffectImage(int weaponType, int frame) const {
    QString weaponName = weaponTypeToString(weaponType);
    if (weaponName.isEmpty()) {
        qDebug() << "ResourceManager: 无效的武器类型:" << weaponType;
        return QPixmap();
    }
    
    // 确保帧号在有效范围内
    frame = qBound(0, frame, 3);
    
    QString effectPath = QString(":/weapons/%1/effects/%1_effect_%2.png").arg(weaponName).arg(frame);
    return getPixmap(effectPath);
}

// 加载地图资源
bool ResourceManager::loadMapResources() {
    bool success = true;
    
    // 加载地板材质
    QStringList floorTypes = {"grass", "dirt", "stone", "wood"};
    for (const QString& type : floorTypes) {
        QString path = QString(":/maps/floors/%1.png").arg(type);
        success &= loadPixmap(path);
    }
    
    // 加载障碍物
    QStringList obstacleTypes = {"tree", "rock", "wall", "crate", "barrel"};
    for (const QString& type : obstacleTypes) {
        QString path = QString(":/maps/obstacles/%1.png").arg(type);
        success &= loadPixmap(path);
    }
    
    // 加载装饰物
    QStringList decorationTypes = {"flower", "grass", "torch", "gravestone"};
    for (const QString& type : decorationTypes) {
        QString path = QString(":/maps/decorations/%1.png").arg(type);
        success &= loadPixmap(path);
    }
    
    // 加载预设地图
    QStringList mapTypes = {"dungeon", "forest", "cemetery"};
    for (const QString& type : mapTypes) {
        QString path = QString(":/maps/presets/%1.png").arg(type);
        success &= loadPixmap(path);
    }
    
    if (success) {
        qDebug() << "ResourceManager: 成功加载地图资源";
    } else {
        qDebug() << "ResourceManager: 部分地图资源加载失败";
    }
    
    return success;
}

// 获取地板材质
QPixmap ResourceManager::getFloorTile(const QString& type) const {
    QString path = QString(":/maps/floors/%1.png").arg(type);
    return getPixmap(path);
}

// 获取障碍物
QPixmap ResourceManager::getObstacle(const QString& type) const {
    QString path = QString(":/maps/obstacles/%1.png").arg(type);
    return getPixmap(path);
}

// 获取装饰物
QPixmap ResourceManager::getDecoration(const QString& type) const {
    QString path = QString(":/maps/decorations/%1.png").arg(type);
    return getPixmap(path);
}

// 获取预设地图
QPixmap ResourceManager::getMapPreset(const QString& type) const {
    QString path = QString(":/maps/presets/%1.png").arg(type);
    return getPixmap(path);
}

// 加载UI资源
bool ResourceManager::loadUIResources() {
    bool success = true;
    
    // 加载按钮
    QStringList buttonStyles = {"normal", "fancy", "wood"};
    for (const QString& style : buttonStyles) {
        QString path = QString(":/ui/buttons/button_%1.png").arg(style);
        success &= loadPixmap(path);
    }
    
    // 加载图标
    QStringList iconTypes = {"health", "xp", "coin", "weapon", "shield", "magic"};
    for (const QString& type : iconTypes) {
        QString path = QString(":/ui/icons/icon_%1.png").arg(type);
        success &= loadPixmap(path);
    }
    
    // 加载状态条
    QStringList barTypes = {"health", "mana", "xp"};
    for (const QString& type : barTypes) {
        QString path = QString(":/ui/bars/bar_%1.png").arg(type);
        success &= loadPixmap(path);
    }
    
    // 加载背景
    QStringList bgTypes = {"menu", "game", "dark"};
    for (const QString& type : bgTypes) {
        QString path = QString(":/ui/backgrounds/bg_%1.png").arg(type);
        success &= loadPixmap(path);
    }
    
    // 加载框架
    QStringList frameStyles = {"basic", "ornate", "minimal"};
    for (const QString& style : frameStyles) {
        QString path = QString(":/ui/frames/frame_%1.png").arg(style);
        success &= loadPixmap(path);
    }
    
    if (success) {
        qDebug() << "ResourceManager: 成功加载UI资源";
    } else {
        qDebug() << "ResourceManager: 部分UI资源加载失败";
    }
    
    return success;
}

// 获取按钮图像
QPixmap ResourceManager::getButtonImage(const QString& style) const {
    QString path = QString(":/ui/buttons/button_%1.png").arg(style);
    return getPixmap(path);
}

// 获取图标图像
QPixmap ResourceManager::getIconImage(const QString& type) const {
    QString path = QString(":/ui/icons/icon_%1.png").arg(type);
    return getPixmap(path);
}

// 获取状态条图像
QPixmap ResourceManager::getBarImage(const QString& type) const {
    QString path = QString(":/ui/bars/bar_%1.png").arg(type);
    return getPixmap(path);
}

// 获取背景图像
QPixmap ResourceManager::getBackgroundImage(const QString& type) const {
    QString path = QString(":/ui/backgrounds/bg_%1.png").arg(type);
    return getPixmap(path);
}

// 获取框架图像
QPixmap ResourceManager::getFrameImage(const QString& style) const {
    QString path = QString(":/ui/frames/frame_%1.png").arg(style);
    return getPixmap(path);
}

// 加载图像到缓存
bool ResourceManager::loadPixmap(const QString& path, bool addToCache) {
    if (pixmapCache.contains(path)) {
        return true; // 已加载
    }
    
    QPixmap pixmap(path);
    if (pixmap.isNull()) {
        qDebug() << "ResourceManager: 无法加载图像:" << path;
        return false;
    }
    
    if (addToCache) {
        pixmapCache.insert(path, pixmap);
    }
    
    return true;
}

// 从缓存获取图像
QPixmap ResourceManager::getPixmap(const QString& path) const {
    if (pixmapCache.contains(path)) {
        return pixmapCache.value(path);
    }
    
    qDebug() << "ResourceManager: 图像未找到:" << path;
    return QPixmap(); // 返回空图像
}

// 转换英雄类型到名称
QString ResourceManager::heroTypeToString(int heroType) const {
    switch (heroType) {
        case 0: return "warrior";
        case 1: return "mage";
        case 2: return "archer";
        case 3: return "rogue";
        default: return "";
    }
}

// 转换敌人类型到名称
QString ResourceManager::enemyTypeToString(int enemyType) const {
    switch (enemyType) {
        case 0: return "melee_basic";
        case 1: return "melee_fast";
        case 2: return "ranged";
        case 3: return "tank";
        case 4: return "boss";
        default: return "";
    }
}

// 转换武器类型到名称
QString ResourceManager::weaponTypeToString(int weaponType) const {
    switch (weaponType) {
        case 0: return "sword";
        case 1: return "wand";
        case 2: return "bow";
        case 3: return "dagger";
        default: return "";
    }
} 