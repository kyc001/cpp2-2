#ifndef MAINSCENE_H
#define MAINSCENE_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>

// 前置声明
class GameState;
class UpgradeUI;
class ShopUI;
class SaveUI;
class SettingsUI;
class IntroductionUI;
class GameMenuUI;
class Drop;

namespace Ui
{
    class MainScene;
}

class MainScene : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainScene(QWidget *parent = nullptr);
    ~MainScene() override;

    // 添加 getGameState 方法，确保 ShopUI 可以访问
    GameState* getGameState() const { return game_state; }

protected:
    void paintEvent(QPaintEvent *event) override;
    void timerEvent(QTimerEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void setupUI();
    void setupMainMenu();
    void setupGlobalUpgradeMenu();
    void setupUpgradeOptions();
    void showMainMenu();
    void hideMainMenu();
    void showCharacterSelection();
    void hideCharacterSelection();
    void showGameUI();
    void hideGameUI();
    void showGameOverScreen();
    void hideGameOverScreen();
    void startGame();
    void pauseGame();
    void resumeGame();
    void loadGame();
    void returnToMainMenu();
    void quitGame() { close(); }
    void hideAllMenus();
    void showGlobalUpgradeMenu();
    void loadResources();

    // 渲染方法
    void renderMap(QPainter &painter);
    void renderHero(QPainter &painter);
    void renderEnemies(QPainter &painter);
    void renderDrops(QPainter &painter);
    void renderSimpleProjectiles(QPainter &painter);

    // UI更新方法
    void updateCoinsLabel();
    void updateUpgradeStatus();
    void showGameMessage(const QString &message);
    void toggleGameMenu();

private slots:
    void onCharacterSelected(int character_id);
    void onGameOver();
    void onScoreUpdated(int score);
    void onTimeUpdated(int time);
    void onCoinsUpdated(int coins);
    void onHeroLeveledUp();
    void onStartGameButtonClicked();
    void onLoadGameButtonClicked();
    void onShopButtonClicked();
    void onShopClosed();
    void onSaveButtonClicked();
    void onSaveClosed();
    void onSettingsButtonClicked();
    void onSettingsClosed();
    void onVolumeChanged(int volume);
    void onScreenSizeChanged(int index);
    void onControlTypeChanged(int type);
    void onIntroButtonClicked();
    void onIntroClosed();
    void onMainMenuSettingsClicked();
    void onGameMenuResumeClicked();
    void onGameMenuSettingsClicked();
    void onGameMenuSaveClicked();
    void onGameMenuMainMenuClicked();
    void onGameMenuQuitClicked();
    void onUpgradeButtonClicked();
    void restartGame();

private:
    Ui::MainScene *ui;
    GameState *game_state;

    // UI组件
    QWidget *main_menu;
    QWidget *character_selection;
    QWidget *game_over_screen;
    QWidget *global_upgrade_menu;

    QList<QPushButton *> main_menu_buttons;
    QList<QPushButton *> character_buttons;
    QLabel *character_title;

    QPushButton *start_game_button;
    QPushButton *load_game_button;
    QPushButton *settings_button;
    QPushButton *intro_button;
    QPushButton *quit_button;

    QPushButton *shop_button;
    QPushButton *save_button;
    QPushButton *restart_button;
    QPushButton *return_to_menu_button;

    QLabel *score_label;
    QLabel *time_label;
    QLabel *coin_label;
    QLabel *final_score_label;
    QLabel *message_label;
    QLabel *coins_label;

    QLabel *upgrade_level_labels[5];
    QLabel *upgrade_price_labels[5];

    // 子UI组件
    UpgradeUI *upgrade_ui;
    ShopUI *shop_ui;
    SaveUI *save_ui;
    SettingsUI *settings_ui;
    IntroductionUI *introduction_ui;
    GameMenuUI *game_menu_ui;

    // 游戏状态
    int timer_id;
    bool is_game_running;
    bool is_game_paused;
    bool keys[4]; // 上右下左
    int screen_width;
    int screen_height;
    int game_volume;
    int control_type;
};

#endif // MAINSCENE_H
