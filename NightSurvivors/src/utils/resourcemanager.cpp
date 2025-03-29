#include "../../include/utils/resourcemanager.h"
#include <QDebug>
#include <QFile>
#include <QDirIterator>

// 初始化静态实例
ResourceManager* ResourceManager::instance = nullptr;

ResourceManager::ResourceManager(QObject* parent) : QObject(parent)
{
    qDebug() << "ResourceManager: 初始化...";
}

ResourceManager::~ResourceManager()
{
    clearResources();
}

ResourceManager& ResourceManager::getInstance()
{
    if (!instance) {
        instance = new ResourceManager();
    }
    return *instance;
}

void ResourceManager::loadResources()
{
    // 不加载资源
}

void ResourceManager::clearResources()
{
    // 清空所有资源缓存
    ui_resources.clear();
    map_resources.clear();
    enemy_resources.clear();
    weapon_resources.clear();
    hero_resources.clear();
}

QPixmap ResourceManager::getUIResource(const QString& name)
{
    if (ui_resources.contains(name)) {
        return ui_resources[name];
    }
    
    // 尝试加载资源
    QString path = ":/resources/ui/" + name;
    QPixmap pixmap(path);
    
    if (!pixmap.isNull()) {
        ui_resources[name] = pixmap;
        return pixmap;
    }
    
    // 返回一个空的pixmap
    return QPixmap();
}

QPixmap ResourceManager::getMapResource(const QString& name)
{
    if (map_resources.contains(name)) {
        return map_resources[name];
    }
    
    // 尝试加载资源
    QString path = ":/resources/maps/" + name;
    QPixmap pixmap(path);
    
    if (!pixmap.isNull()) {
        map_resources[name] = pixmap;
        return pixmap;
    }
    
    // 返回一个空的pixmap
    return QPixmap();
}

QPixmap ResourceManager::getEnemyImage(const QString& type, const QString& animation, int frame)
{
    QString key = QString("%1_%2_%3").arg(type).arg(animation).arg(frame);
    
    if (enemy_resources.contains(key)) {
        return enemy_resources[key];
    }
    
    // 尝试加载资源
    QString path = QString(":/resources/enemies/%1/%1_%2_%3.png").arg(type).arg(animation).arg(frame);
    QPixmap pixmap(path);
    
    // 如果没有找到带数字的帧，尝试不带数字的版本（如idle图像）
    if (pixmap.isNull() && animation == "idle") {
        path = QString(":/resources/enemies/%1/%1_idle.png").arg(type);
        pixmap = QPixmap(path);
    }
    
    if (!pixmap.isNull()) {
        enemy_resources[key] = pixmap;
        return pixmap;
    }
    
    // 返回一个空的pixmap
    return QPixmap();
}

QPixmap ResourceManager::getWeaponImage(const QString& type, const QString& animation, int frame)
{
    QString key = QString("%1_%2_%3").arg(type).arg(animation).arg(frame);
    
    if (weapon_resources.contains(key)) {
        return weapon_resources[key];
    }
    
    // 尝试加载资源
    QString path;
    
    if (animation == "base") {
        path = QString(":/resources/weapons/%1/%1.png").arg(type);
    } else {
        path = QString(":/resources/weapons/%1/effects/%1_effect_%2.png").arg(type).arg(frame);
    }
    
    QPixmap pixmap(path);
    
    if (!pixmap.isNull()) {
        weapon_resources[key] = pixmap;
        return pixmap;
    }
    
    // 返回一个空的pixmap
    return QPixmap();
}

QPixmap ResourceManager::getHeroImage(const QString& type, const QString& animation, int frame)
{
    QString key = QString("%1_%2_%3").arg(type).arg(animation).arg(frame);
    
    if (hero_resources.contains(key)) {
        return hero_resources[key];
    }
    
    // 尝试加载资源
    QString path;
    
    if (animation == "idle") {
        path = QString(":/resources/heroes/%1/%1_idle.png").arg(type);
    } else if (animation == "attack") {
        path = QString(":/resources/heroes/%1/%1_attack_%2.png").arg(type).arg(frame);
    } else if (animation == "walk") {
        path = QString(":/resources/heroes/%1/%1_walk_%2.png").arg(type).arg(frame);
    }
    
    QPixmap pixmap(path);
    
    if (!pixmap.isNull()) {
        hero_resources[key] = pixmap;
        return pixmap;
    }
    
    // 返回一个空的pixmap
    return QPixmap();
}

bool ResourceManager::loadUIResources()
{
    // 加载按钮
    ui_resources["button_normal"] = QPixmap(":/ui/buttons/button_normal.png");
    ui_resources["button_fancy"] = QPixmap(":/ui/buttons/button_fancy.png");
    ui_resources["button_wood"] = QPixmap(":/ui/buttons/button_wood.png");
    
    // 加载背景
    ui_resources["bg_menu"] = QPixmap(":/ui/backgrounds/bg_menu.png");
    ui_resources["bg_game"] = QPixmap(":/ui/backgrounds/bg_game.png");
    ui_resources["bg_dark"] = QPixmap(":/ui/backgrounds/bg_dark.png");
    
    // 加载框架
    ui_resources["frame_basic"] = QPixmap(":/ui/frames/frame_basic.png");
    ui_resources["frame_ornate"] = QPixmap(":/ui/frames/frame_ornate.png");
    ui_resources["frame_minimal"] = QPixmap(":/ui/frames/frame_minimal.png");
    
    // 加载图标
    ui_resources["icon_coin"] = QPixmap(":/ui/icons/icon_coin.png");
    ui_resources["icon_health"] = QPixmap(":/ui/icons/icon_health.png");
    ui_resources["icon_magic"] = QPixmap(":/ui/icons/icon_magic.png");
    ui_resources["icon_shield"] = QPixmap(":/ui/icons/icon_shield.png");
    ui_resources["icon_weapon"] = QPixmap(":/ui/icons/icon_weapon.png");
    ui_resources["icon_xp"] = QPixmap(":/ui/icons/icon_xp.png");
    
    // 加载条形
    ui_resources["bar_health"] = QPixmap(":/ui/bars/bar_health.png");
    ui_resources["bar_mana"] = QPixmap(":/ui/bars/bar_mana.png");
    ui_resources["bar_xp"] = QPixmap(":/ui/bars/bar_xp.png");
    
    qDebug() << "ResourceManager: 成功加载UI资源";
    return true;
}

