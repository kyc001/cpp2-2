#ifndef ENEMYCONTROLLER_H
#define ENEMYCONTROLLER_H
#include <vector>
#include <utility>
#include <QtCore/QMutex>

class GameState;
class Enemy;
class QTimer;

class EnemyController {
public:
    EnemyController(GameState* g, int stage);
    ~EnemyController() = default;
    void tick();
    void reportDeath(int type);
    void reportDeath(int type, double rx, double ry);

    [[nodiscard]] int getEnemyDeathCnt() const { return enemy_death_cnt; }

private:
    struct TypeController {
        int enemy_type{};
        int max_nums{};
        int max_cds{};
        int min_cds{};
        int num_counters{};
        int cds{};
        int reduce_counters{};
        int reduce_cds{};
        int vector_pos{};
    };

    std::vector<TypeController> types;
    GameState *game;
    int enemy_death_cnt;
    int game_stage;
    int type_num;

    void addEnemy(int enemy_type, std::vector<Enemy *> &buffer);
    void enableNullSpace(int enemy_type, Enemy *&space);
    void enableUsedSpace(int enemy_type, Enemy *&space);
    std::pair<double, double> generatePosition();
    
    // 新增方法
    std::pair<double, double> generateSafePosition();
    std::pair<double, double> generatePositionAwayFromPlayer(double minDistance = 300.0);
    bool isPositionValid(double x, double y);
    bool isPositionSafe(double x, double y);
    
    // 同步锁，防止多线程问题
    QMutex enemy_mutex;
};

#endif
