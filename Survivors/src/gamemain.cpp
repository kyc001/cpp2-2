//
// 游戏主窗口实现文件
//

// You may need to build the project (run Qt uic code generator) to get "ui_GameMain.h" resolved

#include "gamemain.h"
#include <QString>
#include <QPushButton>
#include <iostream>
#include "dialogs/upgradedialog.h"
#include "ui/menu.h"
#include "dialogs/gameoverdialog.h"


GameMain::GameMain(QWidget *parent) : QWidget(parent) {
    setupUi();
    setAttribute(Qt::WA_DeleteOnClose);

    initScene();
    m_Timer.setInterval(TIMER_RATE);
    hero_type = 1;
    
    // 暂停按钮点击处理
    connect(pause_button, &QPushButton::clicked, [&]()->void { pauseButtonClicked(); });
    pause_button->setText("暂停");

    // 返回按钮点击处理
    connect(back_button, &QPushButton::clicked, [&]()->void {
        if(!game_paused) { pauseButtonClicked(); }
        this->hide();
        widget_parent->show();
        widget_parent->reportGamePaused();
    });

    // 结算按钮点击处理
    connect(end_button, &QPushButton::clicked, [&]()->void {
        if(!game_paused) { pauseButtonClicked(); }
        this->hide();
        widget_parent->reportGameOver();
        auto * new_dialog = new GameOverDialog(widget_parent, game->getEnemyDeathCnt(), game->getHPPercent());
        new_dialog->show();
        this->hide();
        delete this;
    });

    playGame();
}

void GameMain::setupUi() {
    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // 游戏区域
    gameArea = new QWidget(this);
    gameArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainLayout->addWidget(gameArea);
    
    // 控制区域
    QHBoxLayout *controlsLayout = new QHBoxLayout();
    controlsLayout->setContentsMargins(10, 5, 10, 5);
    
    // 控制按钮
    pause_button = new QPushButton("暂停", this);
    back_button = new QPushButton("返回", this);
    end_button = new QPushButton("结算", this);
    
    // 信息标签
    enemy_cnt_label = new QLabel("已清除敌人数量：0", this);
    level_label = new QLabel("当前等级：1", this);
    
    // 设置字体大小
    QFont labelFont;
    labelFont.setPointSize(14);  // 增大字体大小
    labelFont.setBold(true);     // 设置为粗体
    enemy_cnt_label->setFont(labelFont);
    level_label->setFont(labelFont);
    
    // 添加组件到控制布局
    controlsLayout->addWidget(pause_button);
    controlsLayout->addWidget(back_button);
    controlsLayout->addWidget(end_button);
    controlsLayout->addStretch();
    controlsLayout->addWidget(enemy_cnt_label);
    controlsLayout->addWidget(level_label);
    
    // 添加控制布局到主布局
    mainLayout->addLayout(controlsLayout);
    
    setLayout(mainLayout);
}

GameMain::~GameMain() {
    this->hide();
    delete game;
}

// 初始化游戏场景
void GameMain::initScene() {
    std::cout << "[Log] GameMain::initScene 开始" << std::endl;
    setFixedSize(GAME_WIDTH, GAME_HEIGHT);
    setWindowTitle(GAME_TITLE);
    std::cout << "[Log] GameMain::initScene: 准备创建 GameState" << std::endl;
    game = new GameState(this);
    std::cout << "[Log] GameMain::initScene: GameState 创建完毕，准备 initMap" << std::endl;
    game->initMap(1);
    std::cout << "[Log] GameMain::initScene: initMap 调用完毕，准备 initHero" << std::endl;
    game->initHero(hero_type);
    std::cout << "[Log] GameMain::initScene: initHero 调用完毕，准备 initEnemy" << std::endl;
    game->initEnemy(1);
    std::cout << "[Log] GameMain::initScene: initEnemy 调用完毕" << std::endl;
    GAME_HALT = 0;
    FIRST_RESUME = 0;
    std::cout << "[Log] GameMain::initScene 结束" << std::endl;
}

