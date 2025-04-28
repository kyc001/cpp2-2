//
// 游戏主菜单实现文件
//

// You may need to build the project (run Qt uic code generator) to get "ui_Menu.h" resolved

#include "menu.h"
#include "../gamemain.h"
#include "../dialogs/settingdialog.h"
#include "../dialogs/aboutdialog.h"
#include <QLabel>
#include <QPixmap>
#include <QMessageBox>
#include <QApplication>
#include <QHBoxLayout>

Menu::Menu(QWidget *parent) : QWidget(parent) {
    setupUi();
    
    // 设置背景图片
    background->setPixmap(QPixmap(":/Assets/bg.jpg").scaled(1000, 800));

    // 设置按钮文本
    pushButton->setText("开始游戏");
    pushButton_2->setText("设置");
    pushButton_3->setText("关于");
    
    // 连接按钮信号
    connect(pushButton, &QPushButton::clicked, this, &Menu::launch_game);
    connect(pushButton_2, &QPushButton::clicked, this, &Menu::openSettings);
    connect(pushButton_3, &QPushButton::clicked, this, &Menu::openAbout);
    
    _game = nullptr;
}

void Menu::setupUi() {
    // 设置窗口大小
    this->resize(1000, 800);
    
    // 创建背景标签
    background = new QLabel(this);
    background->setGeometry(0, 0, 1000, 800); // 设置固定大小
    background->lower(); // 确保背景在底层
    
    // 创建按钮并设置位置
    pushButton = new QPushButton(this);
    pushButton->setMinimumHeight(50);
    pushButton->setGeometry(350, 600, 300, 50); // 调整位置和大小
    QFont font;
    font.setPointSize(14);
    font.setBold(true);
    pushButton->setFont(font);
    
    pushButton_2 = new QPushButton(this);
    pushButton_2->setMinimumHeight(50);
    pushButton_2->setGeometry(350, 660, 300, 50); // 调整位置和大小
    pushButton_2->setFont(font);
    
    pushButton_3 = new QPushButton(this);
    pushButton_3->setMinimumHeight(50);
    pushButton_3->setGeometry(350, 720, 300, 50); // 调整位置和大小
    pushButton_3->setFont(font);
    
    // 确保按钮在前景
    pushButton->raise();
    pushButton_2->raise();
    pushButton_3->raise();
    
    // 设置按钮样式，使其半透明以便显示在背景图片上
    QString buttonStyle = "QPushButton { background-color: rgba(45, 45, 45, 180); color: white; border: 2px solid rgba(255, 255, 255, 100); border-radius: 6px; padding: 5px 10px; }";
    pushButton->setStyleSheet(buttonStyle);
    pushButton_2->setStyleSheet(buttonStyle);
    pushButton_3->setStyleSheet(buttonStyle);
}

Menu::~Menu() {
    // 不再需要删除ui
}

// 启动游戏
void Menu::launch_game() {
    // 直接创建游戏实例并进入游戏，使用默认英雄类型1
    if(_game){
        delete _game;
        _game = nullptr;
    }
    auto * new_game = new GameMain(1);
    new_game->show();
    new_game->setWidgetParent(this);
    this->hide();
    _game = new_game;
}

// 打开设置对话框
void Menu::openSettings() {
    SettingDialog dialog(this);
    dialog.exec();
}

// 打开关于对话框
void Menu::openAbout() {
    AboutDialog dialog(this);
    dialog.exec();
}

// 游戏暂停处理
void Menu::reportGamePaused() {
    _game->game->gameHalt();
}

// 游戏结束处理
void Menu::reportGameOver() {
    _game = nullptr;
}

// 禁用继续游戏按钮
void Menu::disableResume() {
    // 此功能已不再需要
}
