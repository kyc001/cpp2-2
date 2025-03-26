#ifndef SAVEFILE_H
#define SAVEFILE_H

#include <QString>
#include <QVector>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

// 存档处理类
class SaveFile {
public:
    SaveFile();
    
    // 加载和保存
    bool loadFromFile(const QString& filename);
    bool saveToFile(const QString& filename);
    
    // 导入和导出
    bool exportToFile(const QString& filename);
    bool importFromFile(const QString& filename);
    
    // 获取和设置数据
    int getTotalCoins() const;
    void setTotalCoins(int coins);
    
    QVector<int> getGlobalUpgrades() const;
    void setGlobalUpgrades(const QVector<int>& upgrades);
    
    // 获取和设置单个全局升级等级
    int getUpgradeLevel(int type) const;
    void setUpgradeLevel(int type, int level);
    
    // 获取和设置解锁的角色
    QVector<bool> getUnlockedCharacters() const;
    void setUnlockedCharacters(const QVector<bool>& unlocked);
    
    // 检查角色是否已解锁
    bool isCharacterUnlocked(int character_id) const;
    void unlockCharacter(int character_id);
    
    // 清除所有数据（重置）
    void clear();
    
private:
    int total_coins;                 // 总金币
    QVector<int> global_upgrades;    // 全局升级等级
    QVector<bool> unlocked_characters; // 解锁的角色
    
    // 将数据转换为JSON
    QJsonObject toJson() const;
    
    // 从JSON加载数据
    bool fromJson(const QJsonObject& json);
};

#endif // SAVEFILE_H 