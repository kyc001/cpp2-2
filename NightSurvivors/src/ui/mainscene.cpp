#include "../../include/ui/mainscene.h"
#include "../../include/entities/drop.h"
#include "../../forms/ui_mainscene.h"
#include "../../include/core/gamestate.h"
#include "../../include/entities/hero.h"
#include "../../include/entities/enemy.h"
#include "../../include/core/gamemap.h"
#include "../../include/core/weapon.h"
#include "../../include/ui/upgradeui.h"
#include "../../include/ui/shopui.h"
#include "../../include/ui/saveui.h"
#include "../../include/ui/settingsui.h"
#include "../../include/ui/introductionui.h"
#include "../../include/ui/gamemenuui.h"
#include "../../include/utils/resourcemanager.h"
#include <QPainter>
#include <QTimer>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QFont>
#include <QKeyEvent>
#include <QCloseEvent>
#include <QVector>
#include <QSettings>
#include <QResizeEvent>
#include <QFontDatabase>
#include <cmath>

MainScene::MainScene(QWidget *parent) : QMainWindow(parent),
                                        ui(new Ui::MainScene), game_state(new GameState(this)), timer_id(0),
                                        is_game_running(false), is_game_paused(false), screen_width(1024), screen_height(768),
                                        game_volume(50), control_type(0)
{
    ui->setupUi(this);

    // Set window title and size
    setWindowTitle("暗夜幸存者");
    resize(screen_width, screen_height);
    setMinimumSize(800, 600);

    // Set focus policy
    setFocusPolicy(Qt::StrongFocus);

    // Create game state
    game_state = new GameState(this);

    // Connect signals
    connect(game_state, &GameState::gameOverSignal, this, &MainScene::onGameOver);
    connect(game_state, &GameState::scoreUpdated, this, &MainScene::onScoreUpdated);
    connect(game_state, &GameState::timeUpdated, this, &MainScene::onTimeUpdated);
    connect(game_state, &GameState::coinsUpdated, this, &MainScene::onCoinsUpdated);
    connect(game_state, &GameState::levelUpOptionsReady, this, &MainScene::onHeroLeveledUp);
    connect(game_state, &GameState::messageSignal, this, &MainScene::showGameMessage);

    // Setup UI
    setupUI();
    setupMainMenu();

    // 加载资源
    loadResources();

    // 初始显示主菜单，而不是角色选择
    showMainMenu();

    // 初始化按键状态
    for (int i = 0; i < 4; ++i)
    {
        keys[i] = false;
    }
}

MainScene::~MainScene()
{
    if (timer_id > 0)
    {
        killTimer(timer_id);
    }

    delete game_state;
    delete ui;
}

void MainScene::setupUI()
{
    // Create score and time labels
    score_label = new QLabel("分数: 0", this);
    score_label->setGeometry(10, 10, 200, 30);
    score_label->setStyleSheet("color: white; font-size: 16px;");
    score_label->hide();

    time_label = new QLabel("时间: 0", this);
    time_label->setGeometry(10, 40, 200, 30);
    time_label->setStyleSheet("color: white; font-size: 16px;");
    time_label->hide();

    coin_label = new QLabel("金币: 0", this);
    coin_label->setGeometry(10, 70, 200, 30);
    coin_label->setStyleSheet("color: gold; font-size: 16px;");
    coin_label->hide();

    // Create character selection UI
    character_selection = new QWidget(this);
    character_selection->setGeometry(0, 0, width(), height());
    character_selection->setStyleSheet("background-color: rgba(0, 0, 0, 180);");
    character_selection->hide();

    QVBoxLayout *char_layout = new QVBoxLayout(character_selection);

    character_title = new QLabel("选择角色", character_selection);
    character_title->setAlignment(Qt::AlignCenter);
    character_title->setStyleSheet("color: white; font-size: 24px; font-weight: bold;");

    QHBoxLayout *buttons_layout = new QHBoxLayout();

    // 仅创建战士角色按钮
    QPushButton *warriorBtn = new QPushButton("战士", character_selection);
    warriorBtn->setMinimumSize(120, 120);
    connect(warriorBtn, &QPushButton::clicked, this, [this]()
            { onCharacterSelected(0); });
    buttons_layout->addWidget(warriorBtn);
    character_buttons.append(warriorBtn);

    char_layout->addWidget(character_title);
    char_layout->addLayout(buttons_layout);

    // Create game over screen
    game_over_screen = new QWidget(this);
    game_over_screen->setGeometry(0, 0, width(), height());
    game_over_screen->setStyleSheet("background-color: rgba(0, 0, 0, 180);");
    game_over_screen->hide();

    QVBoxLayout *go_layout = new QVBoxLayout(game_over_screen);

    QLabel *go_title = new QLabel("游戏结束", game_over_screen);
    go_title->setAlignment(Qt::AlignCenter);
    go_title->setStyleSheet("color: white; font-size: 32px; font-weight: bold;");

    final_score_label = new QLabel("最终分数: 0", game_over_screen);
    final_score_label->setAlignment(Qt::AlignCenter);
    final_score_label->setStyleSheet("color: white; font-size: 24px;");

    restart_button = new QPushButton("重新开始", game_over_screen);
    restart_button->setMinimumSize(120, 40);
    connect(restart_button, &QPushButton::clicked, this, &MainScene::restartGame);

    // 添加返回主界面按钮
    return_to_menu_button = new QPushButton("返回主界面", game_over_screen);
    return_to_menu_button->setMinimumSize(120, 40);
    connect(return_to_menu_button, &QPushButton::clicked, this, &MainScene::returnToMainMenu);

    go_layout->addWidget(go_title);
    go_layout->addWidget(final_score_label);
    go_layout->addWidget(restart_button, 0, Qt::AlignCenter);
    go_layout->addWidget(return_to_menu_button, 0, Qt::AlignCenter);

    // Create shop button
    shop_button = new QPushButton("商店", this);
    shop_button->setGeometry(700, 10, 80, 30);
    shop_button->setStyleSheet("background-color: rgba(70, 70, 70, 180); color: white; border: none; border-radius: 5px;");
    connect(shop_button, &QPushButton::clicked, this, &MainScene::onShopButtonClicked);
    shop_button->hide();

    // 创建存档按钮
    save_button = new QPushButton("存档", this);
    save_button->setGeometry(700, 50, 80, 30);
    save_button->setStyleSheet("background-color: rgba(70, 70, 70, 180); color: white; border: none; border-radius: 5px;");
    connect(save_button, &QPushButton::clicked, this, &MainScene::onSaveButtonClicked);
    save_button->hide();

    // 创建各种UI组件
    upgrade_ui = new UpgradeUI(this);
    shop_ui = new ShopUI(this);
    save_ui = new SaveUI(this);
    settings_ui = new SettingsUI(this);
    introduction_ui = new IntroductionUI(this);
    game_menu_ui = new GameMenuUI(this);

    // 连接信号
    connect(shop_button, &QPushButton::clicked, this, &MainScene::onShopButtonClicked);
    connect(save_button, &QPushButton::clicked, this, &MainScene::onSaveButtonClicked);

    // 连接UI子组件的信号
    connect(shop_ui, &ShopUI::shopClosed, this, &MainScene::onShopClosed);
    connect(save_ui, &SaveUI::saveClosed, this, &MainScene::onSaveClosed);
    connect(settings_ui, &SettingsUI::settingsClosed, this, &MainScene::onSettingsClosed);
    connect(introduction_ui, &IntroductionUI::introClosed, this, &MainScene::onIntroClosed);
    connect(game_menu_ui, &GameMenuUI::resumeClicked, this, &MainScene::onGameMenuResumeClicked);
    connect(game_menu_ui, &GameMenuUI::settingsClicked, this, &MainScene::onGameMenuSettingsClicked);
    connect(game_menu_ui, &GameMenuUI::saveClicked, this, &MainScene::onGameMenuSaveClicked);
    connect(game_menu_ui, &GameMenuUI::mainMenuClicked, this, &MainScene::onGameMenuMainMenuClicked);
    connect(game_menu_ui, &GameMenuUI::quitClicked, this, &MainScene::onGameMenuQuitClicked);
    connect(upgrade_ui, &UpgradeUI::upgradeSelected, this, [this]()
            { resumeGame(); });

    // 设置主菜单
    setupMainMenu();

    // 添加消息标签 - 用于显示游戏中的提示和增益信息
    message_label = new QLabel(this);
    message_label->setGeometry(width() / 2 - 150, 50, 300, 40);
    message_label->setAlignment(Qt::AlignCenter);
    message_label->setStyleSheet("color: #0f0; font-size: 16px; font-weight: bold; background-color: rgba(0, 0, 0, 150); border-radius: 5px;");
    message_label->hide();
}

