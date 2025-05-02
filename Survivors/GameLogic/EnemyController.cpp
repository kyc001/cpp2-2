#include "EnemyController.h"
#include "GameState.h"
#include "Enemy.h"
#include "config.h"
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <cmath>
#include <QMutexLocker>

EnemyController::EnemyController(GameState *g, int stage) : game(g), game_stage(stage){
    std::cout << "[Log] EnemyController 构造函数开始, stage: " << stage << std::endl;
    StageInfo this_stage = STAGE_INFOS[stage - 1];
    type_num = this_stage.type_num;

    int vector_pos = 0;
    for(auto& each : this_stage.type_info){
        std::cout << "[Log] EnemyController 处理敌人类型: " << each.enemy_type << std::endl;
        TypeController temp = {};
        temp.max_cds = each.cd_max;
        temp.enemy_type = each.enemy_type;
        temp.max_nums = each.max_nums;
        temp.num_counters = 0;
        temp.cds = each.cd_max;
        temp.min_cds = each.cd_min;
        temp.reduce_cds = each.reduce_rate;
        temp.reduce_counters = each.reduce_rate;
        temp.vector_pos = vector_pos;
        types.push_back(temp);
        vector_pos ++;
    }

    enemy_death_cnt = 0;

    srand(time(nullptr));
    std::cout << "[Log] EnemyController 构造函数结束" << std::endl;
}

void EnemyController::addEnemy(int enemy_type, std::vector<Enemy *> &buffer) {
    std::cout << "[Log] EnemyController::addEnemy 开始, 类型: " << enemy_type << std::endl;
    
    // 使用互斥锁保护敌人创建过程
    QMutexLocker locker(&enemy_mutex);
    
    for (auto & space : buffer) {
        if (space == nullptr) {
            enableNullSpace(enemy_type, space);
            std::cout << "[Log] EnemyController::addEnemy 结束 (空槽)" << std::endl;
            return;
        } else if (!space->isEnabled()) {
            enableUsedSpace(enemy_type, space);
            std::cout << "[Log] EnemyController::addEnemy 结束 (重用)" << std::endl;
            return;
        }
    }
    std::cout << "[Log] EnemyController::addEnemy 结束 (无可用槽)" << std::endl;
}

void EnemyController::reportDeath(int type) {
    bool found = false;
    for(auto & each : types){
        if(each.enemy_type == type) {
            // 添加安全检查，确保计数器不会变为负数
            if(each.num_counters > 0) {
                each.num_counters--;
                found = true;
            } else {
                std::cout << "警告: 敌人类型 " << type << " 的计数器已经为0，无法再减少" << std::endl;
            }
            break;
        }
    }

    // 只有在成功减少计数器时才增加死亡计数
    if (found) {
        switch(type){
            case 1:
                enemy_death_cnt++;
                break;
            case 2:
                enemy_death_cnt++;
                break;
            case 3:
                enemy_death_cnt += 2;
                break;
            default:
                std::cout << "警告: 未知的敌人类型 " << type << std::endl;
                break;
        }
    }
}