// 开始游戏
void GameMain::playGame() {
    game_paused = false;
    m_Timer.start();

    // 设置定时器回调
    connect(&m_Timer, &QTimer::timeout, [=](){
        game->tick();
        update();
        // 更新UI显示
        enemy_cnt_label->setText("已清除敌人数量：" + QString::number(game->getEnemyDeathCnt()));
        level_label->setText("当前等级："+QString::number(game->getPlayerLevel()));
        
        // 检查游戏状态
        if(game->isGameStop()) {
            // 游戏结束处理
            m_Timer.stop();
            game_paused = true;
            pause_button->setEnabled(false);
            back_button->setEnabled(false);
            enemy_cnt_label->setText("游戏已结束，请点击结算：" + QString::number(game->getEnemyDeathCnt()));
        }
        if(game->isWaiting()) {
            // 升级处理
            UpgradeDialog * up_diag = new UpgradeDialog(this);
            pauseButtonClicked();
            pause_button->setEnabled(false);
            up_diag->show();
        }
    });
}

// 绘制事件处理
void GameMain::paintEvent(QPaintEvent * event) {
    QPainter painter(this);

    try {
        std::vector<PaintInfo> buffer = game->paint();

        for(auto & item: buffer){
            try {
                // 确保QPixmap有效
                if (!item.getTarget().isNull()) {
                    painter.drawPixmap(item.getMX(), item.getMY(), item.getTarget());
                } else {
                    std::cout << "警告：试图绘制空QPixmap，位置：" << item.getMX() << "," << item.getMY() << std::endl;
                }
            } catch (const std::exception& e) {
                std::cout << "绘制单个项目时出错: " << e.what() << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cout << "绘制过程中出错: " << e.what() << std::endl;
    } catch (...) {
        std::cout << "绘制过程中发生未知错误" << std::endl;
    }
}

// 按键按下事件处理
void GameMain::keyPressEvent(QKeyEvent *event) {
    game->keyPressTick(event);
}

// 按键释放事件处理
void GameMain::keyReleaseEvent(QKeyEvent *event) {
    game->keyReleaseTick(event);
}

// 暂停按钮点击处理
void GameMain::pauseButtonClicked() {
    if(game_paused){
        m_Timer.start();
        game_paused = false;
        pause_button->setText("暂停");
    } else {
        m_Timer.stop();
        game_paused = true;
        pause_button->setText("继续");
    }
}

// 恢复游戏
void GameMain::resumeGame() {
    if(game_paused){
        pauseButtonClicked();
        pause_button->setEnabled(true);
    }
}

// 使用指定英雄类型创建游戏
GameMain::GameMain(int hero_type, QWidget *parent) : QWidget(parent), hero_type(hero_type) {
    setupUi();
    setAttribute(Qt::WA_DeleteOnClose);

    initScene();
    m_Timer.setInterval(TIMER_RATE);

    // 暂停按钮点击处理
    connect(pause_button, &QPushButton::clicked, [&]()->void { pauseButtonClicked(); });
    pause_button->setText("暂停");

    // 返回按钮点击处理
    connect(back_button, &QPushButton::clicked, [&]()->void {
        if(!game_paused) { pauseButtonClicked(); }
        this->hide();
        widget_parent->show();
        widget_parent->reportGamePaused();
    });

    // 结算按钮点击处理
    connect(end_button, &QPushButton::clicked, [&]()->void {
        if(!game_paused) { pauseButtonClicked(); }
        this->hide();
        widget_parent->reportGameOver();
        auto * new_dialog = new GameOverDialog(widget_parent, game->getEnemyDeathCnt(), game->getHPPercent());
        new_dialog->show();
        this->hide();
        delete this;
    });

    playGame();
}

// 鼠标移动事件处理
void GameMain::mouseMoveEvent(QMouseEvent *event) {
    // 只有在游戏没有暂停且当前为鼠标控制模式时才处理鼠标移动
    if (!game_paused && CONTROL_MODE == 1) {
        game->keyPressTick(nullptr); // 清除键盘状态
        
        // 由于已将GameMain添加为GameState的友元类，可以直接访问player成员
        if (game->player) {
            game->player->mouseMoveTick(event);
        }
    }
}