void MainScene::showMainMenu()
{
    // 隐藏游戏UI
    hideGameUI();

    // 显示主菜单
    main_menu->show();

    // 停止游戏计时器
    if (timer_id > 0)
    {
        killTimer(timer_id);
        timer_id = 0;
    }

    // 重置游戏状态
    is_game_running = false;
    is_game_paused = false;
}

void MainScene::hideMainMenu()
{
    main_menu->hide();
}

void MainScene::showGameUI()
{
    score_label->show();
    time_label->show();
    coin_label->show();
    shop_button->show();
    save_button->show();
}

void MainScene::hideGameUI()
{
    score_label->hide();
    time_label->hide();
    coin_label->hide();
    shop_button->hide();
    save_button->hide();
    character_selection->hide();
    game_over_screen->hide();
}

void MainScene::showCharacterSelection()
{
    hideMainMenu();
    character_selection->show();

    if (timer_id > 0)
    {
        killTimer(timer_id);
        timer_id = 0;
    }
}

void MainScene::hideCharacterSelection()
{
    character_selection->hide();
}

void MainScene::showGameOverScreen()
{
    game_over_screen->show();
}

void MainScene::hideGameOverScreen()
{
    game_over_screen->hide();
}

void MainScene::onCharacterSelected(int character_id)
{
    if (!game_state)
    {
        return;
    }

    // 选择角色
    game_state->selectCharacter(character_id);

    // 在此处先隐藏角色选择
    character_selection->hide();

    // 确保game_state处于准备状态，然后开始游戏
    startGame();
}

void MainScene::startGame()
{
    if (!game_state)
    {
        return;
    }

    // 初始化游戏状态
    game_state->init();

    // 设置300秒倒计时
    game_state->setMaxGameTime(300);

    // 启动游戏
    game_state->start();

    // 激活游戏运行状态
    is_game_running = true;
    is_game_paused = false;

    // 隐藏菜单UI
    hideAllMenus();

    // 开始定时器
    timer_id = startTimer(16); // ~60fps
}

void MainScene::restartGame()
{
    hideGameOverScreen();
    showCharacterSelection();
}

void MainScene::onGameOver()
{
    // 获取并显示最终分数
    int final_score = 0;
    if (game_state)
    {
        final_score = game_state->getScore();
    }
    final_score_label->setText(QString("最终分数: %1").arg(final_score));
    showGameOverScreen();

    // 更新游戏状态
    is_game_running = false;
}

void MainScene::onScoreUpdated(int score)
{
    score_label->setText(QString("分数: %1").arg(score));
}

void MainScene::onTimeUpdated(int time)
{
    // 显示倒计时格式 (mm:ss)
    int minutes = time / 60;
    int seconds = time % 60;
    time_label->setText(QString("剩余时间: %1:%2")
                            .arg(minutes, 2, 10, QChar('0'))
                            .arg(seconds, 2, 10, QChar('0')));
}

void MainScene::onCoinsUpdated(int coins)
{
    coin_label->setText(QString("金币: %1").arg(coins));
}

void MainScene::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (is_game_running && !is_game_paused && game_state)
    {
        // 先渲染地图
        renderMap(painter);

        // 渲染掉落物
        if (game_state->getDrops().size() > 0)
        {
            renderDrops(painter);
        }

        // 渲染敌人
        renderEnemies(painter);

        // 渲染英雄
        renderHero(painter);

        // 渲染简易的子弹 (如果英雄有武器)
        Hero *hero = game_state->getHero();
        if (hero && hero->isAlive())
        {
            renderSimpleProjectiles(painter);
        }
    }
}