void EnemyController::enableNullSpace(int enemy_type, Enemy *&space) {
    std::cout << "[Log] EnemyController::enableNullSpace 开始, 类型: " << enemy_type << std::endl;
    
    try {
        // 检查参数有效性
        if (enemy_type < 1 || enemy_type > NUM_OF_ENEMIES) {
            std::cout << "错误：敌人类型 " << enemy_type << " 超出支持范围 (1-" << NUM_OF_ENEMIES << ")" << std::endl;
            return;
        }
        
        // 检查游戏状态有效性
        if (!game || !game->_map || !game->player) {
            std::cout << "错误：游戏状态无效，无法创建敌人" << std::endl;
            return;
        }
        
        // 生成位置并确保位置有效
        std::pair<double, double> real_pos = generatePosition();
        if (real_pos.first < 0 || real_pos.second < 0 || 
            real_pos.first > game->_map->getPosRangeX() || 
            real_pos.second > game->_map->getPosRangeY()) {
            // 修正无效位置
            real_pos.first = std::max(0.0, std::min(real_pos.first, (double)game->_map->getPosRangeX()));
            real_pos.second = std::max(0.0, std::min(real_pos.second, (double)game->_map->getPosRangeY()));
            std::cout << "警告：敌人位置超出范围，已修正为 (" << real_pos.first << "," << real_pos.second << ")" << std::endl;
        }
        
        std::cout << "正在创建类型: " << enemy_type << " 的敌人, 坐标: " << real_pos.first << "," << real_pos.second << std::endl;
        
        // 根据敌人类型创建不同敌人
        switch(enemy_type) {
            case 1:
                space = new NoWeaponEnemy(enemy_type, game->parent, this, game->_map,
                                         game->player, real_pos.first, real_pos.second);
                break;
            case 2:
                space = new NoWeaponEnemy(enemy_type, game->parent, this, game->_map,
                                        game->player, real_pos.first, real_pos.second);
                break;
            case 3:
                space = new NoWeaponEnemy(enemy_type, game->parent, this, game->_map,
                                        game->player, real_pos.first, real_pos.second);
                break;
            default:
                std::cout << "警告：尝试创建未处理的敌人类型 " << enemy_type << "，将创建类型 1 代替。" << std::endl;
                space = new NoWeaponEnemy(1, game->parent, this, game->_map,
                                        game->player, real_pos.first, real_pos.second);
                break;
        }

        // 验证敌人创建成功
        if (space) {
            std::cout << "敌人" << enemy_type << "创建完成，图片状态: " << (space->_image.isNull() ? "空" : "有效") << std::endl;
            
            // 确保敌人处于游戏地图边缘（改进生成逻辑）
            double distanceFromCenter = std::sqrt(
                std::pow(real_pos.first - game->player->getRealX(), 2) + 
                std::pow(real_pos.second - game->player->getRealY(), 2)
            );
            
            if (distanceFromCenter < 300) {
                std::cout << "警告：敌人生成位置过于靠近玩家，已调整位置" << std::endl;
                // 重新调整位置以确保不会太靠近玩家
                std::pair<double, double> adjusted_pos = generatePosition();
                space->setRealPosition(adjusted_pos.first, adjusted_pos.second);
            }
            
            // 启用敌人
            space->enable();
            std::cout << "敌人已启用: enemy_type=" << enemy_type << ", enabled=" << space->isEnabled() << ", alive=" << space->alive << std::endl;
        } else {
            std::cout << "错误：创建敌人类型 " << enemy_type << " 失败！" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "创建敌人时发生异常: " << e.what() << std::endl;
    } catch (...) {
        std::cout << "创建敌人时发生未知异常!" << std::endl;
    }
    
    std::cout << "[Log] EnemyController::enableNullSpace 结束" << std::endl;
}

void EnemyController::enableUsedSpace(int enemy_type, Enemy *&space) {
    std::cout << "[Log] EnemyController::enableUsedSpace 开始, 类型: " << enemy_type << std::endl;
    std::pair<double, double> real_pos = generatePosition();
    space->setRealPosition(real_pos.first, real_pos.second);
    space->enable();
    std::cout << "[Log] EnemyController::enableUsedSpace 结束" << std::endl;
}

std::pair<double, double> EnemyController::generatePosition() {
    if (!game || !game->_map) {
        std::cout << "警告：无效的游戏状态或地图，使用默认位置" << std::endl;
        return {0, 0};
    }
    
    // 检查是否能生成安全位置
    std::pair<double, double> safePos = generatePositionAwayFromPlayer(300);
    if (isPositionSafe(safePos.first, safePos.second)) {
        return safePos;
    }
    
    // 原来的位置生成逻辑，作为备用
    bool left_or_right;
    bool up_or_down;
    
    if (game->_map->getAbsoluteX() > 0) {
        left_or_right = true;
    } else if (game->_map->getAbsoluteX() + game->_map->getPosRangeX() <= GAME_WIDTH) {
        left_or_right = false;
    } else {
        left_or_right = rand() % 2;
    }

    if (game->_map->getAbsoluteY() > 0) {
        up_or_down = true;
    } else if (game->_map->getAbsoluteY() + game->_map->getPosRangeY() <= GAME_HEIGHT) {
        up_or_down = false;
    } else {
        up_or_down = rand() % 2;
    }

    double x;
    double y;

    if (left_or_right) {
        x = (double)(0 - game->_map->getAbsoluteX() + GAME_WIDTH);
    } else {
        x = (double)(0 - game->_map->getAbsoluteX());
    }

    if (up_or_down) {
        y = (double)(0 - game->_map->getAbsoluteY() + GAME_HEIGHT);
    } else {
        y = (double)(0 - game->_map->getAbsoluteY());
    }

    // 确保坐标在地图范围内
    x = std::max(0.0, std::min(x, (double)game->_map->getPosRangeX()));
    y = std::max(0.0, std::min(y, (double)game->_map->getPosRangeY()));

    return {x, y};
}

void EnemyController::tick() {
    //先处理reduce_cd
    for(auto & each : types){
        if(each.reduce_counters == 0 && each.max_cds > each.min_cds){
            each.max_cds --;
            each.reduce_counters = each.reduce_cds;
        } else if(each.reduce_counters == 0){
            each.reduce_counters = each.reduce_cds;
        } else {
            each.reduce_counters --;
        }
    }

    //再处理敌人生成
    for(auto & each : types){
        // 无论数量是否已满，只要CD没到0，就减少CD
        if (each.cds > 0) { 
            each.cds--;     
        }

        // 现在检查是否生成
        if(each.cds <= 0 && each.num_counters < each.max_nums){ // CD到了且数量未满
            // 检查enemies数组的索引是否有效
            if(each.vector_pos >= 0 && each.vector_pos < game->enemies.size()) {
                addEnemy(each.enemy_type, game->enemies[each.vector_pos]);
                each.num_counters ++;
                each.cds = each.max_cds; // 重置CD
            } else {
                std::cout << "警告: 无效的enemies数组索引: " << each.vector_pos 
                          << ", 总大小: " << game->enemies.size() << std::endl;
            }
        }
        // 不再需要 else if 分支，因为CD递减已在前面处理
    }
}

void EnemyController::reportDeath(int type, double rx, double ry) {
    bool found = false;
    for(auto & each : types){
        if(each.enemy_type == type) {
            // 添加安全检查，确保计数器不会变为负数
            if(each.num_counters > 0) {
                each.num_counters--;
                found = true;
            } else {
                std::cout << "警告: 敌人类型 " << type << " 的计数器已经为0，无法再减少" << std::endl;
            }
            break;
        }
    }

    // 只有在找到对应类型且成功减少计数器时才添加经验球
    if(found) {
    int value;
    switch(type){
        case 1:
                enemy_death_cnt++;
            value = 10;
            break;
        case 2:
                enemy_death_cnt++;
            value = 10;
            break;
        case 3:
            enemy_death_cnt += 2;
            value = 20;
            break;
            default:
                value = 5; // 默认经验值
                break;
        }
        game->addExpBall(rx, ry, value);
    }
}

// 生成一个远离玩家的安全位置
std::pair<double, double> EnemyController::generateSafePosition() {
    // 尝试最多10次找到有效位置
    for (int attempt = 0; attempt < 10; attempt++) {
        std::pair<double, double> pos = generatePositionAwayFromPlayer();
        if (isPositionSafe(pos.first, pos.second)) {
            return pos;
        }
    }
    
    // 如果无法找到理想位置，回退到基本的生成逻辑
    return generatePosition();
}

// 生成远离玩家一定距离的位置
std::pair<double, double> EnemyController::generatePositionAwayFromPlayer(double minDistance) {
    if (!game || !game->player || !game->_map) {
        // 回退到基本生成逻辑
        std::cout << "[Log] 回退到基本生成逻辑，因为game或player或map为null" << std::endl;
        return generatePosition();
    }
    
    // 获取玩家位置
    double playerX = game->player->getRealX();
    double playerY = game->player->getRealY();
    
    std::cout << "[Log] 玩家位置: (" << playerX << "," << playerY << ")" << std::endl;
    
    // 获取地图尺寸
    double mapWidth = game->_map->getPosRangeX();
    double mapHeight = game->_map->getPosRangeY();
    
    std::cout << "[Log] 地图尺寸: " << mapWidth << "x" << mapHeight << std::endl;
    
    // 尝试生成一个远离玩家的位置
    double x, y;
    double distance = 0;
    int attempts = 0;
    
    // 尝试最多10次找到合适的位置
    while (distance < minDistance && attempts < 10) {
        // 随机选择一个屏幕边缘作为生成点
        int edge = rand() % 4; // 0=上, 1=右, 2=下, 3=左
        
        // 根据选定的边缘生成坐标
        switch (edge) {
            case 0: // 上边缘
                x = playerX + (rand() % 1000 - 500);
                y = playerY - (minDistance + rand() % 300);
                break;
            case 1: // 右边缘
                x = playerX + (minDistance + rand() % 300);
                y = playerY + (rand() % 1000 - 500);
                break;
            case 2: // 下边缘
                x = playerX + (rand() % 1000 - 500);
                y = playerY + (minDistance + rand() % 300);
                break;
            case 3: // 左边缘
                x = playerX - (minDistance + rand() % 300);
                y = playerY + (rand() % 1000 - 500);
                break;
        }
        
        // 确保坐标在地图范围内
        x = std::max(0.0, std::min(x, mapWidth));
        y = std::max(0.0, std::min(y, mapHeight));
        
        // 计算与玩家的距离
        double dx = x - playerX;
        double dy = y - playerY;
        distance = std::sqrt(dx*dx + dy*dy);
        
        attempts++;
        std::cout << "[Log] 生成位置尝试 #" << attempts << ": (" << x << "," << y << "), 距离: " << distance << std::endl;
    }
    
    std::cout << "[Log] 最终生成位置: (" << x << "," << y << "), 距离: " << distance << std::endl;
    return {x, y};
}

// 检查位置是否在地图有效范围内
bool EnemyController::isPositionValid(double x, double y) {
    if (!game || !game->_map) return false;
    
    // 检查坐标是否在地图范围内
    if (x < 0 || y < 0 || 
        x > game->_map->getPosRangeX() || 
        y > game->_map->getPosRangeY()) {
        return false;
    }
    
    return true;
}

// 检查位置是否安全（不在障碍物内且不太靠近玩家）
bool EnemyController::isPositionSafe(double x, double y) {
    if (!isPositionValid(x, y)) return false;
    
    if (!game || !game->player) return true;
    
    // 检查与玩家的距离
    double playerX = game->player->getRealX();
    double playerY = game->player->getRealY();
    double dx = x - playerX;
    double dy = y - playerY;
    double distance = std::sqrt(dx*dx + dy*dy);
    
    // 至少保持200像素距离
    if (distance < 200) {
        return false;
    }
    
    // TODO: 如果有障碍物系统，还可以检查是否在障碍物内
    
    return true;
}
