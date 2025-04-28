//
// 升级选择对话框实现文件
//

#include "upgradedialog.h"
#include "../gamemain.h"
#include <QIcon>
#include <QPixmap>

UpgradeDialog::UpgradeDialog(GameMain * game_widget, QWidget *parent) :
        QDialog(parent), parent(game_widget) {
    setupUi();

    // 连接选择按钮
    connect(choice1, &QPushButton::clicked, [&](){ choose1();});
    connect(choice2, &QPushButton::clicked, [&](){ choose2();});
    connect(choice3, &QPushButton::clicked, [&](){ choose3();});

    // 设置按钮文本
    choice1->setText("更多血量");
    choice1->setToolTip("提高最大生命值");
    choice2->setText("更快速度");
    choice2->setToolTip("提高移动速度");
    choice3->setText("更强武器");
    choice3->setToolTip("提高武器伤害");
}

void UpgradeDialog::setupUi() {
    // 设置窗口标题和大小
    setWindowTitle("升级选择");
    resize(500, 400);
    
    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // 创建标题标签
    titleLabel = new QLabel("选择一个升级", this);
    QFont titleFont;
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);
    
    // 创建按钮布局
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    // 创建三个选择按钮
    choice1 = new QPushButton(this);
    choice1->setMinimumSize(100, 250);
    
    choice2 = new QPushButton(this);
    choice2->setMinimumSize(100, 250);
    
    choice3 = new QPushButton(this);
    choice3->setMinimumSize(100, 250);
    
    // 添加按钮到布局
    buttonLayout->addWidget(choice1);
    buttonLayout->addWidget(choice2);
    buttonLayout->addWidget(choice3);
    
    // 添加按钮布局到主布局
    mainLayout->addLayout(buttonLayout);
    
    setLayout(mainLayout);
}

UpgradeDialog::~UpgradeDialog() {
    // 不需要显式释放内存，Qt的父子关系会处理
}

// 选择选项1
void UpgradeDialog::choose1() {
    parent->game->upgrade(1);
    parent->resumeGame();
    this->hide();
    delete this;
}

// 选择选项2
void UpgradeDialog::choose2() {
    parent->game->upgrade(2);
    parent->resumeGame();
    this->hide();
    delete this;
}

// 选择选项3
void UpgradeDialog::choose3() {
    parent->game->upgrade(3);
    parent->resumeGame();
    this->hide();
    delete this;
}