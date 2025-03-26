#ifndef MAINSCENE_H
#define MAINSCENE_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QLabel>
#include <QPushButton>

class GameState;
class Hero;
class Enemy;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainScene;
}
QT_END_NAMESPACE

class MainScene : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainScene(QWidget *parent = nullptr);
    ~MainScene() override;

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void timerEvent(QTimerEvent *event) override;

private slots:
    void onGameOver();
    void onScoreUpdated(int score);
    void onTimeUpdated(int time);
    void onStartButtonClicked();
    void onCharacterSelected(int character);

private:
    Ui::MainScene *ui;
    GameState *game_state;
    
    // UI elements
    QLabel *score_label;
    QLabel *time_label;
    QPushButton *start_button;
    
    // Character selection UI
    QWidget *character_selection;
    QPushButton *warrior_button;
    QPushButton *mage_button;
    QPushButton *archer_button;
    
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
};

#endif // MAINSCENE_H
