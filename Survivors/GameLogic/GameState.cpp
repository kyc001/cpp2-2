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
    std::cout << "[Log] GameState 构造函数开始" << std::endl;
    _map = nullptr;
    player = nullptr;
    enemy_control = nullptr;
    std::cout << "[Log] GameState: 指针初始化完毕" << std::endl;

    // --- 使用 assign 直接初始化 --- 
    try {
        std::cout << "[Log] GameState: 尝试使用 assign 初始化 exp_balls 大小为 " << EBALL_MAX << std::endl;
        exp_balls.assign(EBALL_MAX, nullptr); // 直接创建 EBALL_MAX 个 nullptrs
        std::cout << "[Log] GameState: exp_balls 使用 assign 初始化完毕" << std::endl;
    } catch (const std::bad_alloc& e) {
        std::cerr << "[致命错误] GameState: exp_balls 初始化内存分配失败: " << e.what() << std::endl;
        // 可以考虑更优雅地处理错误，例如抛出自定义异常或设置错误状态
        throw; // 重新抛出，表明构造失败
    } catch (const std::exception& e) {
        std::cerr << "[错误] GameState: exp_balls 初始化时发生异常: " << e.what() << std::endl;
        throw; // 重新抛出
    }

    exp_cnt = 0;
    exp_max = EBALL_MAX;
    std::cout << "[Log] GameState 构造函数结束" << std::endl;
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
    try {
        // 1. 更新玩家状态
        if (player) {
            player->tick();
        }
        
        // 2. 更新敌人控制器
        if (enemy_control) {
            try {
                enemy_control->tick();
            } catch (const std::exception& e) {
                std::cout << "敌人控制器更新时发生异常: " << e.what() << std::endl;
            } catch (...) {
                std::cout << "敌人控制器更新时发生未知异常" << std::endl;
            }
        }
        
        // 3. 更新悬浮球
        for (auto& orb : floating_orbs) {
            if (orb) {
                orb->tick();
            }
        }
        
        // 4. 悬浮球与敌人的碰撞检测
        try {
            checkOrbEnemyCollisions();
        } catch (const std::exception& e) {
            std::cout << "悬浮球与敌人碰撞检测时发生异常: " << e.what() << std::endl;
        }
        
        // 5. 更新所有敌人
        for (auto& lst : enemies) {
            for (auto& each : lst) {
                if (each && each->isEnabled()) {
                    try {
                        each->tick();
                    } catch (const std::exception& e) {
                        std::cout << "敌人更新时发生异常: " << e.what() << std::endl;
                        // 发生异常时禁用该敌人，防止持续异常
                        each->disable();
                    }
                }
            }
        }
        
        // 6. 判断玩家与敌人的伤害
        try {
            judgeDamageEnemies();
        } catch (const std::exception& e) {
            std::cout << "判断伤害时发生异常: " << e.what() << std::endl;
        }
        
        // 7. 更新所有经验球
        for (auto& each : exp_balls) {
            if (each && each->isEnabled()) {
                try {
                    each->tick();
                } catch (TimeLimitExceeded&) {
                    if (exp_cnt > 0) exp_cnt--; // 安全检查
                } catch (const std::exception& e) {
                    std::cout << "经验球更新时发生异常: " << e.what() << std::endl;
                    each->disable();
                    if (exp_cnt > 0) exp_cnt--;
                }
            }
        }
    } catch (const std::exception& e) {
        std::cout << "游戏逻辑更新时发生异常: " << e.what() << std::endl;
    } catch (...) {
        std::cout << "游戏逻辑更新时发生未知异常" << std::endl;
    }
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
    std::cout << "[Log] GameState::initEnemy 开始, 阶段: " << stage << std::endl;
    
    try {
        // 1. 首先安全地删除敌人控制器
        if (enemy_control) {
            std::cout << "[Log] 删除旧敌人控制器" << std::endl;
            delete enemy_control;
            enemy_control = nullptr;
        }
        
        // 2. 清理现有敌人（使用更安全的方式）
        std::cout << "[Log] 开始清理敌人数组" << std::endl;
        for (auto& enemyList : enemies) {
            for (auto& enemy : enemyList) {
                if (enemy) {
                    try {
                        // 首先禁用敌人，避免其继续活动
                        enemy->disable();
                        // 使用deleteLater安全释放QObject
                        if (enemy->hp_bar) {
                            enemy->hp_bar->hide();
                            enemy->hp_bar->deleteLater();
                            enemy->hp_bar = nullptr;
                        }
                        delete enemy;
                    } catch (...) {
                        std::cout << "[Log] 清理敌人时发生异常" << std::endl;
                    }
                    enemy = nullptr;
                }
            }
            enemyList.clear();
        }
        enemies.clear();
        std::cout << "[Log] 敌人数组清理完成" << std::endl;
        
        // 3. 检查阶段有效性
        if (stage <= 0 || stage > NUM_OF_STAGES) {
            std::cout << "[Log] 错误：无效的游戏阶段 " << stage << std::endl;
            return;
        }
        
        // 4. 获取阶段信息
        const StageInfo& stageInfo = STAGE_INFOS[stage - 1];
        std::cout << "[Log] 初始化阶段 " << stage << "，敌人类型数: " << stageInfo.type_num << std::endl;
        
        // 5. 准备敌人容器
        std::cout << "[Log] 开始准备敌人容器" << std::endl;
        enemies.reserve(stageInfo.type_num); // 预分配空间，减少动态扩展
        
        // 确保敌人类型和数量的设置是一致的
        for (int i = 0; i < stageInfo.type_num; i++) {
            if (i < stageInfo.type_info.size()) {
                const auto& typeInfo = stageInfo.type_info[i];
                std::cout << "[Log] 创建类型 " << typeInfo.enemy_type << " 的容器，最大数量: " << typeInfo.max_nums << std::endl;
                
                // 使用带初始化的vector创建，避免频繁修改
                std::vector<Enemy*> enemyList(typeInfo.max_nums, nullptr);
                enemies.push_back(std::move(enemyList)); // 使用移动语义，避免复制
            } else {
                std::cout << "[Log] 警告：缺少敌人类型 " << i+1 << " 的信息" << std::endl;
                enemies.push_back(std::vector<Enemy*>());
            }
        }
        std::cout << "[Log] 敌人容器准备完成，类型数量: " << enemies.size() << std::endl;
        
        // 6. 创建敌人控制器
        std::cout << "[Log] 创建敌人控制器" << std::endl;
        enemy_control = new EnemyController(this, stage);
        std::cout << "[Log] 敌人控制器创建完成" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "[Log] 初始化敌人时出现异常: " << e.what() << std::endl;
        // 确保enemies至少有一个空vector，防止后续访问出错
        if (enemies.empty()) {
            enemies.push_back(std::vector<Enemy*>());
        }
    } catch (...) {
        std::cout << "[Log] 初始化敌人时出现未知异常" << std::endl;
        if (enemies.empty()) {
            enemies.push_back(std::vector<Enemy*>());
        }
    }
    
    std::cout << "[Log] GameState::initEnemy 结束，敌人类型数: " << enemies.size() << std::endl;
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

