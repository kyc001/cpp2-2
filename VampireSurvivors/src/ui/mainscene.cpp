#include "bin/mainscene.h"
#include "ui_mainscene.h"
#include "bin/gamestate.h"
#include "bin/hero.h"
#include "bin/enemy.h"
#include "bin/gamemap.h"
#include "bin/upgradeui.h"
#include "bin/shopui.h"
#include "bin/saveui.h"
#include <QPainter>
#include <QTimer>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QFont>
#include <QKeyEvent>
#include <QCloseEvent>
#include <QVector>

MainScene::MainScene(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainScene), game_state(nullptr), timer_id(0)
{
    ui->setupUi(this);
    
    // Set window title and size
    setWindowTitle("Vampire Survivors Clone");
    resize(800, 600);
    
    // Create game state
    game_state = new GameState(this);
    
    // Connect signals
    connect(game_state, &GameState::gameOverSignal, this, &MainScene::onGameOver);
    connect(game_state, &GameState::scoreUpdated, this, &MainScene::onScoreUpdated);
    connect(game_state, &GameState::timeUpdated, this, &MainScene::onTimeUpdated);
    connect(game_state, &GameState::coinsUpdated, this, &MainScene::onCoinsUpdated);
    
    // Setup UI
    setupUI();
    
    // Show character selection first
    showCharacterSelection();
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
    score_label = new QLabel("Score: 0", this);
    score_label->setGeometry(10, 10, 200, 30);
    score_label->setStyleSheet("color: white; font-size: 16px;");
    score_label->hide();
    
    time_label = new QLabel("Time: 0", this);
    time_label->setGeometry(10, 40, 200, 30);
    time_label->setStyleSheet("color: white; font-size: 16px;");
    time_label->hide();
    
    coin_label = new QLabel("Coins: 0", this);
    coin_label->setGeometry(10, 70, 200, 30);
    coin_label->setStyleSheet("color: gold; font-size: 16px;");
    coin_label->hide();
    
    // Create character selection UI
    character_selection = new QWidget(this);
    character_selection->setGeometry(0, 0, width(), height());
    character_selection->setStyleSheet("background-color: rgba(0, 0, 0, 180);");
    
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
    
    QLabel *go_title = new QLabel("Game Over", game_over_screen);
    go_title->setAlignment(Qt::AlignCenter);
    go_title->setStyleSheet("color: white; font-size: 32px; font-weight: bold;");
    
    final_score_label = new QLabel("Final Score: 0", game_over_screen);
    final_score_label->setAlignment(Qt::AlignCenter);
    final_score_label->setStyleSheet("color: white; font-size: 24px;");
    
    restart_button = new QPushButton("Restart", game_over_screen);
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
    
    // 创建升级UI
    upgrade_ui = new UpgradeUI(this);
    connect(game_state, &GameState::levelUpOptionsReady, this, &MainScene::onHeroLeveledUp);
    
    // 创建商店UI
    shop_ui = new ShopUI(this);
    connect(shop_ui, &ShopUI::shopClosed, this, &MainScene::onShopClosed);
    
    // 创建存档UI
    save_ui = new SaveUI(this);
    connect(save_ui, &SaveUI::saveUIClosed, this, &MainScene::onSaveClosed);
}

void MainScene::showCharacterSelection()
{
    character_selection->show();
    if (timer_id > 0) {
        killTimer(timer_id);
        timer_id = 0;
    }
    
    score_label->hide();
    time_label->hide();
    coin_label->hide();
}

void MainScene::hideCharacterSelection()
{
    character_selection->hide();
    score_label->show();
    time_label->show();
    coin_label->show();
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
    // Hide character selection UI
    for (QPushButton* btn : character_buttons) {
        btn->hide();
    }
    character_title->hide();
    character_selection->hide();
    
    // Initialize game
    game_state->init();
    
    // Start game
    game_state->start();
    
    // Show score and time labels
    score_label->show();
    time_label->show();
    
    // Show shop and save buttons
    shop_button->show();
    save_button->show();
    
    // Start rendering timer
    if (timer_id == 0) {
        timer_id = startTimer(16); // ~60 fps
    }
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
    final_score_label->setText(QString("Final Score: %1").arg(final_score));
    showGameOverScreen();
}

void MainScene::onScoreUpdated(int score)
{
    score_label->setText(QString("Score: %1").arg(score));
}

void MainScene::onTimeUpdated(int time)
{
    time_label->setText(QString("Time: %1").arg(time));
}

void MainScene::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    
    QPainter painter(this);
    
    // Clear the background
    painter.fillRect(rect(), QColor(20, 20, 30));
    
    // Only render if game is running
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
    if (!game_state || !game_state->getHero()) {
        QMainWindow::keyPressEvent(event);
        return;
    }
    
    // Pass key events to hero for movement
    Hero* hero = game_state->getHero();
    hero->processKeyPress(event->key());
    
    // Handle other keys
    switch (event->key()) {
        case Qt::Key_Escape:
            if (game_state->isRunning()) {
                game_state->pause();
            } else if (!game_state->isGameOver()) {
                game_state->resume();
            }
            break;
    }
    
    QMainWindow::keyPressEvent(event);
}

void MainScene::keyReleaseEvent(QKeyEvent *event)
{
    if (game_state && game_state->getHero()) {
        Hero* hero = game_state->getHero();
        hero->processKeyRelease(event->key());
    }
    
    QMainWindow::keyReleaseEvent(event);
}

void MainScene::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == timer_id) {
        // Update the game state
        if (game_state && game_state->isRunning()) {
            game_state->update();
        }
        
        // Repaint the scene
        update();
    }
    
    QMainWindow::timerEvent(event);
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
    }
}

void MainScene::resumeGame() {
    if (game_state && !game_state->isGameOver()) {
        game_state->resume();
    }
}

void MainScene::closeEvent(QCloseEvent* event) {
    if (game_state) {
        // 保存游戏
        game_state->saveGame();
    }
    QMainWindow::closeEvent(event);
}

void MainScene::onCoinsUpdated(int coins) {
    coin_label->setText(QString("Coins: %1").arg(coins));
    coin_label->show();
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
