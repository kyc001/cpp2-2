#include "GameMap.h"
#include <QString>
#include <fstream>
#include <string>

GameMap::GameMap(unsigned int map_style) : map_style(map_style){
    int target_height = 0;
    if(map_style == 1){
        map_bg.load(MAP_1_PATH);
        target_height = MAP_1_HEIGHT;
        lattice.first = MAP_1_LATTICE_X;
        lattice.second = MAP_1_LATTICE_Y;
    }
    pos_range.second = target_height;
    map_bg = map_bg.scaledToHeight(pos_range.second);
    pos_range.first = map_bg.width();
    absolute_pos.first = 0;
    absolute_pos.second = 0;

    lattice_scale.first = pos_range.first / lattice.first;
    lattice_scale.second = pos_range.second / lattice.second;
}

std::vector<PaintInfo> GameMap::paint() {
    std::vector<PaintInfo> buffer;
    buffer.emplace_back(map_bg,absolute_pos.first,absolute_pos.second);
    return buffer;
}

int GameMap::getPosRangeX() {
    return pos_range.first;
}

int GameMap::getPosRangeY() {
    return pos_range.second;
}

void GameMap::setAbsolutePos(int x, int y) {
    absolute_pos.first = x;
    absolute_pos.second = y;
}

void GameMap::loadBarriers() {
    // 障碍物配置已被舍弃，此方法保留但不执行实际操作
}

bool GameMap::checkPosition(const QRect & object) {
    // 由于舍弃障碍物配置，只检查地图边界
    if(object.x() + object.width() > pos_range.first){
        return false;
    }
    if(object.y() + object.height() > pos_range.second){
        return false;
    }
    if(object.x() < 0 || object.y() < 0){
        return false;
    }
    return true;
}
