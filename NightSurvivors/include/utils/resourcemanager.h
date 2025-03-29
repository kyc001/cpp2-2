#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <QPixmap>
#include <QMap>
#include <QString>

class ResourceManager {
public:
    static ResourceManager& getInstance();
    
    // 英雄资源加载
    bool loadHeroResources(int heroType);
    QPixmap getHeroIdleImage(int heroType) const;
    QPixmap getHeroWalkImage(int heroType, int frame) const;
    QPixmap getHeroAttackImage(int heroType, int frame) const;
    
    // 敌人资源加载
    bool loadEnemyResources(int enemyType);
    QPixmap getEnemyIdleImage(int enemyType) const;
    QPixmap getEnemyWalkImage(int enemyType, int frame) const;
    QPixmap getEnemyAttackImage(int enemyType, int frame) const;
    QPixmap getEnemySpecialImage(int enemyType, int frame) const;
    
    // 武器资源加载
    bool loadWeaponResources(int weaponType);
    QPixmap getWeaponImage(int weaponType) const;
    QPixmap getWeaponEffectImage(int weaponType, int frame) const;
    
    // 地图资源加载
    bool loadMapResources();
    QPixmap getFloorTile(const QString& type) const;
    QPixmap getObstacle(const QString& type) const;
    QPixmap getDecoration(const QString& type) const;
    QPixmap getMapPreset(const QString& type) const;
    
    // UI资源加载
    bool loadUIResources();
    QPixmap getButtonImage(const QString& style) const;
    QPixmap getIconImage(const QString& type) const;
    QPixmap getBarImage(const QString& type) const;
    QPixmap getBackgroundImage(const QString& type) const;
    QPixmap getFrameImage(const QString& style) const;
    
private:
    ResourceManager();  // 私有构造函数
    ~ResourceManager(); // 私有析构函数
    
    // 防止复制
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    
    // 资源缓存
    QMap<QString, QPixmap> pixmapCache;
    
    // 资源加载辅助函数
    bool loadPixmap(const QString& path, bool addToCache = true);
    QPixmap getPixmap(const QString& path) const;
    
    // 转换英雄类型到名称
    QString heroTypeToString(int heroType) const;
    
    // 转换敌人类型到名称
    QString enemyTypeToString(int enemyType) const;
    
    // 转换武器类型到名称
    QString weaponTypeToString(int weaponType) const;
};

#endif // RESOURCEMANAGER_H 