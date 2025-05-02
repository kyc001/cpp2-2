#include "GameState.h"
#include <assert.h>
#include <iostream>

void GameState::initMap(unsigned int map_style) {
    _map = new GameMap(map_style);
    // 障碍物配置已舍弃，不再加载
}

void GameState::initHero(unsigned int hero_style) {
    player = new Hero((int)hero_style, parent, _map);
    player->setGame(this);
    player->giveWeapon();
    HERO_TYPE = hero_style;
    
    // 创建三个环绕玩家的悬浮球
    for (int i = 0; i < 3; i++) {
        floating_orbs.push_back(new FloatingOrb(player, this, i));
    }
}

std::vector<PaintInfo> GameState::paint() {
    std::vector<PaintInfo> buffer;

    std::vector<PaintInfo> temp;
    //地图的信息
    temp = _map->paint();
    buffer.reserve(temp.size());
    for(auto& item: temp){
        buffer.push_back(item);
    }

    temp = player->paint();
    for(auto& item: temp){
        buffer.push_back(item);
    }
    
    // 绘制悬浮球
    for(auto& orb : floating_orbs) {
        buffer.push_back(orb->paint());
    }

    for(auto& lst : enemies){
        for(auto& each : lst){
            if(each && each->isEnabled()) {
                std::vector<PaintInfo> temp_each = each->paint();
                for(auto& item : temp_each){
                    buffer.push_back(item);
                }
            }
        }
    }

    for(auto& each : exp_balls){
        if(each && each->isEnabled()){
            buffer.push_back(each->paint());
        }
    }

    return buffer;
}

GameState::GameState() {
    parent = nullptr;
    _map = nullptr;
    player = nullptr;
    enemy_control = nullptr;

    for(int i = 0; i < EBALL_MAX; i++){
        exp_balls.push_back(nullptr);
    }
    exp_cnt = 0;
    exp_max = EBALL_MAX;
}

GameState::GameState(QWidget *parent) :parent(parent){
    _map = nullptr;
    player = nullptr;
    enemy_control = nullptr;

    for(int i = 0; i < EBALL_MAX; i++){
        exp_balls.push_back(nullptr);
    }
    exp_cnt = 0;
    exp_max = EBALL_MAX;
}

GameState::~GameState() {
    delete _map;
    delete player;
    delete enemy_control;
    
    // 清理悬浮球
    for(auto& orb : floating_orbs){
        delete orb;
    }
    floating_orbs.clear();
}

void GameState::tick() {
    // std::cout << "掉落物数量" << exp_cnt << std::endl; // 暂时注释掉日志
    player->tick();
    
    // 暂时注释掉敌人控制器更新
    // if (enemy_control) {
    //     enemy_control->tick();
    // }
    
    // 更新悬浮球
    for(auto& orb : floating_orbs) {
        orb->tick();
    }
    
    // 暂时注释掉悬浮球与敌人的碰撞检测
    // checkOrbEnemyCollisions();
    
    // 暂时注释掉敌人自身的更新循环
    // for(auto& lst : enemies){
    //     for(auto& each : lst){
    //         if(each && each->isEnabled())
    //             each->tick();
    //     }
    // }

    // 经验球更新保留
    for(auto& each : exp_balls){
        if(each) {
            try {
                each->tick();
            }
            catch (TimeLimitExceeded &) {
                if (exp_cnt > 0) exp_cnt--; // 安全检查
            }
        }
    }

    // 暂时注释掉玩家与敌人的伤害判断
    // judgeDamageEnemies();
}

void GameState::checkOrbEnemyCollisions() {
    // 遍历所有悬浮球
    for (auto& orb : floating_orbs) {
        if (!orb) continue; // 安全检查
        
        // 遍历所有敌人类型
        for (auto& enemyList : enemies) {
            // 遍历每种类型的所有敌人
            for (auto& enemy : enemyList) {
                if (enemy && enemy->isEnabled()) {
                    try {
                        // 检查碰撞
                        orb->checkCollision(enemy);
                    } catch (std::exception& e) {
                        // 捕获可能的异常
                        std::cout << "碰撞检测异常: " << e.what() << std::endl;
                    }
                }
            }
        }
    }
}