void MainScene::renderMap(QPainter &painter)
{
    if (!game_state)
    {
        return;
    }

    GameMap *map = game_state->getMap();
    Hero *hero = game_state->getHero();

    if (!map || !hero)
    {
        return;
    }

    // 获取视窗大小
    int screen_width = width();
    int screen_height = height();

    // 获取英雄位置（屏幕中心）
    int hero_x = hero->getX();
    int hero_y = hero->getY();

    // 获取地图尺寸
    int map_width = map->getWidth();
    int map_height = map->getHeight();

    // 绘制像素风背景
    // 绘制草地背景
    for (int x = 0; x < map_width; x += 32)
    {
        for (int y = 0; y < map_height; y += 32)
        {
            // 计算屏幕上的位置
            int screen_x = screen_width / 2 + (x - hero_x);
            int screen_y = screen_height / 2 + (y - hero_y);

            // 检查是否在屏幕范围内
            if (screen_x < -32 || screen_x > screen_width + 32 ||
                screen_y < -32 || screen_y > screen_height + 32)
            {
                continue;
            }

            // 根据位置生成不同深浅的草地颜色
            int colorVariation = ((x / 32 + y / 32) % 3);
            QColor grassColor;

            switch (colorVariation)
            {
            case 0:
                grassColor = QColor(40, 140, 40);
                break;
            case 1:
                grassColor = QColor(50, 150, 50);
                break;
            case 2:
                grassColor = QColor(30, 130, 30);
                break;
            }

            // 绘制草地块
            painter.fillRect(screen_x, screen_y, 32, 32, grassColor);

            // 添加简单的像素风格装饰
            painter.setPen(QColor(60, 160, 60));
            painter.drawLine(screen_x + 8, screen_y + 16, screen_x + 8, screen_y + 24);
            painter.drawLine(screen_x + 24, screen_y + 8, screen_x + 24, screen_y + 16);
        }
    }

    // 绘制障碍物
    const QVector<QRect> &obstacles = map->getObstacles();
    for (const QRect &obstacle : obstacles)
    {
        // 计算屏幕上的位置
        int screen_x = screen_width / 2 + (obstacle.x() - hero_x);
        int screen_y = screen_height / 2 + (obstacle.y() - hero_y);

        // 检查是否在屏幕范围内
        if (screen_x < -obstacle.width() || screen_x > screen_width ||
            screen_y < -obstacle.height() || screen_y > screen_height)
        {
            continue;
        }

        // 不同类型的障碍物用不同颜色和图案表示
        int obstacleType = (obstacle.x() + obstacle.y()) % 3;

        switch (obstacleType)
        {
        case 0:
        { // 石头
            // 绘制石头底色
            QColor stoneColor(100, 100, 100);
            painter.fillRect(screen_x, screen_y, obstacle.width(), obstacle.height(), stoneColor);

            // 添加细节
            painter.setPen(QColor(80, 80, 80));
            painter.drawLine(screen_x + 5, screen_y + 5, screen_x + 15, screen_y + 10);
            painter.drawLine(screen_x + 20, screen_y + 15, screen_x + 25, screen_y + 20);
            painter.setPen(QColor(120, 120, 120));
            painter.drawLine(screen_x + 10, screen_y + 25, screen_x + 20, screen_y + 30);
            break;
        }
        case 1:
        { // 树
            // 绘制树干
            QColor trunkColor(100, 70, 40);
            painter.fillRect(screen_x + 12, screen_y + 20, 8, 12, trunkColor);

            // 绘制树冠
            QColor leafColor(30, 100, 30);
            painter.setBrush(leafColor);
            painter.setPen(Qt::NoPen);
            painter.drawEllipse(screen_x + 6, screen_y + 5, 20, 20);

            // 添加细节
            painter.setPen(QColor(20, 80, 20));
            painter.drawLine(screen_x + 10, screen_y + 10, screen_x + 15, screen_y + 8);
            painter.drawLine(screen_x + 20, screen_y + 15, screen_x + 25, screen_y + 12);
            break;
        }
        case 2:
        { // 灌木
            // 绘制灌木
            QColor bushColor(50, 120, 50);
            painter.setBrush(bushColor);
            painter.setPen(Qt::NoPen);

            // 绘制几个重叠的圆形创建灌木形状
            painter.drawEllipse(screen_x + 5, screen_y + 10, 15, 15);
            painter.drawEllipse(screen_x + 15, screen_y + 8, 16, 16);
            painter.drawEllipse(screen_x + 10, screen_y + 15, 14, 14);

            // 添加细节
            painter.setPen(QColor(40, 100, 40));
            painter.drawLine(screen_x + 12, screen_y + 12, screen_x + 18, screen_y + 10);
            painter.drawLine(screen_x + 20, screen_y + 15, screen_x + 25, screen_y + 14);
            break;
        }
        }
    }
}

void MainScene::renderHero(QPainter &painter)
{
    if (!game_state)
    {
        return;
    }

    Hero *hero = game_state->getHero();
    if (!hero)
    {
        return;
    }

    // 英雄始终在屏幕中心
    int centerX = width() / 2;
    int centerY = height() / 2;

    // 获取英雄当前方向和状态
    int characterType = hero->getCharacterId();

    // 绘制像素风格英雄
    switch (characterType)
    {
    case 0:
    { // 战士
        // 绘制身体
        QColor bodyColor(200, 50, 50); // 红色战士
        painter.fillRect(centerX - 10, centerY - 10, 20, 20, bodyColor);

        // 绘制头盔
        QColor helmetColor(150, 150, 150);
        painter.fillRect(centerX - 8, centerY - 15, 16, 5, helmetColor);

        // 绘制武器
        QColor weaponColor(180, 180, 180);
        painter.fillRect(centerX + 10, centerY - 5, 15, 4, weaponColor);

        // 绘制盾牌
        QColor shieldColor(100, 100, 200);
        painter.fillRect(centerX - 15, centerY - 5, 5, 10, shieldColor);

        // 添加像素风格细节
        painter.setPen(Qt::black);
        painter.drawRect(centerX - 10, centerY - 10, 20, 20); // 身体轮廓
        painter.drawRect(centerX - 8, centerY - 15, 16, 5);   // 头盔轮廓

        // 绘制眼睛
        painter.fillRect(centerX - 5, centerY - 5, 2, 2, Qt::white);
        painter.fillRect(centerX + 3, centerY - 5, 2, 2, Qt::white);

        break;
    }
    default:
    { // 默认角色
        // 绘制身体
        QColor bodyColor(50, 150, 200); // 蓝色默认
        painter.fillRect(centerX - 10, centerY - 10, 20, 20, bodyColor);

        // 绘制头部
        QColor headColor(200, 180, 160);
        painter.fillRect(centerX - 8, centerY - 15, 16, 5, headColor);

        // 添加像素风格细节
        painter.setPen(Qt::black);
        painter.drawRect(centerX - 10, centerY - 10, 20, 20); // 身体轮廓
        painter.drawRect(centerX - 8, centerY - 15, 16, 5);   // 头部轮廓

        // 绘制眼睛
        painter.fillRect(centerX - 5, centerY - 5, 2, 2, Qt::white);
        painter.fillRect(centerX + 3, centerY - 5, 2, 2, Qt::white);

        break;
    }
    }

    // 绘制血条背景
    int healthBarWidth = 30;
    int healthBarHeight = 4;
    painter.fillRect(centerX - healthBarWidth / 2, centerY - 25, healthBarWidth, healthBarHeight, Qt::darkGray);

    // 绘制血条
    int currentHealth = hero->getHP();
    int maxHealth = hero->getMaxHP();
    double healthRatio = static_cast<double>(currentHealth) / maxHealth;
    int currentHealthWidth = static_cast<int>(healthBarWidth * healthRatio);

    painter.fillRect(centerX - healthBarWidth / 2, centerY - 25, currentHealthWidth, healthBarHeight, Qt::red);
}

