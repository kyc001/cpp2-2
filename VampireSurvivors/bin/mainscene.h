#ifndef MAINSCENE_H
#define MAINSCENE_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QLabel>
#include <QPushButton>
#include <QGraphicsScene>
#include <QGraphicsView>

class GameState;
class Hero;
class Enemy;
class UpgradeUI;
class ShopUI;
class SaveUI;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainScene;
}
QT_END_NAMESPACE

class MainScene : public QMainWindow
{
    Q_OBJECT

public:
    MainScene(QWidget *parent = nullptr);
    ~MainScene();

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void timerEvent(QTimerEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onGameOver();
    void onScoreUpdated(int score);
    void onTimeUpdated(int time);
    void onCoinsUpdated(int coins);
    void onShopButtonClicked();
    void onShopClosed();
    void onHeroLeveledUp();
    void onSaveButtonClicked();
    void onSaveClosed();
    void onCharacterSelected(int character_id);

private:
    Ui::MainScene *ui;
    GameState *game_state;
    
    // UI elements
    QLabel *score_label;
    QLabel *time_label;
    QLabel *coin_label;
    QPushButton *shop_button;
    QPushButton *save_button;
    QWidget *game_over_widget;
    
    // Character selection UI
    QWidget *character_selection;
    QVector<QPushButton*> character_buttons;
    QLabel *character_title;
    
    // Game over UI
    QWidget *game_over_screen;
    QLabel *final_score_label;
    QPushButton *restart_button;
    
    // Timer ID
    int timer_id;
    
    // Rendering methods
    void renderHero(QPainter &painter);
    void renderEnemies(QPainter &painter);
    void renderMap(QPainter &painter);
    
    // UI setup methods
    void setupUI();
    void showCharacterSelection();
    void hideCharacterSelection();
    void showGameOverScreen();
    void hideGameOverScreen();
    
    // Game control methods
    void startGame();
    void restartGame();
    void pauseGame();
    void resumeGame();
    
    // Game components
    UpgradeUI *upgrade_ui;
    ShopUI *shop_ui;
    SaveUI *save_ui;
    
    // Input state
    bool keys[4]; // Up, Right, Down, Left
};

#endif // MAINSCENE_H
