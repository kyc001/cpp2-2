//
// 游戏结束对话框头文件
//

#ifndef VSCPROJECT_GAMEOVERDIALOG_H
#define VSCPROJECT_GAMEOVERDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "../../GameLogic/config.h"

class Menu;

// 游戏结束对话框类
class GameOverDialog : public QDialog {
Q_OBJECT

public:
    // 构造函数
    // @param parent 父菜单
    // @param e_cnt 击杀敌人数量
    // @param h_percent 血量百分比
    // @param s_cnt 灵魂数量
    explicit GameOverDialog(Menu * parent = nullptr, int e_cnt = 0, double h_percent = 0, int s_cnt = 0);

    ~GameOverDialog() override;

private:
    Menu * m;  // 父菜单指针
    
    // UI组件
    QLabel *gameOverLabel;
    QLabel *enemy_cnt;
    QLabel *hp_percent;
    QLabel *soul_cnt;
    QPushButton *pushButton;
    
    // 初始化UI
    void setupUi();
};


#endif //VSCPROJECT_GAMEOVERDIALOG_H
