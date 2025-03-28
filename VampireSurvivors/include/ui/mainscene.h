#ifndef MAINSCENE_H
#define MAINSCENE_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QLabel>
#include <QPushButton>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QResizeEvent>
#include <QVector>
#include <QPair>

namespace Ui {
class MainScene;
}

class GameState;
class Hero;
class Enemy;
class UpgradeUI;
class ShopUI;
class SaveUI;
class SettingsUI;
class IntroductionUI;
class GameMenuUI;

class MainScene : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainScene(QWidget *parent = nullptr);
    ~MainScene() override;
    
    GameState* getGameState() const { return game_state; }

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void timerEvent(QTimerEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    // 游戏状态相关槽
    void onGameOver();
    void onScoreUpdated(int score);
    void onTimeUpdated(int time);
    void onCoinsUpdated(int coins);
    void onHeroLeveledUp();
    void onCharacterSelected(int character_id);
    
    // UI相关槽
    void onShopButtonClicked();
    void onShopClosed();
    void onSaveButtonClicked();
    void onSaveClosed();
    void onSettingsButtonClicked();
    void onSettingsClosed();
    void onIntroButtonClicked();
    void onIntroClosed();
    void onVolumeChanged(int volume);
    void onScreenSizeChanged(int index);
    void onControlTypeChanged(int type);
    
    // 游戏菜单槽
    void onGameMenuResumeClicked();
    void onGameMenuSettingsClicked();
    void onGameMenuSaveClicked();
    void onGameMenuMainMenuClicked();
    void onGameMenuQuitClicked();
    
    // 主菜单槽
    void onStartGameButtonClicked();
    void onLoadGameButtonClicked();
    void onMainMenuSettingsClicked();

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
    
    // Main menu UI
    QWidget *main_menu;
    QPushButton *start_game_button;
    QPushButton *load_game_button;
    QPushButton *intro_button;
    QPushButton *settings_button;
    QPushButton *quit_button;
    
    // Timer ID
    int timer_id;
    
    // Rendering methods
    void renderHero(QPainter &painter);
    void renderEnemies(QPainter &painter);
    void renderMap(QPainter &painter);
    
    // UI setup methods
    void setupUI();
    void setupMainMenu();
    void showMainMenu();
    void hideMainMenu();
    void showCharacterSelection();
    void hideCharacterSelection();
    void showGameOverScreen();
    void hideGameOverScreen();
    void showGameUI();
    void hideGameUI();
    
    // Game control methods
    void startGame();
    void restartGame();
    void pauseGame();
    void resumeGame();
    void toggleGameMenu();
    void quitGame();
    void loadGame();
    void returnToMainMenu();
    
    // Game components
    UpgradeUI *upgrade_ui;
    ShopUI *shop_ui;
    SaveUI *save_ui;
    SettingsUI *settings_ui;
    IntroductionUI *introduction_ui;
    GameMenuUI *game_menu_ui;
    
    // Input state
    bool keys[4]; // Up, Right, Down, Left
    
    // Game state
    bool is_game_running;
    bool is_game_paused;
    int screen_width;
    int screen_height;
    int game_volume;
    int control_type; // 0 for WASD, 1 for Mouse
    
    // Resource loading
    void loadResources();
};

#endif // MAINSCENE_H
