//
// 升级选择对话框实现文件
//

#include "upgradedialog.h"
#include "../gamemain.h"
#include <QIcon>
#include <QPixmap>
#include <QDebug>
#include <QTimer>

UpgradeDialog::UpgradeDialog(GameMain * game_widget, QWidget *parent) :
        QDialog(parent), parent(game_widget) {
    qDebug() << "[Upgrade] 创建升级对话框，父窗口地址:" << game_widget;
    
    // 安全检查
    if(!game_widget) {
        qWarning() << "[Upgrade] 无效的父窗口指针";
        QTimer::singleShot(0, this, &QDialog::reject); // 安全关闭无效对话框
        return;
    }
    
    // 初始化UI
    try {
        setupUi();
        
        // 设置窗口属性
        setWindowTitle("升级选择");
        setWindowModality(Qt::NonModal); // 确保非模态
        setAttribute(Qt::WA_DeleteOnClose, false); // 不自动删除，我们会手动处理
        
        // 连接选择按钮
        connect(choice1, &QPushButton::clicked, this, [this](){
            qDebug() << "[Upgrade] 按钮1被点击";
            choose1();
        });
        connect(choice2, &QPushButton::clicked, this, [this](){
            qDebug() << "[Upgrade] 按钮2被点击";
            choose2();
        });
        connect(choice3, &QPushButton::clicked, this, [this](){
            qDebug() << "[Upgrade] 按钮3被点击";
            choose3();
        });
        
        // 设置按钮文本
        choice1->setText("更多血量");
        choice1->setToolTip("提高最大生命值");
        choice2->setText("更快速度");
        choice2->setToolTip("提高移动速度");
        choice3->setText("更强武器");
        choice3->setToolTip("提高武器伤害");
        
        qDebug() << "[Upgrade] 对话框初始化完成";
    } catch (const std::exception& e) {
        qCritical() << "[Upgrade] 初始化异常:" << e.what();
        QTimer::singleShot(0, this, &QDialog::reject);
    } catch (...) {
        qCritical() << "[Upgrade] 初始化时发生未知异常";
        QTimer::singleShot(0, this, &QDialog::reject);
    }
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
    qDebug() << "[Upgrade] 销毁升级对话框";
    // 不需要显式释放内存，Qt的父子关系会处理
}

// 选择选项1
void UpgradeDialog::choose1() {
    qDebug() << "[Upgrade] 选择升级选项1开始";
    
    // 立即禁用所有按钮，防止多次点击
    choice1->setEnabled(false);
    choice2->setEnabled(false);
    choice3->setEnabled(false);
    
    // 先隐藏对话框，避免卡顿感
    this->hide();
    
    // 使用安全的延迟方式执行升级和恢复游戏
    QTimer::singleShot(0, this, [this]() {
        try {
            // 执行升级
            if (parent && parent->game) {
                qDebug() << "[Upgrade] 调用 upgrade(1)";
                parent->game->upgrade(1);
                qDebug() << "[Upgrade] upgrade(1) 调用完成";
            } else {
                qWarning() << "[Upgrade] 父窗口或游戏状态为空，跳过升级";
            }
            
            // 恢复游戏（延迟执行）
            QTimer::singleShot(100, this, [this]() {
                if (parent) {
                    qDebug() << "[Upgrade] 调用 resumeGame";
                    parent->resumeGame();
                    qDebug() << "[Upgrade] resumeGame 调用完成";
                }
                // 最后安全删除对话框
                qDebug() << "[Upgrade] 对话框准备关闭";
                this->deleteLater();
            });
        } catch (const std::exception& e) {
            qCritical() << "[Upgrade] 升级过程异常:" << e.what();
            if (parent) parent->resumeGame();
            this->deleteLater();
        } catch (...) {
            qCritical() << "[Upgrade] 升级过程未知异常";
            if (parent) parent->resumeGame();
            this->deleteLater();
        }
    });
    
    qDebug() << "[Upgrade] 选择升级选项1结束";
}

// 选择选项2
void UpgradeDialog::choose2() {
    qDebug() << "[Upgrade] 选择升级选项2开始";
    
    // 立即禁用所有按钮，防止多次点击
    choice1->setEnabled(false);
    choice2->setEnabled(false);
    choice3->setEnabled(false);
    
    // 先隐藏对话框，避免卡顿感
    this->hide();
    
    // 使用安全的延迟方式执行升级和恢复游戏
    QTimer::singleShot(0, this, [this]() {
        try {
            // 执行升级
            if (parent && parent->game) {
                qDebug() << "[Upgrade] 调用 upgrade(2)";
                parent->game->upgrade(2);
                qDebug() << "[Upgrade] upgrade(2) 调用完成";
            } else {
                qWarning() << "[Upgrade] 父窗口或游戏状态为空，跳过升级";
            }
            
            // 恢复游戏（延迟执行）
            QTimer::singleShot(100, this, [this]() {
                if (parent) {
                    qDebug() << "[Upgrade] 调用 resumeGame";
                    parent->resumeGame();
                    qDebug() << "[Upgrade] resumeGame 调用完成";
                }
                // 最后安全删除对话框
                qDebug() << "[Upgrade] 对话框准备关闭";
                this->deleteLater();
            });
        } catch (const std::exception& e) {
            qCritical() << "[Upgrade] 升级过程异常:" << e.what();
            if (parent) parent->resumeGame();
            this->deleteLater();
        } catch (...) {
            qCritical() << "[Upgrade] 升级过程未知异常";
            if (parent) parent->resumeGame();
            this->deleteLater();
        }
    });
    
    qDebug() << "[Upgrade] 选择升级选项2结束";
}

// 选择选项3
void UpgradeDialog::choose3() {
    qDebug() << "[Upgrade] 选择升级选项3开始";
    
    // 立即禁用所有按钮，防止多次点击
    choice1->setEnabled(false);
    choice2->setEnabled(false);
    choice3->setEnabled(false);
    
    // 先隐藏对话框，避免卡顿感
    this->hide();
    
    // 使用安全的延迟方式执行升级和恢复游戏
    QTimer::singleShot(0, this, [this]() {
        try {
            // 执行升级
            if (parent && parent->game) {
                qDebug() << "[Upgrade] 调用 upgrade(3)";
                parent->game->upgrade(3);
                qDebug() << "[Upgrade] upgrade(3) 调用完成";
            } else {
                qWarning() << "[Upgrade] 父窗口或游戏状态为空，跳过升级";
            }
            
            // 恢复游戏（延迟执行）
            QTimer::singleShot(100, this, [this]() {
                if (parent) {
                    qDebug() << "[Upgrade] 调用 resumeGame";
                    parent->resumeGame();
                    qDebug() << "[Upgrade] resumeGame 调用完成";
                }
                // 最后安全删除对话框
                qDebug() << "[Upgrade] 对话框准备关闭";
                this->deleteLater();
            });
        } catch (const std::exception& e) {
            qCritical() << "[Upgrade] 升级过程异常:" << e.what();
            if (parent) parent->resumeGame();
            this->deleteLater();
        } catch (...) {
            qCritical() << "[Upgrade] 升级过程未知异常";
            if (parent) parent->resumeGame();
            this->deleteLater();
        }
    });
    
    qDebug() << "[Upgrade] 选择升级选项3结束";
}