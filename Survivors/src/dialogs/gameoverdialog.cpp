//
// 游戏结束对话框实现文件
//

#include "gameoverdialog.h"
#include "../ui/menu.h"

GameOverDialog::GameOverDialog(Menu * parent, int e_cnt, double h_percent, int s_cnt) :
        QDialog(nullptr), m(parent) {
    setupUi();
    setAttribute(Qt::WA_DeleteOnClose);
    
    // 设置击杀敌人数量
    enemy_cnt->setText("本次击杀敌人数量：" + QString::number(e_cnt));
    
    // 设置剩余血量百分比
    hp_percent->setText("剩余血量百分比：" + QString::number((double)(int)(h_percent * 10000) / 100.0) + "%");
    
    // 计算获得的灵魂
    int delta_soul = int(e_cnt * (h_percent * 0.5 + 0.5));
    SOUL += delta_soul;
    
    // 设置灵魂统计
    soul_cnt->setText("本次结算分数：" + QString::number(delta_soul) + "(" + QString::number(SOUL) + ")");

    // 确认按钮点击事件
    connect(pushButton, &QPushButton::clicked, [&](){
        this->hide();
        if(m){
            m->show();
        }
    });
}

void GameOverDialog::setupUi() {
    // 设置窗口标题和大小
    setWindowTitle("游戏结束");
    resize(400, 300);
    
    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // 创建游戏结束标签
    gameOverLabel = new QLabel("游戏结束!", this);
    QFont titleFont;
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    gameOverLabel->setFont(titleFont);
    gameOverLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(gameOverLabel);
    
    // 创建统计标签
    enemy_cnt = new QLabel("本次击杀敌人数量：0", this);
    enemy_cnt->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(enemy_cnt);
    
    hp_percent = new QLabel("剩余血量百分比：0%", this);
    hp_percent->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(hp_percent);
    
    soul_cnt = new QLabel("本次结算分数：0(0)", this);
    soul_cnt->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(soul_cnt);
    
    // 添加一个弹性空间
    mainLayout->addStretch();
    
    // 创建确认按钮
    pushButton = new QPushButton("确认", this);
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(pushButton);
    buttonLayout->addStretch();
    mainLayout->addLayout(buttonLayout);
    
    setLayout(mainLayout);
}

GameOverDialog::~GameOverDialog() {
    // 不需要显式释放内存，Qt的父子关系会处理
}
