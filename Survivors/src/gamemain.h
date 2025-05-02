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

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

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
    
    Menu * widget_parent;  // 父窗口指针
    QTimer m_Timer;        // 游戏计时器
    int hero_type;         // 英雄类型
    bool game_paused;      // 游戏暂停标志

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

    // 暂停按钮点击处理
    void pauseButtonClicked();
};


#endif //VSCPROJECT_GAMEMAIN_H
