#ifndef SAVEFILE_H
#define SAVEFILE_H

#include <QString>
#include <QStringList>
#include <QObject>
#include <QDateTime>
#include <QVector>
#include <QJsonObject>

class GameState;

// 存档处理类
class SaveFile : public QObject {
    Q_OBJECT
    
public:
    explicit SaveFile(QObject* parent = nullptr);
    virtual ~SaveFile();
    
    // 自动存档和加载
    bool saveGame(GameState* gameState);
    bool loadGame(GameState* gameState);
    
    // 获取存档槽列表
    QStringList getSaveSlots();
    
    // 存档槽操作
    bool saveGameToSlot(GameState* gameState, int slot);
    bool loadGameFromSlot(GameState* gameState, int slot);
    bool deleteSaveSlot(int slot);
    
    // 获取存档元数据
    QString getSaveMetadata(int slot);
    
    // 获取游戏状态数据
    int getTotalCoins();
    QVector<int> getGlobalUpgrades();
    QVector<bool> getUnlockedCharacters();
    
    // 设置游戏状态数据
    void setTotalCoins(int totalCoins);
    void setGlobalUpgrades(const QVector<int>& upgrades);
    void setUnlockedCharacters(const QVector<bool>& characters);
    
    // 文件操作
    bool loadFromFile(const QString& filePath);
    bool saveToFile(const QString& filePath);
    
private:
    QJsonObject loadSaveData(int slot);
};

#endif // SAVEFILE_H 