#include "../../include/utils/savefile.h"
#include "../../include/core/gamestate.h"
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

SaveFile::SaveFile(QObject* parent) : QObject(parent)
{
    // 创建存档目录
    QDir dir(QDir::homePath() + "/.nightsurvivors");
    if (!dir.exists()) {
        dir.mkpath(".");
    }
}

SaveFile::~SaveFile()
{
}

bool SaveFile::saveGame(GameState* gameState)
{
    // 默认保存到slot 0
    return saveGameToSlot(gameState, 0);
}

bool SaveFile::loadGame(GameState* gameState)
{
    // 默认从slot 0加载
    return loadGameFromSlot(gameState, 0);
}

QStringList SaveFile::getSaveSlots()
{
    QStringList result;
    
    QDir dir(QDir::homePath() + "/.nightsurvivors");
    QStringList filters;
    filters << "save_*.dat";
    dir.setNameFilters(filters);
    
    QStringList files = dir.entryList(filters, QDir::Files, QDir::Name);
    for (const QString& file : files) {
        // 从文件名中提取槽位号
        int slot = file.mid(5, file.length() - 9).toInt();
        result << QString::number(slot);
    }
    
    return result;
}

bool SaveFile::saveGameToSlot(GameState* gameState, int slot)
{
    if (!gameState) {
        return false;
    }
    
    QJsonObject saveData;
    
    // 保存基本游戏信息
    saveData["total_coins"] = gameState->getTotalCoins();
    saveData["game_score"] = gameState->getScore();
    
    // 保存全局升级信息
    QJsonArray upgradesArray;
    QVector<int> upgrades = gameState->getGlobalUpgrades();
    for (int upgrade : upgrades) {
        upgradesArray.append(upgrade);
    }
    saveData["global_upgrades"] = upgradesArray;
    
    // 保存角色解锁状态
    QJsonArray charactersArray;
    QVector<bool> characters = gameState->getUnlockedCharacters();
    for (bool unlocked : characters) {
        charactersArray.append(unlocked);
    }
    saveData["unlocked_characters"] = charactersArray;
    
    // 将数据写入文件
    QString saveFileName = QDir::homePath() + "/.nightsurvivors/save_" + QString::number(slot) + ".dat";
    QFile saveFile(saveFileName);
    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning() << "无法打开存档文件进行写入:" << saveFileName;
        return false;
    }
    
    QJsonDocument saveDoc(saveData);
    saveFile.write(saveDoc.toJson());
    
    return true;
}

bool SaveFile::loadGameFromSlot(GameState* gameState, int slot)
{
    if (!gameState) {
        return false;
    }
    
    QString saveFileName = QDir::homePath() + "/.nightsurvivors/save_" + QString::number(slot) + ".dat";
    QFile saveFile(saveFileName);
    if (!saveFile.open(QIODevice::ReadOnly)) {
        qWarning() << "无法打开存档文件进行读取:" << saveFileName;
        return false;
    }
    
    QByteArray saveData = saveFile.readAll();
    QJsonDocument saveDoc = QJsonDocument::fromJson(saveData);
    QJsonObject saveObj = saveDoc.object();
    
    // 读取基本游戏信息
    gameState->setTotalCoins(saveObj["total_coins"].toInt());
    // 可以添加其他信息...
    
    return true;
}

bool SaveFile::deleteSaveSlot(int slot)
{
    QString saveFileName = QDir::homePath() + "/.nightsurvivors/save_" + QString::number(slot) + ".dat";
    QFile saveFile(saveFileName);
    if (saveFile.exists()) {
        return saveFile.remove();
    }
    return true; // 如果文件不存在，认为删除成功
}

QString SaveFile::getSaveMetadata(int slot)
{
    QString saveFileName = QDir::homePath() + "/.nightsurvivors/save_" + QString::number(slot) + ".dat";
    QFile saveFile(saveFileName);
    if (!saveFile.open(QIODevice::ReadOnly)) {
        return QString();
    }
    
    QByteArray saveData = saveFile.readAll();
    QJsonDocument saveDoc = QJsonDocument::fromJson(saveData);
    QJsonObject saveObj = saveDoc.object();
    
    // 返回一些简单的元数据
    int totalCoins = saveObj["total_coins"].toInt();
    int gameScore = saveObj["game_score"].toInt();
    
    return QString("Coins: %1 | Score: %2").arg(totalCoins).arg(gameScore);
}

