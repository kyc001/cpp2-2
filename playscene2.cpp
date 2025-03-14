#include "playscene2.h"
#include <QPainter>
#include <QBrush>
#include <QString>
#include <QKeyEvent>
#include <QTimer>
#include <QDebug>
#include <QMediaPlayer>

using namespace std;
// 上1下2左3右4
vector<vector<int>> mainmap2 = {
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
PlayScene2::PlayScene2(QWidget *parent) : QMainWindow(parent)
{
    mainmap2 = forevermap;
    // 设置窗口固定大小
    setFixedSize(1050, 725);
    // 设置标题
    setWindowTitle("坦克大战");
    // 设置图标
    setWindowIcon(QIcon("://res/tank_up.jpg"));
    // 返回按钮
    backBtn2 = new button("://res3/back.tga");
    backBtn2->setParent(this);
    backBtn2->move(1050, 750);
    connect(backBtn2, &button::clicked, [=]()
            {
        backBtn2->pop1();
        backBtn2->pop2();
        QTimer::singleShot(500,this,[=](){
            emit this->playScene2Back();
        }); });
}
// 画基本地图 画坦克
void PlayScene2::paintEvent(QPaintEvent *event)
{
    if (IsGameOver && !Btnmove2)
    {
        Btnmove2 = true;
        backBtn2->move(this->width() * 0.5 - backBtn2->width() * 0.5, this->height() * 0.75);
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
    if (!your_tank->isInit)
    {
        initYour_Tank();
        your_tank->isInit = true;
    }
    // 加载四个方向敌方tank图片
    QPixmap pixUpE;
    pixUpE.load("://res/p2tankU.gif");
    QPixmap pixDownE;
    pixDownE.load("://res/p2tankD.gif");
    QPixmap pixLeftE;
    pixLeftE.load("://res/p2tankL.gif");
    QPixmap pixRightE;
    pixRightE.load("://res/p2tankR.gif");
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
            if (mainmap2[i][j] == 1)
            {
                painter.drawPixmap(25 * j, 25 * i, 25, 25, pix1);
            }
            if (mainmap2[i][j] == 2)
            {
                painter.drawPixmap(25 * j, 25 * i, 25, 25, pix2);
            }
            if (mainmap2[i][j] == 204)
            {
                painter.drawPixmap(25 * j, 25 * i, 50, 50, pixRight);
            }
            if (mainmap2[i][j] == 203)
            {
                painter.drawPixmap(25 * j, 25 * i, 50, 50, pixLeft);
            }
            if (mainmap2[i][j] == 202)
            {
                painter.drawPixmap(25 * j, 25 * i, 50, 50, pixDown);
            }
            if (mainmap2[i][j] == 201)
            {
                painter.drawPixmap(25 * j, 25 * i, 50, 50, pixUp);
            }
            if (mainmap2[i][j] == 304)
            {
                painter.drawPixmap(25 * j, 25 * i, 50, 50, pixRightE);
            }
            if (mainmap2[i][j] == 303)
            {
                painter.drawPixmap(25 * j, 25 * i, 50, 50, pixLeftE);
            }
            if (mainmap2[i][j] == 302)
            {
                painter.drawPixmap(25 * j, 25 * i, 50, 50, pixDownE);
            }
            if (mainmap2[i][j] == 301)
            {
                painter.drawPixmap(25 * j, 25 * i, 50, 50, pixUpE);
            }
        }
    }
    // 判断我方坦克是不是还活着

    DeathEffect(my_tank);
    DeathEffect(your_tank);

    // 画子弹 同时判断子弹有没有击中tank
    paintBullet(bullet1);
    BulletHitTank(your_tank, bullet1);
    BulletHitTank(my_tank, bullet1);
    paintBullet(bullet2);
    BulletHitTank(your_tank, bullet2);
    BulletHitTank(my_tank, bullet2);
    paintBullet(bullet3);
    BulletHitTank(your_tank, bullet3);
    BulletHitTank(my_tank, bullet3);

    paintBullet(bullet1Y);
    BulletHitTank(your_tank, bullet1Y);
    BulletHitTank(my_tank, bullet1Y);
    paintBullet(bullet2Y);
    BulletHitTank(your_tank, bullet2Y);
    BulletHitTank(my_tank, bullet2Y);
    paintBullet(bullet3Y);
    BulletHitTank(your_tank, bullet3Y);
    BulletHitTank(my_tank, bullet3Y);

    GameOver();
}

// 初始化我方tank
void PlayScene2::initMy_Tank()
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
void PlayScene2::initYour_Tank()
{
    your_tank->x = 35;
    your_tank->y = 20;
    your_tank->direction = 3;
    your_tank->alive = true;
    your_tank->IsGood = false;
    // 初始化敌方tank的位置
    LocationSetting(your_tank, your_tank->direction);
    movespeed2 = new QTimer(this);
    connect(movespeed2, &QTimer::timeout, [=]()
            { SpeedControl2 = true; });
    movespeed2->start(200);
}
// 初始化子弹
void PlayScene2::initBullet(tanks *tank, bullets *bullet, int direction)
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
void PlayScene2::tank_update(tanks *tank)
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
bool PlayScene2::checkTankIsMoveable(tanks *tank)
{
    if (tank->direction == 1)
    {
        if (mainmap2[tank->y - 1][tank->x] != 0)
        {
            return false;
        }
        if (mainmap2[tank->y - 1][tank->x + 1] != 0)
        {
            return false;
        }
    }
    if (tank->direction == 2)
    {
        if (mainmap2[tank->y + 2][tank->x] != 0)
        {
            return false;
        }
        if (mainmap2[tank->y + 2][tank->x + 1] != 0)
        {
            return false;
        }
    }
    if (tank->direction == 3)
    {
        if (mainmap2[tank->y][tank->x - 1] != 0)
        {
            return false;
        }
        if (mainmap2[tank->y + 1][tank->x - 1] != 0)
        {
            return false;
        }
    }
    if (tank->direction == 4)
    {
        if (mainmap2[tank->y][tank->x + 2] != 0)
        {
            return false;
        }
        if (mainmap2[tank->y + 1][tank->x + 2] != 0)
        {
            return false;
        }
    }
    return true;
}