bool ResourceManager::loadMapResources()
{
    bool success = true;
    
    // 加载地形
    QStringList terrainTypes = {"dirt", "grass", "stone", "wood"};
    for (const QString& type : terrainTypes) {
        QString path = QString(":/maps/floors/%1.png").arg(type);
        success &= loadPixmap(path);
    }
    
    // 加载障碍物
    QStringList obstacleTypes = {"barrel", "crate", "rock", "tree", "wall"};
    for (const QString& type : obstacleTypes) {
        QString path = QString(":/maps/obstacles/%1.png").arg(type);
        success &= loadPixmap(path);
    }
    
    // 加载装饰物
    QStringList decorTypes = {"flower", "grass", "gravestone", "torch"};
    for (const QString& type : decorTypes) {
        QString path = QString(":/maps/decorations/%1.png").arg(type);
        success &= loadPixmap(path);
    }
    
    // 加载地图预设
    QStringList presetTypes = {"cemetery", "dungeon", "forest"};
    for (const QString& type : presetTypes) {
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

void ResourceManager::loadEnemyResources(const QString& enemyType)
{
    // 加载敌人静止状态图像
    QString idlePath = QString(":/resources/enemies/%1/%1_idle.png").arg(enemyType);
    enemy_resources[QString("%1_idle_0").arg(enemyType)] = QPixmap(idlePath);
    
    // 加载敌人攻击动画
    for (int i = 0; i < 3; ++i) {
        QString attackPath = QString(":/resources/enemies/%1/%1_attack_%2.png").arg(enemyType).arg(i);
        enemy_resources[QString("%1_attack_%2").arg(enemyType).arg(i)] = QPixmap(attackPath);
    }
    
    // 加载敌人行走动画
    for (int i = 0; i < 4; ++i) {
        QString walkPath = QString(":/resources/enemies/%1/%1_walk_%2.png").arg(enemyType).arg(i);
        enemy_resources[QString("%1_walk_%2").arg(enemyType).arg(i)] = QPixmap(walkPath);
    }
    
    qDebug() << "ResourceManager: 成功加载敌人类型:" << "\"" + enemyType + "\"";
}

void ResourceManager::loadWeaponResources(const QString& weaponType)
{
    // 加载武器基础图像
    QString basePath = QString(":/resources/weapons/%1/%1.png").arg(weaponType);
    weapon_resources[QString("%1_base_0").arg(weaponType)] = QPixmap(basePath);
    
    // 加载武器特效
    for (int i = 0; i < 4; ++i) {
        QString effectPath = QString(":/resources/weapons/%1/effects/%1_effect_%2.png").arg(weaponType).arg(i);
        weapon_resources[QString("%1_effect_%2").arg(weaponType).arg(i)] = QPixmap(effectPath);
    }
    
    qDebug() << "ResourceManager: 成功加载武器资源:" << "\"" + weaponType + "\"";
}

void ResourceManager::loadHeroResources(const QString& heroType)
{
    // 加载英雄静止状态图像
    QString idlePath = QString(":/resources/heroes/%1/%1_idle.png").arg(heroType);
    hero_resources[QString("%1_idle_0").arg(heroType)] = QPixmap(idlePath);
    
    // 加载英雄攻击动画
    for (int i = 0; i < 3; ++i) {
        QString attackPath = QString(":/resources/heroes/%1/%1_attack_%2.png").arg(heroType).arg(i);
        hero_resources[QString("%1_attack_%2").arg(heroType).arg(i)] = QPixmap(attackPath);
    }
    
    // 加载英雄行走动画
    for (int i = 0; i < 4; ++i) {
        QString walkPath = QString(":/resources/heroes/%1/%1_walk_%2.png").arg(heroType).arg(i);
        hero_resources[QString("%1_walk_%2").arg(heroType).arg(i)] = QPixmap(walkPath);
    }
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

QPixmap ResourceManager::getPixmap(const QString &name)
{
    // 不使用资源文件，返回空的QPixmap
    Q_UNUSED(name);
    return QPixmap();
}

QString ResourceManager::getImagePath(const QString &name)
{
    // 不使用资源文件，返回空字符串
    Q_UNUSED(name);
    return QString();
}

QPixmap ResourceManager::getHeroPixmap(HeroType type, HeroAction action, int frame)
{
    // 不使用资源文件，返回空的QPixmap
    Q_UNUSED(type);
    Q_UNUSED(action);
    Q_UNUSED(frame);
    return QPixmap();
} 