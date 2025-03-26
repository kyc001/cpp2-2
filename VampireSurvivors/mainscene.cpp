#include "bin/mainscene.h"
#include "ui_mainscene.h"
#include "bin/gamestate.h"
#include "bin/hero.h"
#include "bin/enemy.h"
#include "bin/gamemap.h"
#include <QPainter>
#include <QTimer>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QFont>

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
    score_label->setGeometry(10, 10, 150, 30);
    score_label->setStyleSheet("color: white; font-size: 16px;");
    
    time_label = new QLabel("Time: 0", this);
    time_label->setGeometry(width() - 160, 10, 150, 30);
    time_label->setStyleSheet("color: white; font-size: 16px;");
    
    // Create character selection UI
    character_selection = new QWidget(this);
    character_selection->setGeometry(0, 0, width(), height());
    character_selection->setStyleSheet("background-color: rgba(0, 0, 0, 180);");
    
    QVBoxLayout *char_layout = new QVBoxLayout(character_selection);
    
    QLabel *title = new QLabel("Select Your Character", character_selection);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("color: white; font-size: 24px; font-weight: bold;");
    
    QHBoxLayout *buttons_layout = new QHBoxLayout();
    
    warrior_button = new QPushButton("Warrior", character_selection);
    warrior_button->setMinimumSize(120, 120);
    connect(warrior_button, &QPushButton::clicked, this, [this]() { onCharacterSelected(0); });
    
    mage_button = new QPushButton("Mage", character_selection);
    mage_button->setMinimumSize(120, 120);
    connect(mage_button, &QPushButton::clicked, this, [this]() { onCharacterSelected(1); });
    
    archer_button = new QPushButton("Archer", character_selection);
    archer_button->setMinimumSize(120, 120);
    connect(archer_button, &QPushButton::clicked, this, [this]() { onCharacterSelected(2); });
    
    buttons_layout->addWidget(warrior_button);
    buttons_layout->addWidget(mage_button);
    buttons_layout->addWidget(archer_button);
    
    char_layout->addWidget(title);
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
}

void MainScene::hideCharacterSelection()
{
    character_selection->hide();
    score_label->show();
    time_label->show();
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
    // Initialize game
    game_state->init();
    game_state->start();
    
    // Start game update timer
    if (timer_id == 0) {
        timer_id = startTimer(16); // ~60 fps
    }
    
    // Reset UI
    score_label->setText("Score: 0");
    time_label->setText("Time: 0");
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
