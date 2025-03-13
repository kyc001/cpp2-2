//2025年3月12日 18点53分
#ifndef BASEPLAYSCENE_H
#define BASEPLAYSCENE_H

#include <QMainWindow>
#include <vector>
#include <queue>
#include <cmath>

struct point {
    int x, y;
    int step;
};

//节点结构体，用于 A star 算法
struct Node {
    int x, y;
    int g; // 从起点到当前节点的实际代价
    int h; // 从当前节点到目标节点的启发式估计代价
    int f; // f = g + h
    Node* parent;

    Node(int _x, int _y, int _g, int _h, Node* _parent)
        :x(_x), y(_y), g(_g), h(_h), f(_g + _h), parent(_parent) {}

    // 重载运算符，用于优先队列
    bool operator < (const Node& other) const {
        return f > other.f;
    }
};

class baseplayscene : public QMainWindow {
    Q_OBJECT
public:
    explicit baseplayscene(QWidget *parent = nullptr);
    virtual ~baseplayscene();

    struct tank{
        int x, y;
        int direction;
        bool isInit = false;
        bool alive = true;
        bool IsGood = true;
        bool DieOnce = false;
    };

    struct bullet {
        int x, y;
        int direction;
        bool isHit = true;
        int HitsNum = 0;
        QTimer* timer;
        bool update = true;
    };

    // 纯虚函数，用于绘制场景
    virtual void paintEvent(QPaintEvent *event) = 0;

    // 初始化坦克
    void initMy_Tank();
    void initEnemy_Tank();

    // 初始化子弹
    void initBullet(tank* tank, bullet* bullet, int direction);

    // 设置坦克新位置
    void SetTankNewLocation(tank* tank, int direction);

    // 检查坦克是否可移动
    bool checkTankIsMoveable(tank* tank);

    // 更新坦克位置
    void tank_update(tank* tank);

    // 设置坦克位置
    void LocationSetting(tank* tank, int direction);

    // 子弹撞墙处理
    void BulletHitWall(bullet* bullet, int x, int y);
    void partBulletHitWall(bullet* bullet);

    // 绘制子弹
    void paintBullet(bullet* bullet);

    // 子弹击中坦克处理
    void BulletHitTank(tank* tank, bullet* bullet);

    // 子弹击中处理
    void BulletHit(bullet* bullet);

    // 敌方坦克移动
    void EnemyTankMove();

    // 死亡特效
    void DeathEffect(tank* tank);

    // 敌方开火
    void EnemyFire();

    // 射击子弹
    void ShootBulletE();

    // 躲避子弹
    void DodgeBullet();

    // A* 算法
    std::vector<int> AStar(std::vector<std::vector<int>> tempmap);

    // 游戏结束处理
    void GameOver();

    // 启发式函数，使用曼哈顿距离
    int heuristic(int x1, int y1, int x2, int y2);

    std::vector<int> res;
    int num;
    QTimer* reroute;
    QTimer* EnemyTimer;
    QTimer* movespeed;
    bool SpeedControl = true;
    bool IsGameOver = false;

signals:
    void playSceneBack();

protected:
    tank* my_tank;
    tank* enemy_tank;
    bullet* bullet1;
    bullet* bullet2;
    bullet* bullet3;
    bullet* bullet1E;
    bullet* bullet2E;
    bullet* bullet3E;
private:
    std::vector<std::vector<int>> mapData;
};

#endif // BASEPLAYSCENE_H