void GameState::keyPressTick(QKeyEvent *event) {
    player->keyPressTick(event);
}

void GameState::keyReleaseTick(QKeyEvent *event) {
    player->keyReleaseTick(event);
}

void GameState::initEnemy(int stage) {
    std::cout << "[Log] GameState::initEnemy 进入函数内部" << std::endl;
    // 先清空现有enemies
    std::cout << "[Log] GameState::initEnemy: 清理旧敌人..." << std::endl;
    int outer_loop_count = 0;
    for (auto& enemyList : enemies) {
        std::cout << "[Log] GameState::initEnemy: 外层循环迭代 " << outer_loop_count << std::endl;
        int inner_loop_count = 0;
        for (auto& enemy : enemyList) {
            std::cout << "[Log] GameState::initEnemy:   内层循环迭代 " << inner_loop_count << ", enemy 指针: " << enemy << std::endl;
            if (enemy) {
                std::cout << "[Log] GameState::initEnemy:     尝试 delete enemy..." << std::endl;
                try {
                  delete enemy;
                  std::cout << "[Log] GameState::initEnemy:     delete enemy 成功" << std::endl;
                  enemy = nullptr;
                } catch (const std::exception& e) {
                    std::cout << "[Log] GameState::initEnemy:     delete enemy 时捕获异常: " << e.what() << std::endl;
                    // Consider how to handle this - maybe just set to nullptr and continue?
                    enemy = nullptr; 
                } catch (...) {
                    std::cout << "[Log] GameState::initEnemy:     delete enemy 时捕获未知异常!" << std::endl;
                    enemy = nullptr;
                }
            }
            inner_loop_count++;
        }
        std::cout << "[Log] GameState::initEnemy:   内层循环结束, 尝试 clear()" << std::endl;
        try {
            enemyList.clear();
            std::cout << "[Log] GameState::initEnemy:   clear() 成功" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "[Log] GameState::initEnemy:   clear() 时捕获异常: " << e.what() << std::endl;
        } catch (...) {
            std::cout << "[Log] GameState::initEnemy:   clear() 时捕获未知异常!" << std::endl;
        }
        outer_loop_count++;
    }
    std::cout << "[Log] GameState::initEnemy: 外层循环结束, 尝试清空主 enemies vector..." << std::endl;
    enemies.clear();
    std::cout << "[Log] GameState::initEnemy: 旧敌人清理完毕" << std::endl;
    
    // 检查阶段是否有效
    if (stage <= 0 || stage > NUM_OF_STAGES) {
        std::cout << "错误：无效的游戏阶段 " << stage << std::endl;
        return;
    }
    
    // 安全地初始化enemies数组，根据配置创建足够的容器
    try {
        std::cout << "[Log] GameState::initEnemy: 初始化敌人容器..." << std::endl;
        const StageInfo& stageInfo = STAGE_INFOS[stage - 1];
        std::cout << "初始化敌人阶段" << stage << "，敌人类型数: " << stageInfo.type_num << std::endl;
        
        // 确保敌人类型数量与定义相符
        if (stageInfo.type_num != NUM_OF_ENEMIES) {
            std::cout << "警告：NUM_OF_ENEMIES (" << NUM_OF_ENEMIES 
                      << ") 与 STAGE_INFOS[" << (stage-1) << "].type_num (" 
                      << stageInfo.type_num << ") 不匹配" << std::endl;
        }
        
        // 创建数组
        for (int i = 0; i < stageInfo.type_num; i++) {
            if (i < stageInfo.type_info.size()) {
                std::cout << "创建敌人类型 " << stageInfo.type_info[i].enemy_type 
                          << " 的容器，最大数量: " << stageInfo.type_info[i].max_nums << std::endl;
                enemies.push_back(std::vector<Enemy*>(stageInfo.type_info[i].max_nums, nullptr));
            } else {
                std::cout << "错误：敌人类型索引 " << i << " 超出了类型信息的范围" << std::endl;
                enemies.push_back(std::vector<Enemy*>());
            }
        }
        std::cout << "[Log] GameState::initEnemy: 敌人容器初始化完毕" << std::endl;
        
        // 创建敌人控制器
        std::cout << "[Log] GameState::initEnemy: 创建 EnemyController..." << std::endl;
        if (enemy_control) {
            std::cout << "[Log] GameState::initEnemy: 删除旧 EnemyController..." << std::endl;
            delete enemy_control;
        }
        enemy_control = new EnemyController(this, stage); // 调用 EnemyController 构造函数
        std::cout << "敌人控制器创建完成" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "初始化敌人时出现异常: " << e.what() << std::endl;
        if (enemies.empty()) {
            enemies.push_back(std::vector<Enemy*>());
        }
    }
    std::cout << "[Log] GameState::initEnemy 结束" << std::endl;
}

