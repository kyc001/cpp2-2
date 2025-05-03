//
// 游戏主窗口类头文件
//

#ifndef VSCPROJECT_GAMEMAIN_H
#define VSCPROJECT_GAMEMAIN_H

#include <QWidget>
#include <QTimer>
#include <QPainter>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QMovie>
#include <QShortcut>
#include <QMouseEvent>
#include "GameLogic/GameState.h"
#include "GameLogic/config.h"

class Menu;

// 游戏主窗口类
class GameMain : public QWidget {
Q_OBJECT

public:
    explicit GameMain(QWidget *parent = nullptr);
    explicit GameMain(int hero_type, QWidget * parent = nullptr);

    ~GameMain() override;

    // 设置父窗口
    void setWidgetParent(Menu * in) { widget_parent = in;};
    // 游戏状态对象
    GameState * game;

    // 恢复游戏
    void resumeGame();
    
    // 显示猫蝶GIF动画
    void showMaodieAnimation(int x, int y, int size = 100);

private slots:
    // 快捷键触发显示猫蝶动画
    void onShowMaodieTriggered();
    
protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    // UI组件
    QWidget *gameArea;
    QPushButton *pause_button;
    QPushButton *back_button;
    QPushButton *end_button;
    QLabel *enemy_cnt_label;
    QLabel *level_label;
    QMediaPlayer *bgmPlayer;
    QAudioOutput *audioOutput;
    QMovie *maodieMovie;      // 猫蝶GIF动画
    QLabel *maodieLabel;      // 显示猫蝶动画的标签
    QShortcut *maodieShortcut; // 显示猫蝶的快捷键
    
    Menu * widget_parent;  // 父窗口指针
    QTimer m_Timer;        // 游戏计时器
    int hero_type;         // 英雄类型
    bool game_paused;      // 游戏暂停标志

    // 升级面板相关
    QWidget *upgradePanel;
    QVBoxLayout *upgradePanelLayout;
    QList<QPushButton*> upgradeButtons;

    // 结算面板相关
    QWidget *gameOverPanel;
    QVBoxLayout *gameOverLayout;
    QLabel *gameOverTitleLabel;
    QLabel *gameOverStatsLabel;
    QPushButton *gameOverCloseButton;

    // 初始化UI
    void setupUi();
    // 初始化游戏场景
    void initScene();
    // 开始游戏
    void playGame();
    // 初始化并播放背景音乐
    void initAndPlayBGM();
    // 尝试使用 Windows API 播放背景音乐
    void playBackgroundMusicWithWinAPI();
    // 初始化猫蝶动画
    void initMaodieAnimation();

    // 暂停按钮点击处理
    void pauseButtonClicked();

    // 初始化升级面板
    void initUpgradePanel();
    // 显示升级面板
    void showUpgradePanel();
    // 隐藏升级面板
    void hideUpgradePanel();
    // 处理升级选择
    void handleUpgradeChoice(int choice);

    // 初始化结算面板
    void initGameOverPanel();
    // 显示结算面板
    void showGameOverPanel(int enemyCount, double hpPercent);
    // 隐藏结算面板
    void hideGameOverPanel();
    // 处理结算面板关闭
    void handleGameOverClose();
};


#endif //VSCPROJECT_GAMEMAIN_H
