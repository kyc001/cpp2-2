//
// 游戏关于对话框实现文件
//

#include "aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent)
{
    setupUi();
    
    // 连接关闭按钮的点击信号
    connect(closeButton, &QPushButton::clicked, this, &AboutDialog::onCloseClicked);
}

void AboutDialog::setupUi()
{
    // 设置窗口标题和大小
    setWindowTitle("关于 Survivors");
    resize(500, 400);
    
    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // 创建文本浏览器
    contentBrowser = new QTextBrowser(this);
    
    // 设置HTML内容
    QString aboutText = R"(
    <h2 align="center">Survivors</h2>
    <p align="center">Version 0.1.0</p>
    <hr />
    <p>《Survivors》是一款生存类游戏，玩家将扮演一名勇士，在不断涌现的敌人中生存下来。</p>
    <h3>游戏特点：</h3>
    <ul>
      <li>自动攻击周围敌人</li>
      <li>随着时间推移敌人难度增加</li>
      <li>收集经验升级变强</li>
      <li>三种彩色悬浮球围绕角色旋转，提供额外保护</li>
      <li>支持键盘WASD和鼠标跟随两种控制方式</li>
    </ul>
    <h3>操作说明：</h3>
    <ul>
      <li>WASD键：控制角色移动</li>
      <li>或使用鼠标：角色将自动跟随鼠标位置移动</li>
    </ul>
    <p>Copyright © 2023-2024. All rights reserved.</p>
    )";
    
    contentBrowser->setHtml(aboutText);
    mainLayout->addWidget(contentBrowser);
    
    // 创建按钮布局
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    // 创建关闭按钮
    closeButton = new QPushButton("关闭", this);
    buttonLayout->addWidget(closeButton);
    
    // 添加按钮布局到主布局
    mainLayout->addLayout(buttonLayout);
    
    setLayout(mainLayout);
}

AboutDialog::~AboutDialog()
{
    // 不需要显式释放内存，Qt的父子关系会处理
}

void AboutDialog::onCloseClicked()
{
    // 关闭对话框
    accept();
}