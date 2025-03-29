#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <QPixmap>
#include <QMap>
#include <QString>
#include <QObject>

// 定义英雄和敌人类型枚举
enum class HeroType {
    WARRIOR = 0,
    MAGE,
    ARCHER,
    ROGUE
};

enum class HeroAction {
    IDLE = 0,
    WALK,
    ATTACK
};

class ResourceManager : public QObject
{
    Q_OBJECT

public:
    static ResourceManager &getInstance();

    // 资源加载与清理
    void loadResources();
    void clearResources();

    // UI资源
    QPixmap getUIResource(const QString &name);
    bool loadUIResources();
    QPixmap getButtonImage(const QString &style) const;
    QPixmap getIconImage(const QString &type) const;
    QPixmap getBarImage(const QString &type) const;
    QPixmap getBackgroundImage(const QString &type) const;
    QPixmap getFrameImage(const QString &style) const;

    // 地图资源
    QPixmap getMapResource(const QString &name);
    bool loadMapResources();
    QPixmap getFloorTile(const QString &type) const;
    QPixmap getObstacle(const QString &type) const;
    QPixmap getDecoration(const QString &type) const;
    QPixmap getMapPreset(const QString &type) const;

    // 敌人资源
    QPixmap getEnemyImage(const QString &type, const QString &animation, int frame);
    bool loadEnemyResources(int enemyType);
    QPixmap getEnemyIdleImage(int enemyType) const;
    QPixmap getEnemyWalkImage(int enemyType, int frame) const;
    QPixmap getEnemyAttackImage(int enemyType, int frame) const;
    QPixmap getEnemySpecialImage(int enemyType, int frame) const;
    QString enemyTypeToString(int enemyType) const;

    // 武器资源
    QPixmap getWeaponImage(const QString &type, const QString &animation, int frame);
    QPixmap getWeaponImage(int weaponType) const;
    QPixmap getWeaponEffectImage(int weaponType, int frame) const;
    bool loadWeaponResources(int weaponType);
    QString weaponTypeToString(int weaponType) const;

    // 英雄资源
    QPixmap getHeroImage(const QString &type, const QString &animation, int frame);
    bool loadHeroResources(int heroType);
    QPixmap getHeroIdleImage(int heroType) const;
    QPixmap getHeroWalkImage(int heroType, int frame) const;
    QPixmap getHeroAttackImage(int heroType, int frame) const;
    QString heroTypeToString(int heroType) const;

    // 通用资源方法
    QPixmap getPixmap(const QString &path) const;
    QPixmap getPixmap(const QString &name);
    QString getImagePath(const QString &name);
    bool loadPixmap(const QString &path, bool addToCache = true);
    QPixmap getHeroPixmap(HeroType type, HeroAction action, int frame = 0);

    // 在 ResourceManager 类的 public 部分添加以下方法
public:
    void loadEnemyResourcesByType(int enemyType)
    {
        // 调用 bool 类型的 loadEnemyResources(int) 函数
        loadEnemyResources(enemyType);
    }

private:
    // 单例模式
    static ResourceManager *instance;

    // 构造与析构
    explicit ResourceManager(QObject *parent = nullptr);
    virtual ~ResourceManager();

    // 资源加载辅助函数
    void loadEnemyResources(const QString &enemyType);
    void loadWeaponResources(const QString &weaponType);
    void loadHeroResources(const QString &heroType);

    // 资源缓存
    QMap<QString, QPixmap> ui_resources;     // UI资源
    QMap<QString, QPixmap> map_resources;    // 地图资源
    QMap<QString, QPixmap> enemy_resources;  // 敌人资源
    QMap<QString, QPixmap> weapon_resources; // 武器资源
    QMap<QString, QPixmap> hero_resources;   // 英雄资源
    QMap<QString, QPixmap> pixmapCache;      // 通用 Pixmap 缓存
};

#endif // RESOURCEMANAGER_H