void MainScene::renderEnemies(QPainter &painter)
{
    if (!game_state)
    {
        return;
    }

    // 获取英雄和敌人列表
    Hero *hero = game_state->getHero();
    QVector<Enemy *> enemies = game_state->getEnemies();

    if (!hero)
    {
        return;
    }

    // 获取英雄位置和屏幕尺寸
    int heroX = hero->getX();
    int heroY = hero->getY();
    int screenWidth = width();
    int screenHeight = height();

    // 绘制所有敌人
    for (Enemy *enemy : enemies)
    {
        if (!enemy || !enemy->isActive())
        {
            continue;
        }

        // 获取敌人位置
        int enemyX = enemy->getX();
        int enemyY = enemy->getY();

        // 计算敌人在屏幕上的相对位置
        int screenX = screenWidth / 2 + (enemyX - heroX);
        int screenY = screenHeight / 2 + (enemyY - heroY);

        // 检查是否在屏幕范围内
        if (screenX < -30 || screenX > screenWidth + 30 ||
            screenY < -30 || screenY > screenHeight + 30)
        {
            continue;
        }

        // 获取敌人类型
        int enemyType = enemy->getType();

        // 绘制不同类型的敌人
        switch (enemyType)
        {
        case 0:
        { // 普通敌人
            // 绘制身体
            QColor bodyColor(150, 20, 20);
            painter.fillRect(screenX - 10, screenY - 10, 20, 20, bodyColor);

            // 绘制眼睛
            painter.fillRect(screenX - 6, screenY - 6, 3, 3, Qt::yellow);
            painter.fillRect(screenX + 3, screenY - 6, 3, 3, Qt::yellow);

            // 绘制轮廓
            painter.setPen(Qt::black);
            painter.drawRect(screenX - 10, screenY - 10, 20, 20);
            break;
        }
        case 1:
        { // 快速敌人
            // 绘制身体
            QColor bodyColor(200, 100, 0);
            painter.fillRect(screenX - 8, screenY - 8, 16, 16, bodyColor);

            // 绘制眼睛
            painter.fillRect(screenX - 4, screenY - 4, 2, 2, Qt::yellow);
            painter.fillRect(screenX + 2, screenY - 4, 2, 2, Qt::yellow);

            // 绘制轮廓
            painter.setPen(Qt::black);
            painter.drawRect(screenX - 8, screenY - 8, 16, 16);
            break;
        }
        case 2:
        { // 强壮敌人
            // 绘制身体
            QColor bodyColor(100, 20, 100);
            painter.fillRect(screenX - 12, screenY - 12, 24, 24, bodyColor);

            // 绘制眼睛
            painter.fillRect(screenX - 7, screenY - 7, 4, 4, Qt::yellow);
            painter.fillRect(screenX + 3, screenY - 7, 4, 4, Qt::yellow);

            // 绘制轮廓
            painter.setPen(Qt::black);
            painter.drawRect(screenX - 12, screenY - 12, 24, 24);
            break;
        }
        case 3:
        { // Boss敌人
            // 绘制身体
            QColor bodyColor(50, 50, 100);
            painter.fillRect(screenX - 15, screenY - 15, 30, 30, bodyColor);

            // 绘制角
            painter.fillRect(screenX - 15, screenY - 20, 5, 5, QColor(30, 30, 70));
            painter.fillRect(screenX + 10, screenY - 20, 5, 5, QColor(30, 30, 70));

            // 绘制眼睛
            painter.fillRect(screenX - 8, screenY - 8, 5, 5, Qt::red);
            painter.fillRect(screenX + 3, screenY - 8, 5, 5, Qt::red);

            // 绘制轮廓
            painter.setPen(Qt::black);
            painter.drawRect(screenX - 15, screenY - 15, 30, 30);
            break;
        }
        default:
        {
            // 绘制通用敌人
            QColor bodyColor(100, 100, 100);
            painter.fillRect(screenX - 10, screenY - 10, 20, 20, bodyColor);

            // 绘制眼睛
            painter.fillRect(screenX - 6, screenY - 6, 3, 3, Qt::white);
            painter.fillRect(screenX + 3, screenY - 6, 3, 3, Qt::white);

            // 绘制轮廓
            painter.setPen(Qt::black);
            painter.drawRect(screenX - 10, screenY - 10, 20, 20);
        }
        }

        // 绘制血条背景
        int healthBarWidth = 20;
        int healthBarHeight = 3;
        painter.fillRect(screenX - healthBarWidth / 2, screenY - 20, healthBarWidth, healthBarHeight, Qt::darkGray);

        // 绘制血条
        int currentHealth = enemy->getHP();
        int maxHealth = enemy->getMaxHP();
        double healthRatio = static_cast<double>(currentHealth) / maxHealth;
        int currentHealthWidth = static_cast<int>(healthBarWidth * healthRatio);

        painter.fillRect(screenX - healthBarWidth / 2, screenY - 20, currentHealthWidth, healthBarHeight, Qt::red);
    }
}

