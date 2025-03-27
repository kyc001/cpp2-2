#include "bin/gamemap.h"

GameMap::GameMap(QObject *parent) : QObject(parent) {
    // Initialize with a 30x20 map
    map_width = 30;
    map_height = 20;
    
    // Initialize map with no barriers
    barriers.resize(map_height);
    for(int i = 0; i < map_height; i++) {
        barriers[i].resize(map_width, 0);
    }
    
    // Load barriers
    loadBarriers();
}

GameMap::~GameMap() {
    // Destructor
}

QVector<QVector<int>> GameMap::getBarries() const {
    return barriers;
}

void GameMap::mapLoading() {
    // Map loading logic
}

void GameMap::loadBarriers() {
    // Place at least 5 obstacles as required
    // 1 represents an obstacle
    
    // Obstacle 1: A small 2x2 block near the top left
    barriers[5][5] = 1;
    barriers[5][6] = 1;
    barriers[6][5] = 1;
    barriers[6][6] = 1;
    
    // Obstacle 2: A wall in the middle
    for(int i = 8; i < 12; i++) {
        barriers[10][i] = 1;
    }
    
    // Obstacle 3: Bottom right corner obstacle
    barriers[15][25] = 1;
    barriers[16][25] = 1;
    barriers[15][26] = 1;
    
    // Obstacle 4: Bottom left corner obstacle
    barriers[15][5] = 1;
    barriers[16][5] = 1;
    barriers[15][4] = 1;
    
    // Obstacle 5: Top right corner obstacle
    barriers[5][25] = 1;
    barriers[6][25] = 1;
    barriers[5][24] = 1;
}

int GameMap::getWidth() const {
    return map_width;
}

int GameMap::getHeight() const {
    return map_height;
}

bool GameMap::isBarrier(int x, int y) const {
    if(x < 0 || x >= map_width || y < 0 || y >= map_height) {
        return true; // Map boundaries are barriers
    }
    return barriers[y][x] == 1;
}
