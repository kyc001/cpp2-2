#include "heroselectdialog.h"
#include <QDebug>

HeroSelectDialog::HeroSelectDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("选择角色");
    setFixedSize(800, 600);
    setModal(true); // 设置为模态对话框
    
    // 创建主布局
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    
    // 添加标题
    titleLabel = new QLabel("选择你的英雄", this);
    titleLabel->setStyleSheet("font-size: 28px; font-weight: bold; color: #2c3e50;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);
    
    // 添加说明
    infoLabel = new QLabel("不同英雄有不同的攻击方式和升级选项", this);
    infoLabel->setStyleSheet("font-size: 16px; color: #7f8c8d;");
    infoLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(infoLabel);
    
    // 英雄选择布局
    heroesLayout = new QHBoxLayout();
    heroesLayout->setSpacing(30);
    mainLayout->addLayout(heroesLayout);
    
    // 英雄1（哈气耄耋）
    hero1Widget = new QWidget(this);
    hero1Widget->setStyleSheet("background-color: #ecf0f1; border-radius: 10px; padding: 15px;");
    hero1Layout = new QVBoxLayout(hero1Widget);
    
    hero1NameLabel = new QLabel("哈气耄耋", this);
    hero1NameLabel->setStyleSheet("font-size: 22px; font-weight: bold; color: #2980b9;");
    hero1NameLabel->setAlignment(Qt::AlignCenter);
    hero1Layout->addWidget(hero1NameLabel);
    
    hero1ImageLabel = new QLabel(this);
    hero1ImageLabel->setAlignment(Qt::AlignCenter);
    hero1ImageLabel->setFixedSize(200, 200);
    hero1Movie = new QMovie(":/Assets/maodie.gif");
    hero1Movie->setScaledSize(QSize(180, 180));
    hero1ImageLabel->setMovie(hero1Movie);
    hero1Movie->start();
    hero1Layout->addWidget(hero1ImageLabel);
    
    hero1DescLabel = new QLabel("攻击方式：AOE范围攻击\n特点：强力的范围伤害\n升级：提升AOE攻击范围", this);
    hero1DescLabel->setStyleSheet("font-size: 14px; color: #34495e;");
    hero1DescLabel->setAlignment(Qt::AlignCenter);
    hero1Layout->addWidget(hero1DescLabel);
    
    hero1SelectButton = new QPushButton("选择", this);
    hero1SelectButton->setStyleSheet("QPushButton { background-color: #3498db; color: white; padding: 10px; font-size: 16px; border-radius: 5px; }"
                                    "QPushButton:hover { background-color: #2980b9; }");
    hero1Layout->addWidget(hero1SelectButton);
    
    heroesLayout->addWidget(hero1Widget);
    
    // 英雄2（奔跑耄耋）
    hero2Widget = new QWidget(this);
    hero2Widget->setStyleSheet("background-color: #ecf0f1; border-radius: 10px; padding: 15px;");
    hero2Layout = new QVBoxLayout(hero2Widget);
    
    hero2NameLabel = new QLabel("奔跑耄耋", this);
    hero2NameLabel->setStyleSheet("font-size: 22px; font-weight: bold; color: #16a085;");
    hero2NameLabel->setAlignment(Qt::AlignCenter);
    hero2Layout->addWidget(hero2NameLabel);
    
    hero2ImageLabel = new QLabel(this);
    hero2ImageLabel->setAlignment(Qt::AlignCenter);
    hero2ImageLabel->setFixedSize(200, 200);
    hero2Movie = new QMovie(":/Assets/maodie2.gif");
    hero2Movie->setScaledSize(QSize(180, 180));
    hero2ImageLabel->setMovie(hero2Movie);
    hero2Movie->start();
    hero2Layout->addWidget(hero2ImageLabel);
    
    hero2DescLabel = new QLabel("攻击方式：悬浮球环绕\n特点：多目标同时攻击\n升级：提升悬浮球转速", this);
    hero2DescLabel->setStyleSheet("font-size: 14px; color: #34495e;");
    hero2DescLabel->setAlignment(Qt::AlignCenter);
    hero2Layout->addWidget(hero2DescLabel);
    
    hero2SelectButton = new QPushButton("选择", this);
    hero2SelectButton->setStyleSheet("QPushButton { background-color: #1abc9c; color: white; padding: 10px; font-size: 16px; border-radius: 5px; }"
                                    "QPushButton:hover { background-color: #16a085; }");
    hero2Layout->addWidget(hero2SelectButton);
    
    heroesLayout->addWidget(hero2Widget);
    
    // 连接信号和槽
    connect(hero1SelectButton, &QPushButton::clicked, this, &HeroSelectDialog::onHero1Selected);
    connect(hero2SelectButton, &QPushButton::clicked, this, &HeroSelectDialog::onHero2Selected);
}

HeroSelectDialog::~HeroSelectDialog()
{
    if (hero1Movie) {
        hero1Movie->stop();
        delete hero1Movie;
    }
    
    if (hero2Movie) {
        hero2Movie->stop();
        delete hero2Movie;
    }
}

void HeroSelectDialog::onHero1Selected()
{
    selectedHeroType = 1; // 哈气耄耋
    qDebug() << "选择了英雄1: 哈气耄耋";
    accept(); // 关闭对话框并返回QDialog::Accepted
}

void HeroSelectDialog::onHero2Selected()
{
    selectedHeroType = 2; // 奔跑耄耋
    qDebug() << "选择了英雄2: 奔跑耄耋";
    accept(); // 关闭对话框并返回QDialog::Accepted
} 