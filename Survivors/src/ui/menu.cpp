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
    background->setPixmap(QPixmap(":/image/Assets/bg.jpg").scaled(1000, 800));

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
    
    // 创建布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // 创建背景标签
    background = new QLabel(this);
    mainLayout->addWidget(background);
    
    // 创建按钮布局
    QVBoxLayout *buttonLayout = new QVBoxLayout();
    buttonLayout->setSpacing(12);
    buttonLayout->setContentsMargins(350, 0, 350, 80);
    
    // 创建按钮
    pushButton = new QPushButton(this);
    pushButton->setMinimumHeight(50);
    QFont font;
    font.setPointSize(14);
    font.setBold(true);
    pushButton->setFont(font);
    
    pushButton_2 = new QPushButton(this);
    pushButton_2->setMinimumHeight(50);
    pushButton_2->setFont(font);
    
    pushButton_3 = new QPushButton(this);
    pushButton_3->setMinimumHeight(50);
    pushButton_3->setFont(font);
    
    // 添加按钮到布局
    buttonLayout->addWidget(pushButton);
    buttonLayout->addWidget(pushButton_2);
    buttonLayout->addWidget(pushButton_3);
    
    // 添加按钮布局到主布局
    mainLayout->addLayout(buttonLayout);
    
    setLayout(mainLayout);
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
