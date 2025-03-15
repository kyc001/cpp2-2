#include "playscene.h"
#include <QPainter>
#include <QBrush>
#include <QString>
#include <QKeyEvent>
#include <QTimer>
#include <QDebug>
#include <queue>
#include <QMediaPlayer>
using namespace std;

// 上1下2左3右4
vector<vector<int>> mainmap = {
    // 1表示不能打碎的墙 2表示可以打碎的墙
    // 1
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 2, 2, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    // 2
    {1, 0, 0, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1},
    {1, 0, 0, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 2, 2, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    // 3
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    // 4
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
    // 5
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    // 6
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
    // 7
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    // 8
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};
vector<vector<int>> tempmap = mainmap;
PlayScene::PlayScene(QWidget *parent) : QMainWindow(parent)
{
    mainmap = forevermap;
    tempmap = forevermap;
    // 设置窗口固定大小
    setFixedSize(1050, 725);
    // 设置标题
    setWindowTitle("坦克大战");
    // 设置图标
    setWindowIcon(QIcon("://res/tank_up.jpg"));
    // 返回按钮
    backBtn = new button("://res3/back.tga");
    backBtn->setParent(this);
    backBtn->move(1050, 750);
    connect(backBtn, &button::clicked, [=]()
            {
        backBtn->pop1();
        backBtn->pop2();
        QTimer::singleShot(500,this,[=](){
            emit this->playSceneBack();
        }); });
}
// 画基本地图 画坦克
void PlayScene::paintEvent(QPaintEvent *event)
{
    if (IsGameOver && !Btnmove)
    {
        Btnmove = true;
        backBtn->move(this->width() * 0.5 - backBtn->width() * 0.5, this->height() * 0.75);
    }

    /**************************/
    // 判断我方坦克是否初始化 若未初始化将我方坦克初始化
    if (!my_tank->isInit)
    {
        initMy_Tank();
        my_tank->isInit = true;
    }
    // 加载四个方向我方tank图片
    QPixmap pixUp;
    pixUp.load("://res/tank_up.jpg");
    QPixmap pixDown;
    pixDown.load("://res/tank_down.jpg");
    QPixmap pixLeft;
    pixLeft.load("://res/tank_left.jpg");
    QPixmap pixRight;
    pixRight.load("://res/tank_right.jpg");

    // 判断敌方坦克是否初始化 若未初始化将敌方坦克初始化
    if (!enemy_tank->isInit)
    {
        initEnemy_Tank();
        enemy_tank->isInit = true;
    }
    // 加载四个方向敌方tank图片
    QPixmap pixUpE;
    pixUpE.load("://res3/enemy_tank_down.jpg");
    QPixmap pixDownE;
    pixDownE.load("://res/enemy_tank_up.jpg");
    QPixmap pixLeftE;
    pixLeftE.load("://res3/enemy_tank_left.jpg");
    QPixmap pixRightE;
    pixRightE.load("://res/enemy_tank_right.jpg");
    /**************************/

    QPainter painter(this);
    QBrush Brush(Qt::black);
    QPixmap pix1;
    pix1.load("://res/wall2.jpg");
    QPixmap pix2;
    pix2.load("://res/wall1.jpg");
    painter.setBrush(Brush);
    painter.drawRect(0, 0, this->width(), this->height());

    for (int i = 0; i < 29; ++i)
    {
        for (int j = 0; j < 42; ++j)
        {
            if (mainmap[i][j] == 1)
            {
                painter.drawPixmap(25 * j, 25 * i, 25, 25, pix1);
            }
            if (mainmap[i][j] == 2)
            {
                painter.drawPixmap(25 * j, 25 * i, 25, 25, pix2);
            }
            if (mainmap[i][j] == 204)
            {
                painter.drawPixmap(25 * j, 25 * i, 50, 50, pixRight);
            }
            if (mainmap[i][j] == 203)
            {
                painter.drawPixmap(25 * j, 25 * i, 50, 50, pixLeft);
            }
            if (mainmap[i][j] == 202)
            {
                painter.drawPixmap(25 * j, 25 * i, 50, 50, pixDown);
            }
            if (mainmap[i][j] == 201)
            {
                painter.drawPixmap(25 * j, 25 * i, 50, 50, pixUp);
            }
            if (mainmap[i][j] == 304)
            {
                painter.drawPixmap(25 * j, 25 * i, 50, 50, pixRightE);
            }
            if (mainmap[i][j] == 303)
            {
                painter.drawPixmap(25 * j, 25 * i, 50, 50, pixLeftE);
            }
            if (mainmap[i][j] == 302)
            {
                painter.drawPixmap(25 * j, 25 * i, 50, 50, pixDownE);
            }
            if (mainmap[i][j] == 301)
            {
                painter.drawPixmap(25 * j, 25 * i, 50, 50, pixUpE);
            }
        }
    }
    // 判断我方坦克是不是还活着

    DeathEffect(my_tank);
    DeathEffect(enemy_tank);

    // 画子弹 同时判断子弹有没有击中tank
    paintBullet(bullet1);
    BulletHitTank(enemy_tank, bullet1);
    BulletHitTank(my_tank, bullet1);
    paintBullet(bullet2);
    BulletHitTank(enemy_tank, bullet2);
    BulletHitTank(my_tank, bullet2);
    paintBullet(bullet3);
    BulletHitTank(enemy_tank, bullet3);
    BulletHitTank(my_tank, bullet3);

    paintBullet(bullet1E);
    BulletHitTank(enemy_tank, bullet1E);
    BulletHitTank(my_tank, bullet1E);
    paintBullet(bullet2E);
    BulletHitTank(enemy_tank, bullet2E);
    BulletHitTank(my_tank, bullet2E);
    paintBullet(bullet3E);
    BulletHitTank(enemy_tank, bullet3E);
    BulletHitTank(my_tank, bullet3E);

    GameOver();
}

// 初始化我方tank
void PlayScene::initMy_Tank()
{
    QMediaPlayer *startSound = new QMediaPlayer(this);
    startSound->setSource(QUrl("qrc:/music/background.wav"));
    my_tank->x = 1;
    my_tank->y = 1;
    my_tank->direction = 4;
    my_tank->alive = true;
    my_tank->IsGood = true;
    // 初始化我方tank的位置
    LocationSetting(my_tank, my_tank->direction);
    movespeed = new QTimer(this);
    connect(movespeed, &QTimer::timeout, [=]()
            { SpeedControl = true; });
    movespeed->start(200);
}

// 初始化敌方tank
void PlayScene::initEnemy_Tank()
{
    enemy_tank->x = 35;
    enemy_tank->y = 20;
    enemy_tank->direction = 3;
    enemy_tank->alive = true;
    enemy_tank->IsGood = false;
    // 初始化敌方tank的位置
    LocationSetting(enemy_tank, enemy_tank->direction);
    reroute = new QTimer(this);
    EnemyTankMove();
    connect(reroute, &QTimer::timeout, [=]()
            {
         EnemyTimer->stop();
         EnemyTankMove(); });
    reroute->start(3000);
}

// 初始化子弹
void PlayScene::initBullet(tanks *tank, bullets *bullet, int direction)
{
    if (direction == 1)
    {
        bullet->x = tank->x * 25 + 25;
        bullet->y = tank->y * 25;
    }
    if (direction == 2)
    {
        bullet->x = tank->x * 25 + 25;
        bullet->y = tank->y * 25 + 50;
    }
    if (direction == 3)
    {
        bullet->x = tank->x * 25;
        bullet->y = tank->y * 25 + 25;
    }
    if (direction == 4)
    {
        bullet->x = tank->x * 25 + 50;
        bullet->y = tank->y * 25 + 25;
    }
    bullet->isHit = false;
    bullet->direction = direction;
    bullet->HitsNum = 0;
    bullet->timer = new QTimer(this);
    bullet->timer->start(5);
    bullet->update = false;
    connect(bullet->timer, &QTimer::timeout, [=]()
            {
        bullet->update=true;
        update(); });
}

// 更新坦克的位置
void PlayScene::tank_update(tanks *tank)
{
    switch (tank->direction)
    {
    case 1:
        SetTankNewLocation(tank, 1);
        break;
    case 2:
        SetTankNewLocation(tank, 2);
        break;
    case 3:
        SetTankNewLocation(tank, 3);
        break;
    case 4:
        SetTankNewLocation(tank, 4);
        break;
    default:
        break;
    }
    update();
}

// 检查坦克是否能移动
bool PlayScene::checkTankIsMoveable(tanks *tank)
{
    if (tank->direction == 1)
    {
        if (mainmap[tank->y - 1][tank->x] != 0)
        {
            return false;
        }
        if (mainmap[tank->y - 1][tank->x + 1] != 0)
        {
            return false;
        }
    }
    if (tank->direction == 2)
    {
        if (mainmap[tank->y + 2][tank->x] != 0)
        {
            return false;
        }
        if (mainmap[tank->y + 2][tank->x + 1] != 0)
        {
            return false;
        }
    }
    if (tank->direction == 3)
    {
        if (mainmap[tank->y][tank->x - 1] != 0)
        {
            return false;
        }
        if (mainmap[tank->y + 1][tank->x - 1] != 0)
        {
            return false;
        }
    }
    if (tank->direction == 4)
    {
        if (mainmap[tank->y][tank->x + 2] != 0)
        {
            return false;
        }
        if (mainmap[tank->y + 1][tank->x + 2] != 0)
        {
            return false;
        }
    }
    return true;
}

// 设置坦克的新位置。
void PlayScene::SetTankNewLocation(tanks *tank, int direction)
{
    mainmap[tank->y][tank->x] = 0;
    mainmap[tank->y + 1][tank->x] = 0;
    mainmap[tank->y][tank->x + 1] = 0;
    mainmap[tank->y + 1][tank->x + 1] = 0;
    switch (direction)
    {
    case 1:
        tank->y -= 1;
        LocationSetting(tank, 1);
        break;
    case 2:
        tank->y += 1;
        LocationSetting(tank, 2);
        break;
    case 3:
        tank->x -= 1;
        LocationSetting(tank, 3);
        break;
    case 4:
        tank->x += 1;
        LocationSetting(tank, 4);
        break;
    default:
        break;
    }
}

// 坦克位置设置
void PlayScene::LocationSetting(tanks *tank, int direction)
{
    int a = 200;
    if (!tank->IsGood)
    {
        a = 300;
    }
    mainmap[tank->y][tank->x] = a + direction;
    mainmap[tank->y + 1][tank->x] = a;
    mainmap[tank->y][tank->x + 1] = a;
    mainmap[tank->y + 1][tank->x + 1] = a;
}

// 我方坦克的移动-键盘事件控制
void PlayScene::keyPressEvent(QKeyEvent *event)
{
    if (my_tank->alive && !IsGameOver)
    {
        if (event->key() == Qt::Key_W && SpeedControl)
        {
            SpeedControl = false;
            if (my_tank->direction != 1)
            {
                LocationSetting(my_tank, 1);
                update();
                my_tank->direction = 1;
                return;
            }
            if (checkTankIsMoveable(my_tank))
            {
                tank_update(my_tank);
            }
        }
        if (event->key() == Qt::Key_S && SpeedControl)
        {
            SpeedControl = false;
            if (my_tank->direction != 2)
            {
                LocationSetting(my_tank, 2);
                update();
                my_tank->direction = 2;
                return;
            }
            if (checkTankIsMoveable(my_tank))
            {
                tank_update(my_tank);
            }
        }
        if (event->key() == Qt::Key_A && SpeedControl)
        {
            SpeedControl = false;
            if (my_tank->direction != 3)
            {
                LocationSetting(my_tank, 3);
                update();
                my_tank->direction = 3;
                return;
            }
            if (checkTankIsMoveable(my_tank))
            {
                tank_update(my_tank);
            }
        }
        if (event->key() == Qt::Key_D && SpeedControl)
        {
            SpeedControl = false;
            if (my_tank->direction != 4)
            {
                LocationSetting(my_tank, 4);
                update();
                my_tank->direction = 4;
                return;
            }
            if (checkTankIsMoveable(my_tank))
            {
                tank_update(my_tank);
            }
        }
        if (event->key() == Qt::Key_J)
        {
            if (bullet1->isHit)
            {
                initBullet(my_tank, bullet1, my_tank->direction);
            }
            else if (bullet2->isHit)
            {
                initBullet(my_tank, bullet2, my_tank->direction);
            }
            else if (bullet3->isHit)
            {
                initBullet(my_tank, bullet3, my_tank->direction);
            }
        }
    }
    QWidget::keyPressEvent(event);
}

// 敌方坦克的移动
void PlayScene::EnemyTankMove()
{
    EnemyTimer = new QTimer(this);
    res = Astar(tempmap);
    num = res.size() - 1;
    connect(EnemyTimer, &QTimer::timeout, [&]()
            {
                if (num >= 0)
                {
                    enemy_tank->direction = res[num];
                    tank_update(enemy_tank);
                    --num;
                }
                else
                {
                    EnemyTimer->stop();
                }
                EnemyFire();
            });
    EnemyTimer->start(300);
}

// 画子弹
void PlayScene::paintBullet(bullets *bullet)
{
    if (bullet->HitsNum > 3)
    {
        BulletHit(bullet);
    }
    if (!bullet->isHit)
    {
        QPainter painterbullet(this);
        QBrush Brushbullet(Qt::gray);
        painterbullet.setBrush(Brushbullet);
        if (bullet->direction == 1)
        {
            painterbullet.drawEllipse(QPoint(bullet->x, bullet->y), 5, 5);
            if (bullet->update)
            {
                bullet->update = false;
                --bullet->y;
                if (bullet->y < 30)
                {
                    bullet->direction = 2;
                    ++bullet->HitsNum;
                    update();
                }
                BulletHitWall(bullet, bullet->x, bullet->y);
            }
        }
        if (bullet->direction == 2)
        {
            painterbullet.drawEllipse(QPoint(bullet->x, bullet->y), 5, 5);
            if (bullet->update)
            {
                bullet->update = false;
                ++bullet->y;
                if (bullet->y > 695)
                {
                    bullet->direction = 1;
                    ++bullet->HitsNum;
                    update();
                }
                BulletHitWall(bullet, bullet->x, bullet->y);
            }
        }
        if (bullet->direction == 3)
        {
            painterbullet.drawEllipse(QPoint(bullet->x, bullet->y), 5, 5);
            if (bullet->update)
            {
                bullet->update = false;
                --bullet->x;
                if (bullet->x < 30)
                {
                    bullet->direction = 4;
                    ++bullet->HitsNum;
                    update();
                }
                BulletHitWall(bullet, bullet->x, bullet->y);
            }
        }

        if (bullet->direction == 4)
        {
            painterbullet.drawEllipse(QPoint(bullet->x, bullet->y), 5, 5);
            if (bullet->update)
            {
                bullet->update = false;
                ++bullet->x;
                if (bullet->x > 1020)
                {
                    bullet->direction = 3;
                    ++bullet->HitsNum;
                    update();
                }
                BulletHitWall(bullet, bullet->x, bullet->y);
            }
        }
    }
}

// 判断子弹击中墙
void PlayScene::BulletHitWall(bullets *bullet, int x, int y)
{
    if (mainmap[y / 25][x / 25] == 1)
    {
        partBulletHitWall(bullet);
    }
    else if (mainmap[y / 25 - 1][x / 25] == 1)
    {
        partBulletHitWall(bullet);
    }
    else if (mainmap[y / 25][x / 25 - 1] == 1)
    {
        partBulletHitWall(bullet);
    }
    if (mainmap[y / 25][x / 25] == 2)
    {
        mainmap[y / 25][x / 25] = 0;
        tempmap[y / 25][x / 25] = 0;
        BulletHit(bullet);
    }
    else if (mainmap[y / 25 - 1][x / 25] == 2)
    {
        mainmap[y / 25 - 1][x / 25] = 0;
        tempmap[y / 25 - 1][x / 25] = 0;
        BulletHit(bullet);
    }
    else if (mainmap[y / 25][x / 25 - 1] == 2)
    {
        mainmap[y / 25][x / 25 - 1] = 0;
        tempmap[y / 25][x / 25 - 1] = 0;
        BulletHit(bullet);
    }
}
void PlayScene::partBulletHitWall(bullets *bullet)
{
    if (bullet->direction == 1)
    {
        bullet->direction = 2;
    }
    else if (bullet->direction == 2)
    {
        bullet->direction = 1;
    }
    else if (bullet->direction == 3)
    {
        bullet->direction = 4;
    }
    else if (bullet->direction == 4)
    {
        bullet->direction = 3;
    }
    ++bullet->HitsNum;
    update();
    return;
}

// 判断子弹击中坦克
void PlayScene::BulletHitTank(tanks *tank, bullets *bullet)
{
    int x = tank->x * 25;
    int y = tank->y * 25;
    bool f1 = bullet->x > x && bullet->x < x + 50;
    bool f2 = bullet->y > y && bullet->y < y + 50;
    if (f1 && f2 && tank->alive)
    {
        tank->alive = false;
        EnemyTimer->stop();
        reroute->stop();
        BulletHit(bullet);
    }
}

// 子弹击中后停止的处理
void PlayScene::BulletHit(bullets *bullet)
{
    bullet->timer->stop();
    bullet->isHit = true;
    bullet->x = 0;
    bullet->y = 0;
    update();
}

// 坦克死亡效果
void PlayScene::DeathEffect(tanks *tank)
{
    if (!tank->alive)
    {
        if (!tank->DieOnce)
        {
            QMediaPlayer *BoomSound = new QMediaPlayer(this);
            BoomSound->setSource(QUrl("://music/boom.wav"));
            BoomSound->play();
        }
        tank->DieOnce = true;
        QPainter painter(this);
        QBrush Brush(Qt::black);
        painter.setBrush(Brush);
        QPixmap pixmap;
        QString str = QString("://res/blast8.gif");
        pixmap.load(str);
        painter.drawRect(tank->x * 25, tank->y * 25, 50, 50);
        painter.drawPixmap(tank->x * 25, tank->y * 25, 50, 50, pixmap);
    }
}

// 敌方开火
void PlayScene::EnemyFire()
{
    if (enemy_tank->x == my_tank->x)
    {
        if (enemy_tank->y > my_tank->y && enemy_tank->direction == 1)
        {
            EnemyTimer->stop();
            ShootBulletE();
        }
        else if (enemy_tank->y < my_tank->y && enemy_tank->direction == 2)
        {
            EnemyTimer->stop();
            ShootBulletE();
        }
    }
    else if (enemy_tank->y == my_tank->y)
    {
        if (enemy_tank->x > my_tank->x && enemy_tank->direction == 3)
        {
            EnemyTimer->stop();
            ShootBulletE();
        }
        else if (enemy_tank->x < my_tank->x && enemy_tank->direction == 4)
        {
            EnemyTimer->stop();
            ShootBulletE();
        }
    }
}

// 敌方子弹的射出
void PlayScene::ShootBulletE()
{
    if (bullet1E->isHit)
    {
        initBullet(enemy_tank, bullet1E, enemy_tank->direction);
    }
    else if (bullet2E->isHit)
    {
        initBullet(enemy_tank, bullet2E, enemy_tank->direction);
    }
    else if (bullet3E->isHit)
    {
        initBullet(enemy_tank, bullet3E, enemy_tank->direction);
    }
}


// 游戏结束
void PlayScene::GameOver()
{
    if (!my_tank->alive || !enemy_tank->alive)
    {
        bool a = bullet1->isHit && bullet2->isHit && bullet3->isHit;
        bool b = bullet1E->isHit && bullet2E->isHit && bullet3E->isHit;
        if (a && b)
        {
            QPixmap over;
            over.load("://res3/gameover.jpg");
            QPainter overpainter(this);
            overpainter.drawPixmap(this->width() * 0.5 - over.width(), this->height() * 0.17, over.width() * 2, over.height() * 2, over);
            IsGameOver = true;
        }
    }
}

void PlayScene::DodgeBullet()
{
}

// A* 算法实现
vector<int> PlayScene::Astar(vector<vector<int>> tempmap)
{
    int startX = my_tank->x;
    int startY = my_tank->y;
    int goalX = enemy_tank->x;
    int goalY = enemy_tank->y;

    priority_queue<Node, vector<Node>, greater<Node>> openSet;
    vector<vector<bool>> closedSet(tempmap.size(), vector<bool>(tempmap[0].size(), false));

    openSet.push(Node(startX, startY, 0, heuristic(startX, startY, goalX, goalY)));

    while (!openSet.empty())
    {
        Node current = openSet.top();
        openSet.pop();

        if (current.x == goalX && current.y == goalY)
        {
            vector<int> path;
            while (current.parent != nullptr)
            {
                path.push_back(current.x);
                path.push_back(current.y);
                current = *current.parent;
            }
            path.push_back(startX);
            path.push_back(startY);
            reverse(path.begin(), path.end());
            return path;
        }

        closedSet[current.x][current.y] = true;

        // 四个方向：上、下、左、右
        int dx[] = {-1, 1, 0, 0};
        int dy[] = {0, 0, -1, 1};

        for (int i = 0; i < 4; ++i)
        {
            int newX = current.x + dx[i];
            int newY = current.y + dy[i];

            if (newX >= 0 && newX < tempmap.size() && newY >= 0 && newY < tempmap[0].size() &&
                !closedSet[newX][newY] && tempmap[newX][newY] != 1)
            {
                int newG = current.g + 1;
                Node neighbor(newX, newY, newG, heuristic(newX, newY, goalX, goalY), new Node(current.x, current.y, current.g, current.h, current.parent));
                openSet.push(neighbor);
            }
        }
    }

    return {}; // 没有找到路径
}