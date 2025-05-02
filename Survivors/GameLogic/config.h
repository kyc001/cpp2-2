#ifndef CONFIG_H
#define CONFIG_H
#include <vector>
/*------游戏设置信息------*/
#define GAME_WIDTH 1000
#define GAME_HEIGHT 800
#define GAME_TITLE "Survivors"
#define GAME_VERSION "V0.1.0"
#define GAME_RATE 60
#define TIMER_RATE (1000 / GAME_RATE)

extern int SOUL;
extern int LEVEL;
extern int POWER;
extern int SPEED;
extern int LIVE;
extern int INTELLIGENCE;
extern int GAME_HALT;
extern int HERO_TYPE;
extern int HALT_HP;
extern int HALT_HPM;
extern int DAMAGE;
extern int HALT_CD;
extern int GAME_LEVEL;
extern int HALT_EXP;
extern int HALT_EXPM;
extern int FIRST_RESUME;
extern double HERO_X;
extern double HERO_Y;
extern int HERO_SPEED;
extern double HERO_REDUCE;

// 游戏控制模式 (0: 键盘WASD, 1: 鼠标跟随)
extern int CONTROL_MODE;
// 游戏音量 (0-100)
extern int GAME_VOLUME;

/*------地图设置信息------*/
#define NUM_OF_MAPS 1
#define MAP_1_NAME "Grass"
#define MAP_1_PATH ":/Assets/map1_bg.png"
#define MAP_1_BR ":/Assets/map1_br.png"
#define MAP_1_CONFIG "MapConfigs/map1_br.txt"
#define MAP_1_HEIGHT 3000
#define MAP_1_LATTICE_X 30
#define MAP_1_LATTICE_Y 30

/*------玩家角色设置信息--*/
#define NUM_OF_HEROS 2
#define HERO_1_NAME "Link"
#define HERO_1_PATH ":/Assets/maodie.gif"
#define HERO_1_HEALTH (50 + 40 * LIVE + 10 * POWER)
#define HERO_INIT_EXP 20
#define HERO_1_SPEED (4 + 4 * SPEED + LIVE)
#define HERO_1_REDUCE (0 + 0.01 * LIVE + 0.02 * POWER)

#define HERO_2_HEALTH (40 + 25 * LIVE + 10 * POWER + 5 * SPEED)
#define HERO_2_SPEED (5 + 5 * SPEED + POWER)
#define HERO_2_REDUCE (0 + 0.01 * LIVE)

#define INC_RATE (1.0 + 0.02 * SPEED + 0.02 * POWER + 0.02 * LIVE + 0.05 * INTELLIGENCE)
#define HP_INC_RATE INC_RATE
#define SPEED_INC_RATE INC_RATE
#define WEAPON_INC_RATE INC_RATE

/*------子弹相关设置信息--*/
#define NUM_OF_BULLETS 2
#define DYNAMIC_BULLETS_MAX 30
#define AOE_BULLET_1_PATH ":/Assets/aoebullet1.png"
#define AOE_BULLET_1_CD (32 - 2 * SPEED - INTELLIGENCE)

#define BULLET_2_PATH ":/Assets/bullet2.svg"
#define BULLET_2_SPEED 10
#define BULLET_2_SIZE 20

#define BULLET_3_PATH ":/Assets/enemybullet.png"
#define BULLET_3_SPEED 8
#define BULLET_3_SIZE 15

/*------武器相关设置信息--*/
#define NUM_OF_WEAPONS 1

#define WEAPON_1_NAME "电磁阵"
#define WEAPON_1_DAMAGE (3 + POWER)
#define WEAPON_1_DEFAULT_RANGE (130 + 10 * INTELLIGENCE)
#define WEAPON_1_BULLET_TYPE 1

#define WEAPON_2_DAMAGE (5 + 3 * POWER)
#define WEAPON_2_CD (42 - 2 * SPEED)
#define WEAPON_2_BULLET_TYPE 2

#define E_WEAPON_DAMAGE 5
#define E_WEAPON_CD 60

/*------怪物相关设置信息--*/
#define NUM_OF_ENEMIES 3

#define ENEMY_1_NAME "老菜"
#define ENEMY_1_TYPE NoWeaponEnemy
#define ENEMY_1_POWER 5
#define ENEMY_1_HEALTH 30
#define ENEMY_1_SPEED 5
#define ENEMY_1_CD 60
#define ENEMY_1_PATH ":/Assets/laocai.png"

#define ENEMY_2_NAME "绿块"
#define ENEMY_2_TYPE NoWeaponEnemy
#define ENEMY_2_POWER 4
#define ENEMY_2_HEALTH 35
#define ENEMY_2_SPEED 4
#define ENEMY_2_CD 70
#define ENEMY_2_PATH ":/Assets/enemy2.png"

#define ENEMY_3_NAME "蓝三角"
#define ENEMY_3_TYPE NoWeaponEnemy
#define ENEMY_3_POWER 7
#define ENEMY_3_HEALTH 45
#define ENEMY_3_SPEED 5.5
#define ENEMY_3_CD 80
#define ENEMY_3_PATH ":/Assets/enemy3.png"

#define EBALL_PATH ":/Assets/expball.png"
#define EBALL_SIZE 20
#define EBALL_CD 1200
#define EBALL_MAX 300

/*-----难度相关设置------*/
#define NUM_OF_STAGES 1


struct StageInfo{
    int type_num;
    struct TypeInfo{
        int enemy_type;
        int max_nums;
        int cd_max;
        int cd_min;
        int reduce_rate;
    };
    std::vector<TypeInfo> type_info;
};

const StageInfo STAGE_INFOS[NUM_OF_STAGES] = {
        {NUM_OF_ENEMIES,  // 显式设置为NUM_OF_ENEMIES，确保一致性
            {   // 类型1敌人：红色圆形
                {1, 5, 200, 100, 30},     // 最大数量5，CD 3-1.5秒，每30帧减少CD
                // 类型2敌人：绿色方块
                {2, 3, 300, 200, 40},     // 最大数量3，CD 5-3秒，每40帧减少CD
                // 类型3敌人：蓝色三角
                {3, 2, 400, 300, 50},    // 最大数量2，CD 6.5-5秒，每50帧减少CD
            }
        },
};

#endif //VSCPROJECT_CONFIG_H
