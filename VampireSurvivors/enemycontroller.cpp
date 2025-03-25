#include "bin/enemycontroller.h"

EnemyController::EnemyController(GameState *g, int stage) : game(g), game_stage(stage) {
    StageInfo this_stage = STAGE_INFOS[stage - 1];
    type_num = this_stage.type_num;

    for(auto & each : g - > enemies) {
        each.clear();
    }
    g -> enemies.clear();

    int vector_pos = 0;
    for(auto& each:this_stage.type_info) {
        TypeController temp = {};
        temp.max_cds = each.cd_max;
        temp.enemy_type = each.type;
        temp.max_nums = each.max_nums;
        temp.num_counters = 0;
        temp.cds = each.cd_max;
        temp.min_cds = each.cd_min;
        temp.reduce_cds = each.reduce_rate;
        temp.reduce_counters = each.reduce_rate;
        temp.vector_pos = vector_pos;
        types.push_back(temp);
        std::vector<Enemy *> t(each.max_nums);
        g->enemies.push_back(t);
        vector_pos++;
    }
    enemy_death_cnt = 0;

    srand(time(nullptr));
}
