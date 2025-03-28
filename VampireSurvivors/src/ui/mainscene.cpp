#include "../../include/ui/mainscene.h"
#include "../../ui/ui_mainscene.h"
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

MainScene::MainScene(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainScene), game_state(nullptr), timer_id(0),
    is_game_running(false), is_game_paused(false), screen_width(800), screen_height(600),
    game_volume(100), control_type(0)
{
    ui->setupUi(this);
    
    // Set window title and size
    setWindowTitle("吸血鬼生存者");
    resize(screen_width, screen_height);
    
    // Create game state
    game_state = new GameState(this);
    
    // Connect signals
    connect(game_state, &GameState::gameOverSignal, this, &MainScene::onGameOver);
    connect(game_state, &GameState::scoreUpdated, this, &MainScene::onScoreUpdated);
    connect(game_state, &GameState::timeUpdated, this, &MainScene::onTimeUpdated);
    connect(game_state, &GameState::coinsUpdated, this, &MainScene::onCoinsUpdated);
    
    // Setup UI
    setupUI();
    setupMainMenu();
    
    // 初始显示主菜单，而不是角色选择
    showMainMenu();
}

MainScene::~MainScene()
{
    if (timer_id > 0) {
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
    
    // 创建角色按钮
    QStringList character_names = {"战士", "法师", "弓箭手", "盗贼"};
    
    for (int i = 0; i < character_names.size(); i++) {
        QPushButton* btn = new QPushButton(character_names[i], character_selection);
        btn->setMinimumSize(120, 120);
        connect(btn, &QPushButton::clicked, this, [this, i]() { onCharacterSelected(i); });
        buttons_layout->addWidget(btn);
        character_buttons.append(btn);
    }
    
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
    
    go_layout->addWidget(go_title);
    go_layout->addWidget(final_score_label);
    go_layout->addWidget(restart_button, 0, Qt::AlignCenter);
    
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
    connect(game_state, &GameState::levelUpOptionsReady, this, &MainScene::onHeroLeveledUp);
    
    shop_ui = new ShopUI(this);
    connect(shop_ui, &ShopUI::shopClosed, this, &MainScene::onShopClosed);
    
    save_ui = new SaveUI(this);
    connect(save_ui, &SaveUI::saveUIClosed, this, &MainScene::onSaveClosed);
    
    // 创建设置UI
    settings_ui = new SettingsUI(this);
    connect(settings_ui, &SettingsUI::settingsUIClosed, this, &MainScene::onSettingsClosed);
    connect(settings_ui, &SettingsUI::volumeChanged, this, &MainScene::onVolumeChanged);
    connect(settings_ui, &SettingsUI::screenSizeChanged, this, &MainScene::onScreenSizeChanged);
    connect(settings_ui, &SettingsUI::controlTypeChanged, this, &MainScene::onControlTypeChanged);
    
    // 创建游戏介绍UI
    introduction_ui = new IntroductionUI(this);
    connect(introduction_ui, &IntroductionUI::introductionUIClosed, this, &MainScene::onIntroClosed);
    
    // 创建游戏菜单UI (ESC时显示)
    game_menu_ui = new GameMenuUI(this);
    connect(game_menu_ui, &GameMenuUI::resumeGameSignal, this, &MainScene::onGameMenuResumeClicked);
    connect(game_menu_ui, &GameMenuUI::settingsOpenSignal, this, &MainScene::onGameMenuSettingsClicked);
    connect(game_menu_ui, &GameMenuUI::saveGameSignal, this, &MainScene::onGameMenuSaveClicked);
    connect(game_menu_ui, &GameMenuUI::mainMenuSignal, this, &MainScene::onGameMenuMainMenuClicked);
    connect(game_menu_ui, &GameMenuUI::quitGameSignal, this, &MainScene::onGameMenuQuitClicked);
}

void MainScene::setupMainMenu()
{
    // 创建主菜单
    main_menu = new QWidget(this);
    main_menu->setGeometry(0, 0, width(), height());
    main_menu->setStyleSheet("background-color: rgba(0, 0, 0, 180);");
    
    QVBoxLayout *main_layout = new QVBoxLayout(main_menu);
    
    // 标题
    QLabel *title = new QLabel("吸血鬼生存者", main_menu);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("color: white; font-size: 48px; font-weight: bold; margin: 40px;");
    
    // 按钮布局
    QVBoxLayout *buttons_layout = new QVBoxLayout();
    buttons_layout->setSpacing(20);
    
    // 主菜单按钮
    start_game_button = new QPushButton("开始游戏", main_menu);
    start_game_button->setMinimumSize(250, 50);
    start_game_button->setStyleSheet("background-color: #4a90e2; color: white; border: none; border-radius: 5px; font-size: 18px;");
    connect(start_game_button, &QPushButton::clicked, this, &MainScene::onStartGameButtonClicked);
    
    load_game_button = new QPushButton("读取存档", main_menu);
    load_game_button->setMinimumSize(250, 50);
    load_game_button->setStyleSheet("background-color: #4a90e2; color: white; border: none; border-radius: 5px; font-size: 18px;");
    connect(load_game_button, &QPushButton::clicked, this, &MainScene::onLoadGameButtonClicked);
    
    intro_button = new QPushButton("游戏介绍", main_menu);
    intro_button->setMinimumSize(250, 50);
    intro_button->setStyleSheet("background-color: #4a90e2; color: white; border: none; border-radius: 5px; font-size: 18px;");
    connect(intro_button, &QPushButton::clicked, this, &MainScene::onIntroButtonClicked);
    
    settings_button = new QPushButton("设置", main_menu);
    settings_button->setMinimumSize(250, 50);
    settings_button->setStyleSheet("background-color: #4a90e2; color: white; border: none; border-radius: 5px; font-size: 18px;");
    connect(settings_button, &QPushButton::clicked, this, &MainScene::onMainMenuSettingsClicked);
    
    quit_button = new QPushButton("退出游戏", main_menu);
    quit_button->setMinimumSize(250, 50);
    quit_button->setStyleSheet("background-color: #e24a4a; color: white; border: none; border-radius: 5px; font-size: 18px;");
    connect(quit_button, &QPushButton::clicked, this, &MainScene::onGameMenuQuitClicked);
    
    // 添加按钮到布局
    buttons_layout->addWidget(start_game_button, 0, Qt::AlignCenter);
    buttons_layout->addWidget(load_game_button, 0, Qt::AlignCenter);
    buttons_layout->addWidget(intro_button, 0, Qt::AlignCenter);
    buttons_layout->addWidget(settings_button, 0, Qt::AlignCenter);
    buttons_layout->addWidget(quit_button, 0, Qt::AlignCenter);
    
    // 添加按钮布局到主布局
    main_layout->addWidget(title);
    main_layout->addLayout(buttons_layout);
    main_layout->addStretch();
}

void MainScene::showMainMenu()
{
    // 隐藏游戏UI
    hideGameUI();
    
    // 显示主菜单
    main_menu->show();
    
    // 停止游戏计时器
    if (timer_id > 0) {
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
    
    if (timer_id > 0) {
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

void MainScene::onCharacterSelected(int character)
{
    game_state->selectCharacter(character);
    hideCharacterSelection();
    startGame();
}

void MainScene::startGame()
{
    // 隐藏角色选择UI
    hideCharacterSelection();
    
    // 初始化游戏
    game_state->init();
    
    // 开始游戏
    game_state->start();
    
    // 显示游戏UI
    showGameUI();
    
    // 开始渲染计时器
    if (timer_id == 0) {
        timer_id = startTimer(16); // ~60 fps
    }
    
    // 更新游戏状态
    is_game_running = true;
    is_game_paused = false;
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
    if (game_state) {
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
    time_label->setText(QString("时间: %1").arg(time));
}

void MainScene::onCoinsUpdated(int coins)
{
    coin_label->setText(QString("金币: %1").arg(coins));
}

void MainScene::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    
    QPainter painter(this);
    
    // 清除背景
    painter.fillRect(rect(), QColor(20, 20, 30));
    
    // 只有在游戏运行时渲染
    if (game_state && game_state->isRunning()) {
        renderMap(painter);
        renderEnemies(painter);
        renderHero(painter);
    }
}

void MainScene::renderMap(QPainter &painter)
{
    // Draw map background
    painter.fillRect(rect(), QColor(50, 50, 70));
    
    // Draw map barriers
    if (game_state && game_state->getMap()) {
        GameMap* map = game_state->getMap();
        QVector<QVector<int>> barriers = map->getBarries();
        
        // 使用深色绘制障碍物
        painter.setBrush(QColor(30, 30, 50));
        painter.setPen(Qt::NoPen);
        
        for (int y = 0; y < barriers.size(); y++) {
            for (int x = 0; x < barriers[y].size(); x++) {
                if (barriers[y][x] == 1) {
                    // 绘制障碍物
                    int screenX = x * 20;
                    int screenY = y * 20;
                    painter.drawRect(screenX, screenY, 20, 20);
                }
            }
        }
    }
}

void MainScene::renderHero(QPainter &painter)
{
    Hero* hero = game_state->getHero();
    if (!hero) return;
    
    // Draw hero
    int x = hero->getX() * 20; // Scale to screen coordinates
    int y = hero->getY() * 20;
    
    painter.setBrush(QColor(0, 200, 0));
    painter.drawRect(x - 10, y - 10, 20, 20);
}

void MainScene::renderEnemies(QPainter &painter)
{
    QVector<Enemy*> enemies = game_state->getEnemies();
    
    for (Enemy* enemy : enemies) {
        if (!enemy || !enemy->isActive()) continue;
        
        int x = enemy->getX() * 20; // Scale to screen coordinates
        int y = enemy->getY() * 20;
        
        // Draw different colors for different enemy types
        switch(enemy->getType()) {
            case 0: // Basic melee
                painter.setBrush(QColor(200, 0, 0));
                break;
            case 1: // Fast melee
                painter.setBrush(QColor(200, 100, 0));
                break;
            case 2: // Ranged
                painter.setBrush(QColor(200, 0, 200));
                break;
            case 3: // Tank
                painter.setBrush(QColor(150, 0, 0));
                break;
            default:
                painter.setBrush(QColor(200, 0, 0));
        }
        
        painter.drawRect(x - 10, y - 10, 20, 20);
    }
}

void MainScene::keyPressEvent(QKeyEvent *event)
{
    // 处理游戏内按键
    if (game_state && game_state->isRunning() && game_state->getHero()) {
        // 传递移动按键到英雄
        Hero* hero = game_state->getHero();
        hero->processKeyPress(event->key());
        
        // 处理ESC键 - 显示游戏菜单
        if (event->key() == Qt::Key_Escape) {
            toggleGameMenu();
        }
    }
    
    QMainWindow::keyPressEvent(event);
}

void MainScene::keyReleaseEvent(QKeyEvent *event)
{
    if (game_state && game_state->isRunning() && game_state->getHero()) {
        Hero* hero = game_state->getHero();
        hero->processKeyRelease(event->key());
    }
    
    QMainWindow::keyReleaseEvent(event);
}

void MainScene::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == timer_id) {
        // 更新游戏状态
        if (game_state && game_state->isRunning()) {
            game_state->update();
        }
        
        // 重绘场景
        update();
    }
    
    QMainWindow::timerEvent(event);
}

void MainScene::toggleGameMenu()
{
    if (game_menu_ui->isVisible()) {
        game_menu_ui->hide();
        resumeGame();
    } else {
        pauseGame();
        
        // 居中显示游戏菜单
        QRect rect = this->geometry();
        game_menu_ui->move(rect.width()/2 - game_menu_ui->width()/2, 
                           rect.height()/2 - game_menu_ui->height()/2);
        
        game_menu_ui->show();
    }
}

void MainScene::onSaveButtonClicked() {
    if (game_state) {
        pauseGame();
        save_ui->showSaveUI(game_state);
    }
}

void MainScene::onSaveClosed() {
    resumeGame();
}

void MainScene::pauseGame() {
    if (game_state && game_state->isRunning()) {
        game_state->pause();
        is_game_paused = true;
    }
}

void MainScene::resumeGame() {
    if (game_state && !game_state->isGameOver() && is_game_paused) {
        game_state->resume();
        is_game_paused = false;
    }
}

void MainScene::closeEvent(QCloseEvent* event) {
    if (game_state && is_game_running) {
        // 保存游戏
        game_state->saveGame();
    }
    QMainWindow::closeEvent(event);
}

void MainScene::onShopButtonClicked() {
    if (game_state) {
        pauseGame();
        shop_ui->showShop();
    }
}

void MainScene::onShopClosed() {
    resumeGame();
}

void MainScene::onHeroLeveledUp() {
    if (game_state && game_state->getHero()) {
        pauseGame();
        upgrade_ui->showUpgradeOptions(game_state->getHero());
    }
}

// 设置相关槽
void MainScene::onSettingsButtonClicked() {
    pauseGame();
    
    // 居中显示设置界面
    QRect rect = this->geometry();
    settings_ui->move(rect.width()/2 - settings_ui->width()/2, 
                      rect.height()/2 - settings_ui->height()/2);
    
    settings_ui->show();
}

void MainScene::onSettingsClosed() {
    if (is_game_running) {
        resumeGame();
    }
}

void MainScene::onVolumeChanged(int volume) {
    game_volume = volume;
    // 这里可以添加实际控制音量的代码
}

void MainScene::onScreenSizeChanged(int index) {
    // 根据索引设置屏幕大小
    QSize size;
    switch(index) {
        case 0: size = QSize(800, 600); break;
        case 1: size = QSize(1024, 768); break;
        case 2: size = QSize(1280, 720); break;
        case 3: size = QSize(1600, 900); break;
        case 4: size = QSize(1920, 1080); break;
        default: size = QSize(800, 600);
    }
    
    screen_width = size.width();
    screen_height = size.height();
    
    // 重新调整窗口大小
    resize(screen_width, screen_height);
    
    // 更新所有UI组件的位置
    // 这里需要更新所有需要居中的UI元素
    if (game_menu_ui->isVisible()) {
        game_menu_ui->move(width()/2 - game_menu_ui->width()/2, 
                          height()/2 - game_menu_ui->height()/2);
    }
}

void MainScene::onControlTypeChanged(int type) {
    control_type = type;
    // 更新控制类型到游戏状态
    if (game_state && game_state->getHero()) {
        game_state->getHero()->setControlType(type);
    }
}

// 游戏介绍相关槽
void MainScene::onIntroButtonClicked() {
    // 居中显示游戏介绍界面
    QRect rect = this->geometry();
    introduction_ui->move(rect.width()/2 - introduction_ui->width()/2, 
                         rect.height()/2 - introduction_ui->height()/2);
    
    introduction_ui->show();
}

void MainScene::onIntroClosed() {
    // 游戏介绍关闭时不需要特别处理
}

// 主菜单相关槽
void MainScene::onStartGameButtonClicked() {
    hideMainMenu();
    showCharacterSelection();
}

void MainScene::onLoadGameButtonClicked() {
    loadGame();
}

void MainScene::onMainMenuSettingsClicked() {
    // 居中显示设置界面
    QRect rect = this->geometry();
    settings_ui->move(rect.width()/2 - settings_ui->width()/2, 
                      rect.height()/2 - settings_ui->height()/2);
    
    settings_ui->show();
}

// 游戏菜单相关槽
void MainScene::onGameMenuResumeClicked() {
    game_menu_ui->hide();
    resumeGame();
}

void MainScene::onGameMenuSettingsClicked() {
    // 居中显示设置界面
    QRect rect = this->geometry();
    settings_ui->move(rect.width()/2 - settings_ui->width()/2, 
                      rect.height()/2 - settings_ui->height()/2);
    
    settings_ui->show();
}

void MainScene::onGameMenuSaveClicked() {
    save_ui->showSaveUI(game_state);
}

void MainScene::onGameMenuMainMenuClicked() {
    // 确保游戏暂停
    pauseGame();
    
    // 返回主菜单
    returnToMainMenu();
}

void MainScene::onGameMenuQuitClicked() {
    // 退出前保存游戏
    if (game_state && is_game_running) {
        game_state->saveGame();
    }
    
    // 关闭应用
    QApplication::quit();
}

void MainScene::loadGame() {
    // 在这里添加读取存档的逻辑
    if (game_state) {
        bool loaded = game_state->loadGame();
        if (loaded) {
            // 隐藏主菜单
            hideMainMenu();
            
            // 显示游戏UI
            showGameUI();
            
            // 开始渲染计时器
            if (timer_id == 0) {
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

void MainScene::returnToMainMenu() {
    // 停止游戏
    if (game_state && is_game_running) {
        // 保存游戏状态
        game_state->saveGame();
        
        // 重置游戏状态
        game_state->reset();
    }
    
    // 隐藏游戏UI和菜单
    hideGameUI();
    game_menu_ui->hide();
    
    // 显示主菜单
    showMainMenu();
}
