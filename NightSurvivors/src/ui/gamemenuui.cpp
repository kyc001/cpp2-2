#include "../../include/ui/gamemenuui.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

GameMenuUI::GameMenuUI(QWidget *parent) : QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(300, 400);
    hide();
    
    setupUI();
}

GameMenuUI::~GameMenuUI()
{
}

void GameMenuUI::setupUI()
{
    QVBoxLayout *main_layout = new QVBoxLayout(this);
    
    // 创建标题
    QLabel *title_label = new QLabel("游戏菜单", this);
    title_label->setAlignment(Qt::AlignCenter);
    title_label->setStyleSheet("color: white; font-size: 24px; font-weight: bold; margin-bottom: 20px;");
    
    // 创建按钮
    resume_button = new QPushButton("继续游戏", this);
    resume_button->setMinimumSize(200, 40);
    resume_button->setStyleSheet("background-color: #4a90e2; color: white; border: none; border-radius: 5px; font-size: 16px;");
    
    settings_button = new QPushButton("设置", this);
    settings_button->setMinimumSize(200, 40);
    settings_button->setStyleSheet("background-color: #5a6268; color: white; border: none; border-radius: 5px; font-size: 16px;");
    
    save_button = new QPushButton("保存游戏", this);
    save_button->setMinimumSize(200, 40);
    save_button->setStyleSheet("background-color: #5a6268; color: white; border: none; border-radius: 5px; font-size: 16px;");
    
    main_menu_button = new QPushButton("返回主菜单", this);
    main_menu_button->setMinimumSize(200, 40);
    main_menu_button->setStyleSheet("background-color: #5a6268; color: white; border: none; border-radius: 5px; font-size: 16px;");
    
    quit_button = new QPushButton("退出游戏", this);
    quit_button->setMinimumSize(200, 40);
    quit_button->setStyleSheet("background-color: #e24a4a; color: white; border: none; border-radius: 5px; font-size: 16px;");
    
    // 添加到主布局
    main_layout->addWidget(title_label);
    main_layout->addWidget(resume_button, 0, Qt::AlignCenter);
    main_layout->addSpacing(10);
    main_layout->addWidget(settings_button, 0, Qt::AlignCenter);
    main_layout->addSpacing(10);
    main_layout->addWidget(save_button, 0, Qt::AlignCenter);
    main_layout->addSpacing(10);
    main_layout->addWidget(main_menu_button, 0, Qt::AlignCenter);
    main_layout->addSpacing(10);
    main_layout->addWidget(quit_button, 0, Qt::AlignCenter);
    main_layout->addStretch(1);
    
    setLayout(main_layout);
    
    // 设置样式
    this->setStyleSheet("QWidget { background-color: rgba(40, 40, 40, 220); border-radius: 10px; padding: 20px; }");
    
    // 连接信号
    connect(resume_button, &QPushButton::clicked, this, &GameMenuUI::onResumeButtonClicked);
    connect(settings_button, &QPushButton::clicked, this, &GameMenuUI::onSettingsButtonClicked);
    connect(save_button, &QPushButton::clicked, this, &GameMenuUI::onSaveButtonClicked);
    connect(main_menu_button, &QPushButton::clicked, this, &GameMenuUI::onMainMenuButtonClicked);
    connect(quit_button, &QPushButton::clicked, this, &GameMenuUI::onQuitButtonClicked);
}

void GameMenuUI::show()
{
    // 确保窗口保持在前
    setWindowFlag(Qt::WindowStaysOnTopHint, true);
    activateWindow();
    raise();
    
    // 居中显示
    centerUI();
    
    QWidget::show();
}

void GameMenuUI::hide()
{
    QWidget::hide();
}

bool GameMenuUI::isVisible() const
{
    return QWidget::isVisible();
}

void GameMenuUI::onResumeButtonClicked()
{
    hide();
    emit resumeGameSignal();
}

void GameMenuUI::onSettingsButtonClicked()
{
    emit settingsOpenSignal();
}

void GameMenuUI::onSaveButtonClicked()
{
    emit saveGameSignal();
}

void GameMenuUI::onMainMenuButtonClicked()
{
    hide();
    emit mainMenuSignal();
}

void GameMenuUI::onQuitButtonClicked()
{
    emit quitGameSignal();
}

void GameMenuUI::centerUI()
{
    if (parentWidget()) {
        QRect parent_rect = parentWidget()->rect();
        move(parentWidget()->mapToGlobal(QPoint(
            parent_rect.width()/2 - width()/2,
            parent_rect.height()/2 - height()/2
        )));
    }
} 