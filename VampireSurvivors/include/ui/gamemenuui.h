#ifndef GAMEMENUUI_H
#define GAMEMENUUI_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>

// 游戏菜单UI (按ESC键显示)
class GameMenuUI : public QWidget
{
    Q_OBJECT

public:
    explicit GameMenuUI(QWidget *parent = nullptr);
    ~GameMenuUI();

    void show();
    void hide();
    bool isVisible() const;
    void centerUI();

signals:
    // 各种菜单操作的信号
    void resumeGameSignal();
    void settingsOpenSignal();
    void saveGameSignal();
    void mainMenuSignal();
    void quitGameSignal();

private slots:
    // 按钮槽函数
    void onResumeButtonClicked();
    void onSettingsButtonClicked();
    void onSaveButtonClicked();
    void onMainMenuButtonClicked();
    void onQuitButtonClicked();

private:
    void setupUI();

    // UI元素
    QPushButton *resume_button;
    QPushButton *settings_button;
    QPushButton *save_button;
    QPushButton *main_menu_button;
    QPushButton *quit_button;
};

#endif // GAMEMENUUI_H 