void MainScene::keyPressEvent(QKeyEvent *event)
{
    if (is_game_running && !is_game_paused)
    {
        Hero *hero = game_state ? game_state->getHero() : nullptr;
        if (hero)
        {
            // 处理移动键
            switch (event->key())
            {
            case Qt::Key_W:
            case Qt::Key_Up:
                keys[0] = true; // Up
                hero->processKeyPress(event->key());
                break;

            case Qt::Key_D:
            case Qt::Key_Right:
                keys[1] = true; // Right
                hero->processKeyPress(event->key());
                break;

            case Qt::Key_S:
            case Qt::Key_Down:
                keys[2] = true; // Down
                hero->processKeyPress(event->key());
                break;

            case Qt::Key_A:
            case Qt::Key_Left:
                keys[3] = true; // Left
                hero->processKeyPress(event->key());
                break;

            case Qt::Key_Space:
                // 暂停/继续游戏
                if (is_game_running)
                {
                    toggleGameMenu();
                }
                break;

            case Qt::Key_Escape:
                // 打开游戏菜单
                toggleGameMenu();
                break;
            }
        }
    }
    else
    {
        // 菜单或暂停状态
        if (event->key() == Qt::Key_Escape)
        {
            // 如果游戏暂停，继续游戏
            if (is_game_running && is_game_paused)
            {
                resumeGame();
            }
        }
    }
}

void MainScene::keyReleaseEvent(QKeyEvent *event)
{
    if (is_game_running && !is_game_paused)
    {
        Hero *hero = game_state ? game_state->getHero() : nullptr;
        if (hero)
        {
            // 处理移动键
            switch (event->key())
            {
            case Qt::Key_W:
            case Qt::Key_Up:
                keys[0] = false; // Up
                hero->processKeyRelease(event->key());
                break;

            case Qt::Key_D:
            case Qt::Key_Right:
                keys[1] = false; // Right
                hero->processKeyRelease(event->key());
                break;

            case Qt::Key_S:
            case Qt::Key_Down:
                keys[2] = false; // Down
                hero->processKeyRelease(event->key());
                break;

            case Qt::Key_A:
            case Qt::Key_Left:
                keys[3] = false; // Left
                hero->processKeyRelease(event->key());
                break;
            }
        }
    }
}

void MainScene::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == timer_id && is_game_running && !is_game_paused)
    {
        // 更新游戏状态
        if (game_state)
        {
            game_state->update();
        }

        // 重绘场景
        update();
    }

    QMainWindow::timerEvent(event);
}

void MainScene::toggleGameMenu()
{
    if (game_menu_ui->isVisible())
    {
        game_menu_ui->hide();
        resumeGame();
    }
    else
    {
        pauseGame();

        // 居中显示游戏菜单
        QRect rect = this->geometry();
        game_menu_ui->move(rect.width() / 2 - game_menu_ui->width() / 2,
                           rect.height() / 2 - game_menu_ui->height() / 2);

        game_menu_ui->show();
    }
}

void MainScene::onSaveButtonClicked()
{
    if (save_ui)
    {
        pauseGame();
        save_ui->show();
    }
}

void MainScene::onSaveClosed()
{
    resumeGame();
}

void MainScene::pauseGame()
{
    if (game_state && game_state->isRunning())
    {
        game_state->pause();
        is_game_paused = true;
    }
}

void MainScene::resumeGame()
{
    if (game_state && !game_state->isGameOver() && is_game_paused)
    {
        game_state->resume();
        is_game_paused = false;
    }
}

void MainScene::closeEvent(QCloseEvent *event)
{
    if (game_state && is_game_running)
    {
        // 保存游戏
        game_state->saveGame();
    }
    QMainWindow::closeEvent(event);
}

void MainScene::onShopButtonClicked()
{
    if (shop_ui)
    {
        pauseGame();
        shop_ui->show();
    }
}

void MainScene::onShopClosed()
{
    resumeGame();
}

void MainScene::onHeroLeveledUp()
{
    if (game_state && game_state->getHero())
    {
        pauseGame();
        upgrade_ui->showUpgradeOptions(game_state->getHero());
    }
}

// 设置相关槽
void MainScene::onSettingsButtonClicked()
{
    if (settings_ui)
    {
        pauseGame();
        settings_ui->show();
    }
}

void MainScene::onSettingsClosed()
{
    // 应用设置更改
    resumeGame();
}

void MainScene::onVolumeChanged(int volume)
{
    game_volume = volume;
    // 在这里更新游戏音量
}

void MainScene::onScreenSizeChanged(int index)
{
    // 根据索引更新屏幕大小
    switch (index)
    {
    case 0: // 800x600
        screen_width = 800;
        screen_height = 600;
        break;
    case 1: // 1024x768
        screen_width = 1024;
        screen_height = 768;
        break;
    case 2: // 1280x720
        screen_width = 1280;
        screen_height = 720;
        break;
    case 3: // 1920x1080
        screen_width = 1920;
        screen_height = 1080;
        break;
    }

    // 调整窗口大小
    resize(screen_width, screen_height);
}

void MainScene::onControlTypeChanged(int type)
{
    control_type = type;

    // 更新控制类型（0为WASD，1为鼠标）
    if (game_state && game_state->getHero())
    {
        game_state->getHero()->setControlType(type);
    }
}

// 游戏介绍相关槽
void MainScene::onIntroButtonClicked()
{
    if (introduction_ui)
    {
        pauseGame();
        introduction_ui->show();
    }
}

void MainScene::onIntroClosed()
{
    resumeGame();
}

// 主菜单相关槽
void MainScene::onStartGameButtonClicked()
{
    // 显示角色选择界面
    showCharacterSelection();
}

void MainScene::onLoadGameButtonClicked()
{
    loadGame();
}

void MainScene::onMainMenuSettingsClicked()
{
    if (settings_ui)
    {
        settings_ui->show();
    }
}

// 游戏菜单相关槽
void MainScene::onGameMenuResumeClicked()
{
    if (game_menu_ui)
    {
        game_menu_ui->hide();
    }
    resumeGame();
}

void MainScene::onGameMenuSettingsClicked()
{
    if (settings_ui)
    {
        settings_ui->show();
    }
}

void MainScene::onGameMenuSaveClicked()
{
    if (save_ui)
    {
        save_ui->show();
    }
}

void MainScene::onGameMenuMainMenuClicked()
{
    returnToMainMenu();
}

void MainScene::onGameMenuQuitClicked()
{
    quitGame();
}

