#include "../../include/utils/savefile.h"
#include <QFile>
#include <QDir>

SaveFile::SaveFile() : total_coins(0) {
    // 初始化全局升级数组，默认所有升级都是0级
    global_upgrades.resize(10, 0);  // 预留10种升级类型
    
    // 初始化角色解锁状态，默认第一个角色解锁，其他锁定
    unlocked_characters.resize(5, false);  // 预留5个角色
    unlocked_characters[0] = true;  // 默认第一个角色已解锁
}

bool SaveFile::loadFromFile(const QString& filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    
    QByteArray saveData = file.readAll();
    file.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(saveData);
    if (doc.isNull() || !doc.isObject()) {
        return false;
    }
    
    return fromJson(doc.object());
}

bool SaveFile::saveToFile(const QString& filename) {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    
    QJsonObject saveObj = toJson();
    QJsonDocument doc(saveObj);
    file.write(doc.toJson());
    file.close();
    
    return true;
}

bool SaveFile::exportToFile(const QString& filename) {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    
    // 构建导出文本格式
    QString exportText = "NightSurvivors_SaveData\n";
    exportText += QString("TotalCoins:%1\n").arg(total_coins);
    
    // 添加全局升级
    exportText += "GlobalUpgrades:";
    for (int i = 0; i < global_upgrades.size(); i++) {
        exportText += QString("%1").arg(global_upgrades[i]);
        if (i < global_upgrades.size() - 1) {
            exportText += ",";
        }
    }
    exportText += "\n";
    
    // 添加角色解锁状态
    exportText += "UnlockedCharacters:";
    for (int i = 0; i < unlocked_characters.size(); i++) {
        exportText += unlocked_characters[i] ? "1" : "0";
        if (i < unlocked_characters.size() - 1) {
            exportText += ",";
        }
    }
    exportText += "\n";
    
    file.write(exportText.toUtf8());
    file.close();
    
    return true;
}

bool SaveFile::importFromFile(const QString& filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }
    
    QString content = file.readAll();
    file.close();
    
    QStringList lines = content.split("\n");
    if (lines.size() < 4 || lines[0] != "NightSurvivors_SaveData") {
        return false; // 无效的存档文件
    }
    
    // 解析总金币
    QString coinsLine = lines[1];
    if (!coinsLine.startsWith("TotalCoins:")) {
        return false;
    }
    total_coins = coinsLine.mid(11).toInt();
    
    // 解析全局升级
    QString upgradesLine = lines[2];
    if (!upgradesLine.startsWith("GlobalUpgrades:")) {
        return false;
    }
    QStringList upgradesStr = upgradesLine.mid(14).split(",");
    global_upgrades.clear();
    for (const QString& upgrade : upgradesStr) {
        global_upgrades.append(upgrade.toInt());
    }
    
    // 解析角色解锁状态
    QString charactersLine = lines[3];
    if (!charactersLine.startsWith("UnlockedCharacters:")) {
        return false;
    }
    QStringList charactersStr = charactersLine.mid(18).split(",");
    unlocked_characters.clear();
    for (const QString& character : charactersStr) {
        unlocked_characters.append(character == "1");
    }
    
    return true;
}

int SaveFile::getTotalCoins() const {
    return total_coins;
}

void SaveFile::setTotalCoins(int coins) {
    total_coins = coins;
}

QVector<int> SaveFile::getGlobalUpgrades() const {
    return global_upgrades;
}

void SaveFile::setGlobalUpgrades(const QVector<int>& upgrades) {
    global_upgrades = upgrades;
}

int SaveFile::getUpgradeLevel(int type) const {
    if (type >= 0 && type < global_upgrades.size()) {
        return global_upgrades[type];
    }
    return 0;
}

void SaveFile::setUpgradeLevel(int type, int level) {
    if (type >= 0 && type < global_upgrades.size()) {
        global_upgrades[type] = level;
    }
}

QVector<bool> SaveFile::getUnlockedCharacters() const {
    return unlocked_characters;
}

void SaveFile::setUnlockedCharacters(const QVector<bool>& unlocked) {
    unlocked_characters = unlocked;
}

bool SaveFile::isCharacterUnlocked(int character_id) const {
    if (character_id >= 0 && character_id < unlocked_characters.size()) {
        return unlocked_characters[character_id];
    }
    return false;
}

void SaveFile::unlockCharacter(int character_id) {
    if (character_id >= 0 && character_id < unlocked_characters.size()) {
        unlocked_characters[character_id] = true;
    }
}

void SaveFile::clear() {
    total_coins = 0;
    
    // 重置全局升级
    for (int i = 0; i < global_upgrades.size(); i++) {
        global_upgrades[i] = 0;
    }
    
    // 重置角色解锁状态，保持第一个角色解锁
    for (int i = 0; i < unlocked_characters.size(); i++) {
        unlocked_characters[i] = (i == 0);
    }
}

QJsonObject SaveFile::toJson() const {
    QJsonObject json;
    
    // 保存总金币
    json["total_coins"] = total_coins;
    
    // 保存全局升级
    QJsonArray upgradesArray;
    for (int level : global_upgrades) {
        upgradesArray.append(level);
    }
    json["global_upgrades"] = upgradesArray;
    
    // 保存角色解锁状态
    QJsonArray charactersArray;
    for (bool unlocked : unlocked_characters) {
        charactersArray.append(unlocked);
    }
    json["unlocked_characters"] = charactersArray;
    
    return json;
}

bool SaveFile::fromJson(const QJsonObject& json) {
    // 确保所需的键存在
    if (!json.contains("total_coins") || !json.contains("global_upgrades") || 
        !json.contains("unlocked_characters")) {
        return false;
    }
    
    // 加载总金币
    total_coins = json["total_coins"].toInt();
    
    // 加载全局升级
    QJsonArray upgradesArray = json["global_upgrades"].toArray();
    global_upgrades.clear();
    for (const QJsonValue& value : upgradesArray) {
        global_upgrades.append(value.toInt());
    }
    
    // 加载角色解锁状态
    QJsonArray charactersArray = json["unlocked_characters"].toArray();
    unlocked_characters.clear();
    for (const QJsonValue& value : charactersArray) {
        unlocked_characters.append(value.toBool());
    }
    
    return true;
} 