// 设置坦克的新位置。
void PlayScene2::SetTankNewLocation(tanks *tank, int direction)
{
    mainmap2[tank->y][tank->x] = 0;
    mainmap2[tank->y + 1][tank->x] = 0;
    mainmap2[tank->y][tank->x + 1] = 0;
    mainmap2[tank->y + 1][tank->x + 1] = 0;
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
void PlayScene2::LocationSetting(tanks *tank, int direction)
{
    int a = 200;
    if (!tank->IsGood)
    {
        a = 300;
    }
    mainmap2[tank->y][tank->x] = a + direction;
    mainmap2[tank->y + 1][tank->x] = a;
    mainmap2[tank->y][tank->x + 1] = a;
    mainmap2[tank->y + 1][tank->x + 1] = a;
}

// 我方坦克的移动-键盘事件控制
void PlayScene2::keyPressEvent(QKeyEvent *event)
{
    if (my_tank->alive)
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
    if (your_tank->alive)
    {
        if (event->key() == Qt::Key_Up && SpeedControl2)
        {
            SpeedControl2 = false;
            if (your_tank->direction != 1)
            {
                LocationSetting(your_tank, 1);
                update();
                your_tank->direction = 1;
                return;
            }
            if (checkTankIsMoveable(your_tank))
            {
                tank_update(your_tank);
            }
        }
        if (event->key() == Qt::Key_Down && SpeedControl2)
        {
            SpeedControl2 = false;
            if (your_tank->direction != 2)
            {
                LocationSetting(your_tank, 2);
                update();
                your_tank->direction = 2;
                return;
            }
            if (checkTankIsMoveable(your_tank))
            {
                tank_update(your_tank);
            }
        }
        if (event->key() == Qt::Key_Left && SpeedControl2)
        {
            SpeedControl2 = false;
            if (your_tank->direction != 3)
            {
                LocationSetting(your_tank, 3);
                update();
                your_tank->direction = 3;
                return;
            }
            if (checkTankIsMoveable(your_tank))
            {
                tank_update(your_tank);
            }
        }
        if (event->key() == Qt::Key_Right && SpeedControl2)
        {
            SpeedControl2 = false;
            if (your_tank->direction != 4)
            {
                LocationSetting(your_tank, 4);
                update();
                your_tank->direction = 4;
                return;
            }
            if (checkTankIsMoveable(your_tank))
            {
                tank_update(your_tank);
            }
        }
        if (event->key() == Qt::Key_End)
        {
            if (bullet1Y->isHit)
            {
                initBullet(your_tank, bullet1Y, your_tank->direction);
            }
            else if (bullet2Y->isHit)
            {
                initBullet(your_tank, bullet2Y, your_tank->direction);
            }
            else if (bullet3Y->isHit)
            {
                initBullet(your_tank, bullet3Y, your_tank->direction);
            }
        }
    }
    QWidget::keyPressEvent(event);
}
// 画子弹
void PlayScene2::paintBullet(bullets *bullet)
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
void PlayScene2::BulletHitWall(bullets *bullet, int x, int y)
{
    if (mainmap2[y / 25][x / 25] == 1)
    {
        partBulletHitWall(bullet);
    }
    else if (mainmap2[y / 25 - 1][x / 25] == 1)
    {
        partBulletHitWall(bullet);
    }
    else if (mainmap2[y / 25][x / 25 - 1] == 1)
    {
        partBulletHitWall(bullet);
    }
    if (mainmap2[y / 25][x / 25] == 2)
    {
        mainmap2[y / 25][x / 25] = 0;

        BulletHit(bullet);
    }
    else if (mainmap2[y / 25 - 1][x / 25] == 2)
    {
        mainmap2[y / 25 - 1][x / 25] = 0;

        BulletHit(bullet);
    }
    else if (mainmap2[y / 25][x / 25 - 1] == 2)
    {
        mainmap2[y / 25][x / 25 - 1] = 0;

        BulletHit(bullet);
    }
}
void PlayScene2::partBulletHitWall(bullets *bullet)
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
void PlayScene2::BulletHitTank(tanks *tank, bullets *bullet)
{
    int x = tank->x * 25;
    int y = tank->y * 25;
    bool f1 = bullet->x > x && bullet->x < x + 50;
    bool f2 = bullet->y > y && bullet->y < y + 50;
    if (f1 && f2 && tank->alive)
    {
        tank->alive = false;
        BulletHit(bullet);
    }
}

// 子弹击中后停止的处理
void PlayScene2::BulletHit(bullets *bullet)
{
    bullet->timer->stop();
    bullet->isHit = true;
    bullet->x = 0;
    bullet->y = 0;
    update();
}

// 坦克死亡效果
void PlayScene2::DeathEffect(tanks *tank)
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

// 游戏结束
void PlayScene2::GameOver()
{
    if (!my_tank->alive || !your_tank->alive)
    {
        bool a = bullet1->isHit && bullet2->isHit && bullet3->isHit;
        bool b = bullet1Y->isHit && bullet2Y->isHit && bullet3Y->isHit;
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