void MainScene::loadGame()
{
    // 在这里添加读取存档的逻辑
    if (game_state)
    {
        bool loaded = game_state->loadGame();
        if (loaded)
        {
            // 隐藏主菜单
            hideMainMenu();

            // 显示游戏UI
            showGameUI();

            // 开始渲染计时器
            if (timer_id == 0)
            {
                timer_id = startTimer(16);
            }

            // 更新游戏状态
            is_game_running = true;
            is_game_paused = false;

            // 开始游戏
            game_state->resume();
        }
    }
}

void MainScene::returnToMainMenu()
{
    // 隐藏游戏结束界面
    hideGameOverScreen();

    // 显示主菜单
    showMainMenu();
}

void MainScene::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    // 更新主菜单大小
    if (main_menu)
    {
        main_menu->setGeometry(0, 0, width(), height());
    }

    // 更新角色选择UI大小
    if (character_selection)
    {
        character_selection->setGeometry(0, 0, width(), height());
    }

    // 更新游戏结束屏幕大小
    if (game_over_screen)
    {
        game_over_screen->setGeometry(0, 0, width(), height());
    }

    // 更新游戏UI元素位置
    if (shop_button)
    {
        shop_button->setGeometry(width() - 90, 10, 80, 30);
    }

    if (save_button)
    {
        save_button->setGeometry(width() - 90, 50, 80, 30);
    }

    // 居中显示各种弹出UI
    if (game_menu_ui && game_menu_ui->isVisible())
    {
        game_menu_ui->centerUI();
    }

    if (settings_ui && settings_ui->isVisible())
    {
        settings_ui->centerUI();
    }

    if (save_ui && save_ui->isVisible())
    {
        save_ui->centerUI();
    }

    if (introduction_ui && introduction_ui->isVisible())
    {
        introduction_ui->centerUI();
    }

    if (upgrade_ui && upgrade_ui->isVisible())
    {
        upgrade_ui->centerUI();
    }

    if (shop_ui && shop_ui->isVisible())
    {
        shop_ui->centerUI();
    }
}

void MainScene::loadResources()
{
    // 禁用资源加载，使用简单图形替代
    // 不加载任何资源，使用代码绘制简单形状代替
    return;
}

// 添加设置全局强化界面的方法
void MainScene::setupGlobalUpgradeMenu()
{
    // 创建全局强化菜单容器
    global_upgrade_menu = new QWidget(this);
    global_upgrade_menu->setObjectName("globalUpgradeMenu");
    global_upgrade_menu->setGeometry((width() - 600) / 2, (height() - 400) / 2, 600, 400);
    global_upgrade_menu->setStyleSheet("background-color: rgba(20, 20, 40, 220); border: 2px solid #444;");

    // 添加标题
    QLabel *titleLabel = new QLabel("全局强化", global_upgrade_menu);
    titleLabel->setGeometry(0, 20, 600, 40);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("color: white; font-size: 20px; font-weight: bold; border: none;");

    // 显示总金币
    coins_label = new QLabel(global_upgrade_menu);
    coins_label->setGeometry(230, 70, 200, 30);
    coins_label->setStyleSheet("color: gold; font-size: 16px; border: none;");
    updateCoinsLabel();

    // 添加强化选项
    setupUpgradeOptions();

    // 添加返回按钮
    QPushButton *backButton = new QPushButton("返回", global_upgrade_menu);
    backButton->setGeometry(250, 340, 100, 40);
    backButton->setStyleSheet("background-color: #555; color: white; border: 1px solid #777;");
    connect(backButton, &QPushButton::clicked, this, &MainScene::showMainMenu);

    // 默认隐藏
    global_upgrade_menu->hide();
}

// 设置强化选项
void MainScene::setupUpgradeOptions()
{
    QStringList upgradeNames = {
        "最大生命值", "基础攻击力", "金币加成", "拾取范围", "移动速度"};

    QStringList upgradeDescs = {
        "+20生命值", "+2攻击力", "+10%金币", "+10拾取范围", "+5%移动速度"};

    int optionHeight = 50;
    int spacing = 10;
    int startY = 120;

    for (int i = 0; i < 5; i++)
    {
        QWidget *container = new QWidget(global_upgrade_menu);
        container->setGeometry(50, startY + i * (optionHeight + spacing), 500, optionHeight);
        container->setStyleSheet("background-color: rgba(40, 40, 60, 150); border: 1px solid #555;");

        // 名称标签
        QLabel *nameLabel = new QLabel(upgradeNames[i], container);
        nameLabel->setGeometry(10, 5, 150, 20);
        nameLabel->setStyleSheet("color: white; font-size: 14px; border: none;");

        // 描述标签
        QLabel *descLabel = new QLabel(upgradeDescs[i], container);
        descLabel->setGeometry(10, 25, 150, 20);
        descLabel->setStyleSheet("color: #aaa; font-size: 12px; border: none;");

        // 等级标签
        upgrade_level_labels[i] = new QLabel("等级: 0", container);
        upgrade_level_labels[i]->setGeometry(170, 15, 100, 20);
        upgrade_level_labels[i]->setStyleSheet("color: #0f0; font-size: 14px; border: none;");

        // 价格标签
        upgrade_price_labels[i] = new QLabel("价格: 100", container);
        upgrade_price_labels[i]->setGeometry(280, 15, 100, 20);
        upgrade_price_labels[i]->setStyleSheet("color: gold; font-size: 14px; border: none;");

        // 升级按钮
        QPushButton *upgradeButton = new QPushButton("升级", container);
        upgradeButton->setGeometry(400, 10, 80, 30);
        upgradeButton->setStyleSheet("background-color: #464; color: white; border: 1px solid #676;");

        // 为按钮设置标识和连接
        upgradeButton->setProperty("upgrade_type", i);
        connect(upgradeButton, &QPushButton::clicked, this, &MainScene::onUpgradeButtonClicked);
    }

    // 更新升级状态显示
    updateUpgradeStatus();
}

// 更新金币显示
void MainScene::updateCoinsLabel()
{
    if (game_state && coins_label)
    {
        coins_label->setText(QString("可用金币: %1").arg(game_state->getTotalCoins()));
    }
}

// 更新升级状态显示
void MainScene::updateUpgradeStatus()
{
    if (!game_state)
        return;

    for (int i = 0; i < 5; i++)
    {
        int level = game_state->getGlobalUpgradeLevel(i);
        int nextPrice = 100 + level * 100;

        if (upgrade_level_labels[i])
        {
            upgrade_level_labels[i]->setText(QString("等级: %1").arg(level));
        }

        if (upgrade_price_labels[i])
        {
            upgrade_price_labels[i]->setText(QString("价格: %1").arg(nextPrice));
        }
    }
}

