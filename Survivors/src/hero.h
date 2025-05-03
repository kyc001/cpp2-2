[[nodiscard]] int getLevel() const { return level; }
[[nodiscard]] double get_hp_percent() const {
    int temp = hp;
    if(temp < 0){
        temp = 0;
    }
    return (double)temp / HP_MAX;
}

void upgrade(int type);

// 获取可用的升级选项描述
[[nodiscard]] QVector<QString> getUpgradeOptions() const;

[[nodiscard]] int getHpMax() const; 