//
// 游戏主菜单类头文件
//

#ifndef SURVIVORS_MENU_H
#define SURVIVORS_MENU_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include "../gamemain.h"

class GameMain;

// 主菜单类
class Menu : public QWidget {
Q_OBJECT

public:
    explicit Menu(QWidget *parent = nullptr);

    ~Menu() override;

    // 游戏结束通知
    void reportGameOver();
    // 游戏暂停通知
    void reportGamePaused();
    // 游戏开始通知
    void reportGameStart(GameMain * game) { _game = game; }
    // 禁用继续游戏按钮
    void disableResume();

private:
    QLabel *background;
    QPushButton *pushButton;      // 开始游戏按钮
    QPushButton *pushButton_2;    // 设置按钮
    QPushButton *pushButton_3;    // 关于按钮
    GameMain * _game;             // 游戏主窗口指针

    // 初始化UI
    void setupUi();

private slots:
    // 启动游戏槽函数
    void launch_game();
    void openSettings();
    void openAbout();
};


#endif //VSCPROJECT_MENU_H