// 以下是新增加的方法，用于解决gamestate.cpp中的报错

int SaveFile::getTotalCoins()
{
    // 从配置中读取总金币数
    QJsonObject saveData = loadSaveData(0);
    return saveData["total_coins"].toInt(0);
}

QVector<int> SaveFile::getGlobalUpgrades()
{
    QVector<int> result;
    QJsonObject saveData = loadSaveData(0);
    QJsonArray upgradesArray = saveData["global_upgrades"].toArray();
    
    for (const QJsonValue& value : upgradesArray) {
        result.append(value.toInt());
    }
    
    // 确保至少有5个升级槽位
    if (result.size() < 5) {
        result.resize(5, 0);
    }
    
    return result;
}

QVector<bool> SaveFile::getUnlockedCharacters()
{
    QVector<bool> result;
    QJsonObject saveData = loadSaveData(0);
    QJsonArray charactersArray = saveData["unlocked_characters"].toArray();
    
    for (const QJsonValue& value : charactersArray) {
        result.append(value.toBool());
    }
    
    // 确保至少有4个角色槽位，第一个角色默认解锁
    if (result.size() < 4) {
        result.resize(4, false);
        result[0] = true;
    }
    
    return result;
}

QJsonObject SaveFile::loadSaveData(int slot)
{
    QString saveFileName = QDir::homePath() + "/.nightsurvivors/save_" + QString::number(slot) + ".dat";
    QFile saveFile(saveFileName);
    if (!saveFile.open(QIODevice::ReadOnly)) {
        // 文件不存在，返回空对象
        return QJsonObject();
    }
    
    QByteArray saveData = saveFile.readAll();
    QJsonDocument saveDoc = QJsonDocument::fromJson(saveData);
    return saveDoc.object();
}

bool SaveFile::loadFromFile(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    
    // 假设文件存在且读取成功
    file.close();
    return true;
}

void SaveFile::setTotalCoins(int totalCoins)
{
    QJsonObject saveData = loadSaveData(0);
    saveData["total_coins"] = totalCoins;
    
    // 保存回文件
    QString saveFileName = QDir::homePath() + "/.nightsurvivors/save_0.dat";
    QFile saveFile(saveFileName);
    if (saveFile.open(QIODevice::WriteOnly)) {
        QJsonDocument saveDoc(saveData);
        saveFile.write(saveDoc.toJson());
    }
}

void SaveFile::setGlobalUpgrades(const QVector<int>& upgrades)
{
    QJsonObject saveData = loadSaveData(0);
    
    QJsonArray upgradesArray;
    for (int upgrade : upgrades) {
        upgradesArray.append(upgrade);
    }
    saveData["global_upgrades"] = upgradesArray;
    
    // 保存回文件
    QString saveFileName = QDir::homePath() + "/.nightsurvivors/save_0.dat";
    QFile saveFile(saveFileName);
    if (saveFile.open(QIODevice::WriteOnly)) {
        QJsonDocument saveDoc(saveData);
        saveFile.write(saveDoc.toJson());
    }
}

void SaveFile::setUnlockedCharacters(const QVector<bool>& characters)
{
    QJsonObject saveData = loadSaveData(0);
    
    QJsonArray charactersArray;
    for (bool unlocked : characters) {
        charactersArray.append(unlocked);
    }
    saveData["unlocked_characters"] = charactersArray;
    
    // 保存回文件
    QString saveFileName = QDir::homePath() + "/.nightsurvivors/save_0.dat";
    QFile saveFile(saveFileName);
    if (saveFile.open(QIODevice::WriteOnly)) {
        QJsonDocument saveDoc(saveData);
        saveFile.write(saveDoc.toJson());
    }
}

bool SaveFile::saveToFile(const QString& filePath)
{
    QJsonObject saveData = loadSaveData(0);
    
    QFile saveFile(filePath);
    if (!saveFile.open(QIODevice::WriteOnly)) {
        return false;
    }
    
    QJsonDocument saveDoc(saveData);
    saveFile.write(saveDoc.toJson());
    saveFile.close();
    
    return true;
} 

