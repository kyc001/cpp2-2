#include "GameMap.h"
#include <QString>
#include <fstream>
#include <string>
#include <iostream>

GameMap::GameMap(unsigned int map_style) : map_style(map_style){
    int target_height = 0;
    bool load_success = false;
    if(map_style == 1){
        load_success = map_bg.load(MAP_1_PATH);
        if (!load_success) {
            std::cerr << "[错误] GameMap: 无法加载地图背景图片: " << MAP_1_PATH << std::endl;
        }
        target_height = MAP_1_HEIGHT;
        lattice.first = MAP_1_LATTICE_X;
        lattice.second = MAP_1_LATTICE_Y;
    }
    pos_range.second = target_height;

    if (load_success) {
    map_bg = map_bg.scaledToHeight(pos_range.second);
    pos_range.first = map_bg.width();
    } else {
        // 加载失败，创建一个备用纯色背景
        pos_range.first = GAME_WIDTH; // 使用游戏窗口宽度作为备用宽度
        map_bg = QPixmap(pos_range.first, pos_range.second);
        map_bg.fill(Qt::darkGray); // 填充深灰色
        std::cout << "[警告] GameMap: 使用备用纯色背景" << std::endl;
    }

    absolute_pos.first = 0;
    absolute_pos.second = 0;

    // 防止除零错误
    if (lattice.first > 0) {
    lattice_scale.first = pos_range.first / lattice.first;
    } else {
         lattice_scale.first = 0;
    }
    if (lattice.second > 0) {
    lattice_scale.second = pos_range.second / lattice.second;
    } else {
         lattice_scale.second = 0;
    }
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
