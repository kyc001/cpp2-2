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
    std::cout << "掉落物数量" << exp_cnt << std::endl;
    player->tick();
    
    // 确保enemy_control不为空
    if (enemy_control) {
        enemy_control->tick();
    }
    
    // 更新悬浮球
    for(auto& orb : floating_orbs) {
        orb->tick();
    }
    
    // 检查悬浮球与敌人的碰撞
    checkOrbEnemyCollisions();
    
    // 安全遍历enemies数组
    for(auto& lst : enemies){
        for(auto& each : lst){
            if(each && each->isEnabled())
                each->tick();
        }
    }

    for(auto& each : exp_balls){
        if(each) {
            try {
                each->tick();
            }
            catch (TimeLimitExceeded &) {
                exp_cnt--;
            }
        }
    }

    judgeDamageEnemies();
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
    // 先清空现有enemies
    enemies.clear();
    
    // 初始化enemies数组，根据配置创建足够的容器
    const StageInfo& stageInfo = STAGE_INFOS[stage - 1];
    for (int i = 0; i < stageInfo.type_num; i++) {
        enemies.push_back(std::vector<Enemy*>(stageInfo.type_info[i].max_nums, nullptr));
    }
    
    // 创建敌人控制器
    enemy_control = new EnemyController(this, stage);
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
    for(auto & each : exp_balls){
        if(each &&  each->isEnabled()){
            double x = each->real_pos.first;
            double y = each->real_pos.second;
            if(x >= player_pos.first && x <= player_pos.first + 60 &&
                y >= player_pos.second && y <= player_pos.second + 60){
                cnt += each->getValue();
                each->disable();
            }
        }
    }
    std::cout << "获得经验：" << cnt << std::endl;
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