void GameState::judgeDamageEnemies() {
    if (!player) return; // 安全检查
    
    try {
        for(auto & type : enemies){
            for(auto & each : type){
                if (each && each->isEnabled()) {
                    try {
                        player->judgeDamage(each);
                    } catch (std::exception& e) {
                        // 捕获可能的异常
                        std::cout << "判断伤害异常: " << e.what() << std::endl;
                    }
                }
            }
        }
    } catch (std::exception& e) {
        // 捕获外层可能的异常
        std::cout << "判断伤害外层异常: " << e.what() << std::endl;
    } catch (...) {
        // 捕获所有其他异常
        std::cout << "判断伤害未知异常!" << std::endl;
    }
}

void GameState::addExpBall(double x, double y, int value) {
    if(exp_cnt >= exp_max){
        return;
    }
    for(auto & space : exp_balls){
        if(space == nullptr){
            EBEnableNullSpace(x,y,value,space);
            return;
        } else if(!space->isEnabled()){
            EBEnableUsedSpace(x,y,value, space);
            return;
        }
    }
}

void GameState::EBEnableNullSpace(double x, double y, int value, ExpBall * &space) {
    assert(space == nullptr);
    space = new ExpBall(x,y,value,_map);
    exp_cnt ++;
}

void GameState::EBEnableUsedSpace(double x, double y, int value, ExpBall * &space) {
    assert(space && !space->isEnabled());
    space->enable(x,y,value);
    exp_cnt ++;
}

int GameState::countExp(std::pair<double, double> player_pos) {
    int cnt = 0;
    // 计算玩家角色的中心点
    double player_center_x = player_pos.first + 30; // 假设角色宽度为60
    double player_center_y = player_pos.second + 30; // 假设角色高度为60
    
    for(auto & each : exp_balls) {
        if(each && each->isEnabled()) {
            // 计算经验球到玩家中心的距离
            double ball_center_x = each->real_pos.first + EBALL_SIZE/2.0;
            double ball_center_y = each->real_pos.second + EBALL_SIZE/2.0;
            
            // 计算距离平方（避免开平方根）
            double dx = ball_center_x - player_center_x;
            double dy = ball_center_y - player_center_y;
            double distance_squared = dx*dx + dy*dy;
            
            // 如果距离小于角色宽度的一半，认为可以吸收
            // 只有当距离很近时才吸收，而不是使用矩形碰撞检测
            double pickup_radius = 30.0; // 吸收半径
            if(distance_squared <= pickup_radius * pickup_radius) {
                cnt += each->getValue();
                each->disable();
                std::cout << "吸收经验球，位置: (" << ball_center_x << "," << ball_center_y 
                          << "), 距离: " << sqrt(distance_squared) << std::endl;
            }
        }
    }
    
    if(cnt > 0) {
    std::cout << "获得经验：" << cnt << std::endl;
    }
    return cnt;
}

void GameState::gameHalt() {
    std::cout << "游戏挂起" << std::endl;
    GAME_HALT = 1;
    HALT_HP = player->getHp();
    HALT_HPM = player->getHpMax();
    GAME_LEVEL = player->getLevel();
    HALT_EXP = player->getExp();
    HALT_EXPM = player->getExpMax();
    HERO_SPEED = player->getSpeed();
    HERO_REDUCE = player->getReduce();
    HERO_X = player->getRealX();
    HERO_Y = player->getRealY();
    DAMAGE = player->getDamage();
    HALT_CD = player->getCD();
}