// 点击升级按钮处理
void MainScene::onUpgradeButtonClicked()
{
    if (!game_state)
        return;

    QPushButton *button = qobject_cast<QPushButton *>(sender());
    if (!button)
        return;

    int upgradeType = button->property("upgrade_type").toInt();
    int currentLevel = game_state->getGlobalUpgradeLevel(upgradeType);
    int price = 100 + currentLevel * 100;

    if (game_state->getTotalCoins() >= price)
    {
        // 扣除金币
        game_state->setTotalCoins(game_state->getTotalCoins() - price);

        // 应用升级
        game_state->applyGlobalUpgrade(upgradeType, currentLevel + 1);

        // 保存游戏
        game_state->saveGame();

        // 更新UI
        updateCoinsLabel();
        updateUpgradeStatus();
    }
    else
    {
        // 金币不足提示
        QMessageBox::information(this, "升级失败", "金币不足，无法升级!");
    }
}

// 在main menu中添加全局强化按钮
void MainScene::setupMainMenu()
{
    // 创建主菜单
    main_menu = new QWidget(this);
    main_menu->setGeometry(0, 0, width(), height());
    main_menu->setStyleSheet("background-color: rgba(0, 0, 0, 180);");

    // 主布局
    QVBoxLayout *main_layout = new QVBoxLayout(main_menu);

    // 标题
    QLabel *title = new QLabel("暗夜幸存者", main_menu);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("color: white; font-size: 48px; font-weight: bold; margin: 40px;");

    // 按钮布局
    QVBoxLayout *buttons_layout = new QVBoxLayout();
    buttons_layout->setSpacing(20);

    // 按钮样式
    QString buttonStyle = "QPushButton { background-color: rgba(40, 40, 80, 180); color: white; border: 2px solid #666; border-radius: 5px; padding: 8px; font-size: 16px; } "
                          "QPushButton:hover { background-color: rgba(60, 60, 100, 200); border: 2px solid #888; } "
                          "QPushButton:pressed { background-color: rgba(80, 80, 120, 220); }";

    // 按钮尺寸
    int buttonWidth = 250;
    int buttonHeight = 50;

    // 开始新游戏按钮
    start_game_button = new QPushButton("开始新游戏", main_menu);
    start_game_button->setMinimumSize(buttonWidth, buttonHeight);
    start_game_button->setStyleSheet(buttonStyle);
    connect(start_game_button, &QPushButton::clicked, this, &MainScene::onStartGameButtonClicked);

    // 读取存档按钮
    load_game_button = new QPushButton("读取存档", main_menu);
    load_game_button->setMinimumSize(buttonWidth, buttonHeight);
    load_game_button->setStyleSheet(buttonStyle);
    connect(load_game_button, &QPushButton::clicked, this, &MainScene::onLoadGameButtonClicked);

    // 游戏介绍按钮
    intro_button = new QPushButton("游戏介绍", main_menu);
    intro_button->setMinimumSize(buttonWidth, buttonHeight);
    intro_button->setStyleSheet(buttonStyle);
    connect(intro_button, &QPushButton::clicked, this, &MainScene::onIntroButtonClicked);

    // 全局强化按钮
    QPushButton *upgrade_button = new QPushButton("全局强化", main_menu);
    upgrade_button->setMinimumSize(buttonWidth, buttonHeight);
    upgrade_button->setStyleSheet(buttonStyle);
    connect(upgrade_button, &QPushButton::clicked, this, &MainScene::showGlobalUpgradeMenu);

    // 设置按钮
    settings_button = new QPushButton("设置", main_menu);
    settings_button->setMinimumSize(buttonWidth, buttonHeight);
    settings_button->setStyleSheet(buttonStyle);
    connect(settings_button, &QPushButton::clicked, this, &MainScene::onMainMenuSettingsClicked);

    // 退出游戏按钮
    quit_button = new QPushButton("退出游戏", main_menu);
    quit_button->setMinimumSize(buttonWidth, buttonHeight);
    quit_button->setStyleSheet(buttonStyle);
    connect(quit_button, &QPushButton::clicked, this, &MainScene::onGameMenuQuitClicked);

    // 添加按钮到布局
    buttons_layout->addWidget(start_game_button, 0, Qt::AlignCenter);
    buttons_layout->addWidget(load_game_button, 0, Qt::AlignCenter);
    buttons_layout->addWidget(intro_button, 0, Qt::AlignCenter);
    buttons_layout->addWidget(upgrade_button, 0, Qt::AlignCenter);
    buttons_layout->addWidget(settings_button, 0, Qt::AlignCenter);
    buttons_layout->addWidget(quit_button, 0, Qt::AlignCenter);

    // 添加标题和按钮布局到主布局
    main_layout->addWidget(title);
    main_layout->addLayout(buttons_layout);
    main_layout->addStretch();

    // 清空按钮列表并添加新按钮
    main_menu_buttons.clear();
    main_menu_buttons.append(start_game_button);
    main_menu_buttons.append(load_game_button);
    main_menu_buttons.append(intro_button);
    main_menu_buttons.append(upgrade_button);
    main_menu_buttons.append(settings_button);
    main_menu_buttons.append(quit_button);
}

// 显示全局强化菜单
void MainScene::showGlobalUpgradeMenu()
{
    hideAllMenus();
    if (global_upgrade_menu)
    {
        // 更新金币和强化状态
        updateCoinsLabel();
        updateUpgradeStatus();
        global_upgrade_menu->show();
    }
}

// 添加到hideAllMenus方法
void MainScene::hideAllMenus()
{
    // 隐藏主菜单
    if (main_menu)
    {
        main_menu->hide();
    }

    // 隐藏角色选择界面
    if (character_selection)
    {
        character_selection->hide();
    }

    // 隐藏设置界面
    if (settings_ui)
    {
        settings_ui->hide();
    }

    // 隐藏全局强化界面
    if (global_upgrade_menu)
    {
        global_upgrade_menu->hide();
    }

    // 隐藏存档UI
    if (save_ui)
    {
        save_ui->hide();
    }
}

