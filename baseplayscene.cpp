#include "baseplayscene.h"
#include "playscene1.h"
#include <QPainter>
#include <QTimer>
#include <QDebug>
#include <algorithm>
#include <vector>

baseplayscene::baseplayscene(QWidget *parent) : QMainWindow(parent) {
    my_tank = new tank;
    enemy_tank = new tank;
    bullet1 = new bullet;
    bullet2 = new bullet;
    bullet3 = new bullet;
    bullet1E = new bullet;
    bullet2E = new bullet;
    bullet3E = new bullet;

    initMy_Tank();
    initEnemy_Tank();
    PlayScene1 scene;
    mapData = mapData;
}

baseplayscene::~baseplayscene() {
    delete my_tank;
    delete enemy_tank;
    delete bullet1;
    delete bullet2;
    delete bullet3;
    delete bullet1E;
    delete bullet2E;
    delete bullet3E;
}

void baseplayscene::initMy_Tank() {
    my_tank->x = 2;
    my_tank->y = 2;
    my_tank->direction = 1;
    my_tank->isInit = true;
    my_tank->alive = true;
    my_tank->IsGood = true;
    my_tank->DieOnce = false;
}

void baseplayscene::initEnemy_Tank() {
    enemy_tank->x = 38;
    enemy_tank->y = 26;
    enemy_tank->direction = 2;
    enemy_tank->isInit = true;
    enemy_tank->alive = true;
    enemy_tank->IsGood = false;
    enemy_tank->DieOnce = false;
}

void baseplayscene::initBullet(tank* tank, bullet* bullet, int direction) {
    if (direction == 1) {
        bullet->x = tank->x * 25 + 25;
        bullet->y = tank->y * 25;
    } else if (direction == 2) {
        bullet->x = tank->x * 25 + 25;
        bullet->y = tank->y * 25 + 50;
    } else if (direction == 3) {
        bullet->x = tank->x * 25;
        bullet->y = tank->y * 25 + 25;
    } else if (direction == 4) {
        bullet->x = tank->x * 25 + 50;
        bullet->y = tank->y * 25 + 25;
    }
    bullet->isHit = false;
    bullet->direction = direction;
    bullet->HitsNum = 0;
    bullet->timer = new QTimer(this);
    bullet->timer->start(5);
    bullet->update = false;
    connect(bullet->timer, &QTimer::timeout, [=]() {
        bullet->update = true;
        update();
    });
}

void baseplayscene::SetTankNewLocation(tank* tank, int direction) {
    tank->direction = direction;
    switch (direction) {
    case 1:
        tank->y--;
        break;
    case 2:
        tank->y++;
        break;
    case 3:
        tank->x--;
        break;
    case 4:
        tank->x++;
        break;
    default:
        break;
    }
}

// 补充EnemyFire函数实现
void baseplayscene::EnemyFire() {
    if (enemy_tank->alive) {
        initBullet(enemy_tank, bullet1E, enemy_tank->direction);
    }
}

void baseplayscene::tank_update(tank* tank) {
    if (checkTankIsMoveable(tank)) {
        SetTankNewLocation(tank, tank->direction);
    }
}

void baseplayscene::LocationSetting(tank* tank, int direction) {
    tank->direction = direction;
    update();
}

void baseplayscene::BulletHitWall(bullet* bullet, int x, int y) {
    PlayScene1 scene;
    std::vector<std::vector<int>>& tempmap = mapData;
    if (tempmap[x][y] == 2) {
        tempmap[x][y] = 0;
        bullet->isHit = true;
        bullet->HitsNum++;
    } else if (tempmap[x][y] == 1) {
        bullet->isHit = true;
        bullet->HitsNum++;
    }
}

void baseplayscene::partBulletHitWall(bullet* bullet) {
    int x = bullet->y / 25;
    int y = bullet->x / 25;
    BulletHitWall(bullet, x, y);
}

void baseplayscene::paintBullet(bullet* bullet) {
    if (!bullet->isHit) {
        QPainter painter(this);
        QPixmap bulletPixmap("://res/bullet.png");
        painter.drawPixmap(bullet->x, bullet->y, 10, 10, bulletPixmap);
    }
}

void baseplayscene::BulletHitTank(tank* tank, bullet* bullet) {
    if (!bullet->isHit) {
        int tankX = tank->x * 25;
        int tankY = tank->y * 25;
        int bulletX = bullet->x;
        int bulletY = bullet->y;

        if (bulletX >= tankX && bulletX <= tankX + 50 && bulletY >= tankY && bulletY <= tankY + 50) {
            tank->alive = false;
            bullet->isHit = true;
            bullet->HitsNum++;
            DeathEffect(tank);
        }
    }
}

void baseplayscene::BulletHit(bullet* bullet) {
    partBulletHitWall(bullet);
    BulletHitTank(my_tank, bullet);
    BulletHitTank(enemy_tank, bullet);
}

