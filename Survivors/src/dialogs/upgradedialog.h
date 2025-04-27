//
// 升级选择对话框头文件
//

#ifndef SURVIVORS_UPGRADEDIALOG_H
#define SURVIVORS_UPGRADEDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

class GameMain;

// 升级选择对话框类
class UpgradeDialog : public QDialog {
Q_OBJECT

public:
    // 构造函数
    // @param game_widget 游戏主窗口
    // @param parent 父窗口
    explicit UpgradeDialog(GameMain * game_widget, QWidget *parent = nullptr);

    ~UpgradeDialog() override;

private:
    GameMain * parent;  // 游戏主窗口指针
    
    // UI组件
    QLabel *titleLabel;
    QPushButton *choice1;
    QPushButton *choice2;
    QPushButton *choice3;
    
    // 初始化UI
    void setupUi();

    // 选择选项1
    void choose1();
    // 选择选项2
    void choose2();
    // 选择选项3
    void choose3();
};


#endif //VSCPROJECT_UPGRADEDIALOG_H