// 添加显示游戏消息的函数
void MainScene::showGameMessage(const QString &message)
{
    if (!message_label || message.isEmpty())
    {
        return;
    }

    // 设置消息内容
    message_label->setText(message);
    message_label->show();

    // 3秒后自动隐藏消息
    QTimer::singleShot(3000, [this]()
                       {
        if (message_label) {
            message_label->hide();
        } });
}

// 添加渲染掉落物的方法
void MainScene::renderDrops(QPainter &painter)
{
    if (!game_state)
    {
        return;
    }

    Hero *hero = game_state->getHero();
    QVector<Drop *> drops = game_state->getDrops();

    if (!hero || drops.isEmpty())
    {
        return;
    }

    // 获取英雄位置和屏幕尺寸
    int heroX = hero->getX();
    int heroY = hero->getY();
    int screenWidth = width();
    int screenHeight = height();

    // 绘制所有掉落物
    for (Drop *drop : drops)
    {
        if (!drop || !drop->isActive())
        {
            continue;
        }

        // 获取掉落物位置
        int dropX = drop->getX();
        int dropY = drop->getY();

        // 计算掉落物在屏幕上的相对位置
        int screenX = screenWidth / 2 + (dropX - heroX);
        int screenY = screenHeight / 2 + (dropY - heroY);

        // 检查是否在屏幕范围内
        if (screenX < -20 || screenX > screenWidth + 20 ||
            screenY < -20 || screenY > screenHeight + 20)
        {
            continue;
        }

        // 绘制掉落物
        QColor dropColor;
        QString dropText;

        switch (drop->getType())
        {
        case DropType::HEALTH:
            dropColor = QColor(220, 50, 50);
            dropText = "H";
            break;
        case DropType::EXPERIENCE:
            dropColor = QColor(50, 100, 220);
            dropText = "X";
            break;
        case DropType::COIN:
            dropColor = QColor(220, 200, 50);
            dropText = "C";
            break;
        case DropType::POWER_UP:
            dropColor = QColor(100, 220, 100);
            dropText = "P";
            break;
        default:
            dropColor = QColor(200, 200, 200);
            dropText = "?";
        }

        // 绘制掉落物
        painter.setPen(Qt::NoPen);
        painter.setBrush(dropColor);
        painter.drawRect(screenX - 8, screenY - 8, 16, 16);

        // 绘制文字
        painter.setPen(Qt::white);
        painter.drawText(screenX - 5, screenY + 5, dropText);
    }
}

// 添加渲染简易投射物的方法
void MainScene::renderSimpleProjectiles(QPainter &painter)
{
    if (!game_state)
    {
        return;
    }

    Hero *hero = game_state->getHero();
    if (!hero)
    {
        return;
    }

    // 获取英雄位置和屏幕尺寸
    int heroX = hero->getX();
    int heroY = hero->getY();
    int screenWidth = width();
    int screenHeight = height();

    // 获取敌人列表，根据敌人位置生成简易子弹
    QVector<Enemy *> enemies = game_state->getEnemies();

    if (enemies.isEmpty())
    {
        return;
    }

    // 屏幕中心是英雄位置
    int centerX = screenWidth / 2;
    int centerY = screenHeight / 2;

    // 简单的逻辑：对于英雄和敌人之间，随机在路径上生成一些子弹
    for (int i = 0; i < qMin(5, enemies.size()); i++)
    {
        Enemy *enemy = enemies.at(i);

        if (!enemy || !enemy->isActive())
        {
            continue;
        }

        // 计算敌人在屏幕上的相对位置
        int enemyX = enemy->getX();
        int enemyY = enemy->getY();
        int screenEnemyX = screenWidth / 2 + (enemyX - heroX);
        int screenEnemyY = screenHeight / 2 + (enemyY - heroY);

        // 如果敌人不在屏幕范围内，跳过
        if (screenEnemyX < -30 || screenEnemyX > screenWidth + 30 ||
            screenEnemyY < -30 || screenEnemyY > screenHeight + 30)
        {
            continue;
        }

        // 计算从英雄到敌人的方向角
        double dx = screenEnemyX - centerX;
        double dy = screenEnemyY - centerY;
        double angle = std::atan2(dy, dx);

        // 生成1-3个子弹，沿着英雄到敌人的方向
        int bulletCount = 1 + (i % 3); // 简单变化，不同敌人有不同数量的子弹

        for (int j = 0; j < bulletCount; j++)
        {
            // 计算子弹位置 (在英雄到敌人的路径上随机一点)
            //            double distance = sqrt(dx * dx + dy * dy);
            double bulletRatio = 0.2 + (0.2 * j) + (static_cast<double>(rand() % 20) / 100.0);

            if (bulletRatio > 0.9)
                bulletRatio = 0.9; // 确保子弹不会太靠近敌人

            int bulletX = centerX + static_cast<int>(dx * bulletRatio);
            int bulletY = centerY + static_cast<int>(dy * bulletRatio);

            // 绘制子弹
            painter.save();
            painter.translate(bulletX, bulletY);
            painter.rotate(angle * 180 / M_PI);

            // 根据英雄攻击力绘制不同的子弹
            int attack = hero->getAttack();

            if (attack < 10)
            {
                // 小型子弹
                painter.setBrush(QColor(200, 200, 50));
                painter.setPen(Qt::black);
                painter.drawRect(-5, -1, 10, 2);
            }
            else if (attack < 20)
            {
                // 中型子弹
                painter.setBrush(QColor(220, 180, 50));
                painter.setPen(Qt::black);
                painter.drawRect(-6, -2, 12, 4);

                // 添加箭头
                QPolygon arrowHead;
                arrowHead << QPoint(4, -3) << QPoint(8, 0) << QPoint(4, 3);
                painter.setBrush(QColor(220, 180, 50));
                painter.drawPolygon(arrowHead);
            }
            else
            {
                // 大型子弹
                painter.setBrush(QColor(250, 150, 50));
                painter.setPen(Qt::black);
                painter.drawRect(-7, -3, 14, 6);

                // 添加箭头
                QPolygon arrowHead;
                arrowHead << QPoint(5, -4) << QPoint(10, 0) << QPoint(5, 4);
                painter.setBrush(QColor(250, 150, 50));
                painter.drawPolygon(arrowHead);

                // 添加尾部效果
                painter.setPen(QPen(QColor(250, 200, 100, 150), 1));
                painter.drawLine(-10, 0, -7, 0);
            }

            painter.restore();
        }
    }
}