void baseplayscene::EnemyTankMove() {
    EnemyTimer = new QTimer(this);
    PlayScene1 scene;
    res = AStar(mapData); // 调用 A* 算法
    num = res.size() - 1;
    connect(EnemyTimer, &QTimer::timeout, [&]() {
        if (num >= 0) {
            enemy_tank->direction = res[num];
            tank_update(enemy_tank);
            --num;
        } else {
            EnemyTimer->stop();
        }
        EnemyFire();
    });
}

// 启发式函数，使用曼哈顿距离
int baseplayscene::heuristic(int x1, int y1, int x2, int y2) {
    return std::abs(x1 - x2) + std::abs(y1 - y2);
}

// A* 算法实现
std::vector<int> baseplayscene::AStar(std::vector<std::vector<int>> tempmap) {
    std::vector<int> path;
    int startX = enemy_tank->x;
    int startY = enemy_tank->y;
    int goalX = my_tank->x;
    int goalY = my_tank->y;

    // 定义开放列表和关闭列表
    std::priority_queue<Node> openList;
    std::vector<std::vector<bool>> closedList(tempmap.size(), std::vector<bool>(tempmap[0].size(), false));

    // 初始化起点节点
    Node startNode(startX, startY, 0, heuristic(startX, startY, goalX, goalY), nullptr);
    openList.push(startNode);

    while (!openList.empty()) {
        // 从开放列表中选择 f 值最小的节点
        Node current = openList.top();
        openList.pop();

        // 如果当前节点是目标节点，构建路径
        if (current.x == goalX && current.y == goalY) {
            while (current.parent != nullptr) {
                // 计算方向
                int dx = current.x - current.parent->x;
                int dy = current.y - current.parent->y;
                int direction;
                if (dx == 1) direction = 4;
                else if (dx == -1) direction = 3;
                else if (dy == 1) direction = 2;
                else direction = 1;
                path.push_back(direction);
                current = *current.parent;
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        // 将当前节点加入关闭列表
        closedList[current.y][current.x] = true;

        // 定义相邻节点的偏移量
        int dx[] = {0, 0, 1, -1};
        int dy[] = {1, -1, 0, 0};

        // 扩展相邻节点
        for (int i = 0; i < 4; ++i) {
            int newX = current.x + dx[i];
            int newY = current.y + dy[i];

            // 检查相邻节点是否合法
            if (newX >= 0 && newX < tempmap[0].size() && newY >= 0 && newY < tempmap.size() &&
                tempmap[newY][newX] == 0 && !closedList[newY][newX]) {
                int newG = current.g + 1;
                int newH = heuristic(newX, newY, goalX, goalY);
                Node newNode(newX, newY, newG, newH, new Node(current.x, current.y, current.g, current.h, current.parent));
                openList.push(newNode);
            }
        }
    }

    // 如果未找到路径，返回空路径
    return path;
}

void baseplayscene::DeathEffect(tank* tank) {
    // 这里可以添加死亡特效的具体逻辑，比如绘制爆炸动画、播放音效等
    if (tank) {
        tank->alive = false;
        // 示例：绘制一个简单的爆炸效果
        QPainter painter(this);
        QPixmap explosionPixmap("://res/explosion.png");
        painter.drawPixmap(tank->x * 25, tank->y * 25, 50, 50, explosionPixmap);
    }
}

// 补充ShootBulletE函数实现
void baseplayscene::ShootBulletE() {
    if (enemy_tank->alive) {
        initBullet(enemy_tank, bullet1E, enemy_tank->direction);
    }
}

// 补充DodgeBullet函数实现
void baseplayscene::DodgeBullet() {
    // 简单示例：敌方坦克随机改变方向躲避子弹
    int randomDirection = rand() % 4 + 1;
    LocationSetting(enemy_tank, randomDirection);
}

// 补充GameOver函数实现
void baseplayscene::GameOver() {
    IsGameOver = true;
    // 可以添加游戏结束的具体逻辑，如显示游戏结束界面等
}

// 优化checkTankIsMoveable函数，减少重复代码
bool baseplayscene::checkTankIsMoveable(tank* tank) {
    int tx = tank->y;
    int ty = tank->x;
    PlayScene1 scene;
    std::vector<std::vector<int>> tempmap = mapData;
    std::vector<std::pair<int, int>> offsets = {{1, 0}, {1, 1}, {0, 1}};
    for (const auto& offset : offsets) {
        int newX = tx + offset.first;
        int newY = ty + offset.second;
        if (newX < tempmap.size() && newY < tempmap[0].size() && (tempmap[newX][newY] == 1 || tempmap[newX][newY] == 2)) {
            return false;
        }
    }
    return tx >= 0 && ty >= 0 && tx < tempmap.size() && ty < tempmap[0].size() && tempmap[tx][ty] == 0;
}

void baseplayscene::paintEvent(QPaintEvent *event) {
    QMainWindow::paintEvent(event);
    QPainter painter(this);
painter.setPen(Qt::red);
painter.drawText(100, 100, "BasePlayScene Test"); // 添加简单的绘制代码
}
