#include "../../include/ui/introductionui.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPixmap>

IntroductionUI::IntroductionUI(QWidget *parent) : QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(800, 600);
    hide();
    
    setupUI();
}

IntroductionUI::~IntroductionUI()
{
}

void IntroductionUI::setupUI()
{
    QVBoxLayout *main_layout = new QVBoxLayout(this);
    
    // 创建标题
    QLabel *title_label = new QLabel("游戏指南", this);
    title_label->setAlignment(Qt::AlignCenter);
    title_label->setStyleSheet("color: white; font-size: 24px; font-weight: bold;");
    
    // 创建选项卡控件
    tab_widget = new QTabWidget(this);
    tab_widget->setStyleSheet(
        "QTabWidget::pane { border: 1px solid #444; background-color: rgba(40, 40, 40, 220); } "
        "QTabBar::tab { background-color: #444; color: white; padding: 8px 16px; } "
        "QTabBar::tab:selected { background-color: #666; } "
        "QTabBar::tab:hover { background-color: #555; }"
    );
    
    // 添加选项卡
    tab_widget->addTab(createControlsTab(), "操作控制");
    tab_widget->addTab(createHeroesTab(), "英雄介绍");
    tab_widget->addTab(createEnemiesTab(), "敌人介绍");
    
    // 创建关闭按钮
    close_button = new QPushButton("关闭", this);
    close_button->setMinimumSize(100, 30);
    close_button->setStyleSheet("background-color: #e24a4a; color: white; border: none; border-radius: 5px;");
    
    // 添加到主布局
    main_layout->addWidget(title_label);
    main_layout->addWidget(tab_widget, 1);
    main_layout->addWidget(close_button, 0, Qt::AlignCenter);
    
    setLayout(main_layout);
    
    // 设置样式
    this->setStyleSheet("QWidget { background-color: rgba(40, 40, 40, 220); border-radius: 10px; padding: 10px; }");
    
    // 连接信号
    connect(close_button, &QPushButton::clicked, this, &IntroductionUI::onCloseButtonClicked);
}

QWidget* IntroductionUI::createControlsTab()
{
    QWidget *tab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(tab);
    
    QScrollArea *scroll_area = new QScrollArea();
    scroll_area->setWidgetResizable(true);
    scroll_area->setFrameShape(QFrame::NoFrame);
    
    QWidget *scroll_content = new QWidget();
    QVBoxLayout *content_layout = new QVBoxLayout(scroll_content);
    
    // 添加操作说明
    QLabel *movement_title = new QLabel("移动控制");
    movement_title->setStyleSheet("color: white; font-size: 18px; font-weight: bold;");
    
    QLabel *movement_desc = new QLabel(
        "- W/↑: 向上移动\n"
        "- A/←: 向左移动\n"
        "- S/↓: 向下移动\n"
        "- D/→: 向右移动\n"
        "- 或使用鼠标点击控制角色移动方向（取决于设置中的控制方式）"
    );
    movement_desc->setStyleSheet("color: white; font-size: 16px;");
    movement_desc->setWordWrap(true);
    
    QLabel *game_title = new QLabel("游戏控制");
    game_title->setStyleSheet("color: white; font-size: 18px; font-weight: bold;");
    
    QLabel *game_desc = new QLabel(
        "- ESC: 打开/关闭游戏菜单\n"
        "- 空格键: 暂停游戏\n"
        "- 数字键1-5: 快速选择技能升级选项\n"
        "- Tab: 查看详细状态"
    );
    game_desc->setStyleSheet("color: white; font-size: 16px;");
    game_desc->setWordWrap(true);
    
    QLabel *tips_title = new QLabel("游戏提示");
    tips_title->setStyleSheet("color: white; font-size: 18px; font-weight: bold;");
    
    QLabel *tips_desc = new QLabel(
        "- 尽量保持移动以避开敌人\n"
        "- 收集经验宝石以升级\n"
        "- 优先选择互补的技能和武器\n"
        "- 时刻留意敌人的生成方向\n"
        "- 升级武器比获取新武器更重要"
    );
    tips_desc->setStyleSheet("color: white; font-size: 16px;");
    tips_desc->setWordWrap(true);
    
    // 添加到布局
    content_layout->addWidget(movement_title);
    content_layout->addWidget(movement_desc);
    content_layout->addSpacing(20);
    content_layout->addWidget(game_title);
    content_layout->addWidget(game_desc);
    content_layout->addSpacing(20);
    content_layout->addWidget(tips_title);
    content_layout->addWidget(tips_desc);
    content_layout->addStretch(1);
    
    scroll_area->setWidget(scroll_content);
    layout->addWidget(scroll_area);
    
    return tab;
}

QWidget* IntroductionUI::createHeroesTab()
{
    QWidget *tab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(tab);
    
    QScrollArea *scroll_area = new QScrollArea();
    scroll_area->setWidgetResizable(true);
    scroll_area->setFrameShape(QFrame::NoFrame);
    
    QWidget *scroll_content = new QWidget();
    QVBoxLayout *content_layout = new QVBoxLayout(scroll_content);
    
    // 战士介绍
    QLabel *warrior_title = new QLabel("战士");
    warrior_title->setStyleSheet("color: #ffbb00; font-size: 20px; font-weight: bold;");
    
    QLabel *warrior_desc = new QLabel(
        "初始属性:\n"
        "- 生命值: 高\n"
        "- 移动速度: 中等\n"
        "- 初始武器: 短剑\n"
        "- 特殊能力: 生命恢复速度提高\n\n"
        "适合玩法:\n"
        "战士适合初学者，拥有较高的生存能力，可以抵抗更多伤害。推荐使用近战武器和防御道具。"
    );
    warrior_desc->setStyleSheet("color: white; font-size: 16px;");
    warrior_desc->setWordWrap(true);
    
    // 法师介绍
    QLabel *mage_title = new QLabel("法师");
    mage_title->setStyleSheet("color: #00aaff; font-size: 20px; font-weight: bold;");
    
    QLabel *mage_desc = new QLabel(
        "初始属性:\n"
        "- 生命值: 低\n"
        "- 移动速度: 中等\n"
        "- 初始武器: 魔法飞弹\n"
        "- 特殊能力: 武器攻击范围增加\n\n"
        "适合玩法:\n"
        "法师依赖远程攻击和躲避伤害。推荐使用区域伤害武器和冷却道具，尽量保持与敌人的距离。"
    );
    mage_desc->setStyleSheet("color: white; font-size: 16px;");
    mage_desc->setWordWrap(true);
    
    // 弓箭手介绍
    QLabel *archer_title = new QLabel("弓箭手");
    archer_title->setStyleSheet("color: #22cc44; font-size: 20px; font-weight: bold;");
    
    QLabel *archer_desc = new QLabel(
        "初始属性:\n"
        "- 生命值: 中等\n"
        "- 移动速度: 高\n"
        "- 初始武器: 弓箭\n"
        "- 特殊能力: 武器穿透力增加\n\n"
        "适合玩法:\n"
        "弓箭手擅长单体高伤害和快速移动，适合走位风格的玩家。推荐使用高攻速武器和移动速度加成道具。"
    );
    archer_desc->setStyleSheet("color: white; font-size: 16px;");
    archer_desc->setWordWrap(true);
    
    // 盗贼介绍
    QLabel *thief_title = new QLabel("盗贼");
    thief_title->setStyleSheet("color: #cc44cc; font-size: 20px; font-weight: bold;");
    
    QLabel *thief_desc = new QLabel(
        "初始属性:\n"
        "- 生命值: 中低\n"
        "- 移动速度: 极高\n"
        "- 初始武器: 匕首\n"
        "- 特殊能力: 获得更多金币和道具\n\n"
        "适合玩法:\n"
        "盗贼依靠高速移动躲避敌人，适合经验丰富的玩家。推荐注重道具收集和暴击伤害的构建。"
    );
    thief_desc->setStyleSheet("color: white; font-size: 16px;");
    thief_desc->setWordWrap(true);
    
    // 添加到布局
    content_layout->addWidget(warrior_title);
    content_layout->addWidget(warrior_desc);
    content_layout->addSpacing(20);
    content_layout->addWidget(mage_title);
    content_layout->addWidget(mage_desc);
    content_layout->addSpacing(20);
    content_layout->addWidget(archer_title);
    content_layout->addWidget(archer_desc);
    content_layout->addSpacing(20);
    content_layout->addWidget(thief_title);
    content_layout->addWidget(thief_desc);
    content_layout->addStretch(1);
    
    scroll_area->setWidget(scroll_content);
    layout->addWidget(scroll_area);
    
    return tab;
}

QWidget* IntroductionUI::createEnemiesTab()
{
    QWidget *tab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(tab);
    
    QScrollArea *scroll_area = new QScrollArea();
    scroll_area->setWidgetResizable(true);
    scroll_area->setFrameShape(QFrame::NoFrame);
    
    QWidget *scroll_content = new QWidget();
    QVBoxLayout *content_layout = new QVBoxLayout(scroll_content);
    
    // 敌人类型介绍
    QLabel *basic_title = new QLabel("基础敌人");
    basic_title->setStyleSheet("color: #22cc44; font-size: 20px; font-weight: bold;");
    
    QLabel *basic_desc = new QLabel(
        "- 骷髅: 低生命值，中等速度，成群出现\n"
        "- 蝙蝠: 低生命值，高速度，容易躲避\n"
        "- 僵尸: 中等生命值，低速度，造成中等伤害\n"
        "- 幽灵: 低生命值，可穿墙，不规则移动"
    );
    basic_desc->setStyleSheet("color: white; font-size: 16px;");
    basic_desc->setWordWrap(true);
    
    QLabel *elite_title = new QLabel("精英敌人");
    elite_title->setStyleSheet("color: #ffbb00; font-size: 20px; font-weight: bold;");
    
    QLabel *elite_desc = new QLabel(
        "- 地狱猎犬: 高速度，追踪玩家，中等生命值\n"
        "- 巫师: 远程攻击，召唤小怪，低生命值\n"
        "- 石像鬼: 高生命值，定期进入无敌状态，造成高伤害\n"
        "- 血肉傀儡: 极高生命值，低速度，死亡时分裂成小怪"
    );
    elite_desc->setStyleSheet("color: white; font-size: 16px;");
    elite_desc->setWordWrap(true);
    
    QLabel *boss_title = new QLabel("BOSS敌人");
    boss_title->setStyleSheet("color: #ff0000; font-size: 20px; font-weight: bold;");
    
    QLabel *boss_desc = new QLabel(
        "- 死亡骑士: 10分钟刷新，高速冲锋攻击，召唤骷髅\n"
        "- 魔女: 20分钟刷新，远程魔法攻击，定期瞬移，召唤精英敌人\n"
        "- 恶魔领主: 30分钟刷新，极高生命值，多阶段战斗，多种攻击方式\n"
        "- 死神: 最终BOSS，极度危险，击败后游戏胜利"
    );
    boss_desc->setStyleSheet("color: white; font-size: 16px;");
    boss_desc->setWordWrap(true);
    
    QLabel *special_title = new QLabel("特殊敌人");
    special_title->setStyleSheet("color: #00aaff; font-size: 20px; font-weight: bold;");
    
    QLabel *special_desc = new QLabel(
        "- 宝箱怪: 看起来像宝箱，但攻击玩家，击败后掉落稀有物品\n"
        "- 金币守卫: 不主动攻击，但被动高防御，击败后掉落大量金币\n"
        "- 幸运精灵: 极速移动，难以击中，击败后获得特殊能力\n"
        "- 诅咒之影: 偶尔出现，不造成伤害，但会使周围敌人变强"
    );
    special_desc->setStyleSheet("color: white; font-size: 16px;");
    special_desc->setWordWrap(true);
    
    // 添加到布局
    content_layout->addWidget(basic_title);
    content_layout->addWidget(basic_desc);
    content_layout->addSpacing(20);
    content_layout->addWidget(elite_title);
    content_layout->addWidget(elite_desc);
    content_layout->addSpacing(20);
    content_layout->addWidget(boss_title);
    content_layout->addWidget(boss_desc);
    content_layout->addSpacing(20);
    content_layout->addWidget(special_title);
    content_layout->addWidget(special_desc);
    content_layout->addStretch(1);
    
    scroll_area->setWidget(scroll_content);
    layout->addWidget(scroll_area);
    
    return tab;
}

void IntroductionUI::show()
{
    QWidget::show();
}

void IntroductionUI::hide()
{
    QWidget::hide();
}

bool IntroductionUI::isVisible() const
{
    return QWidget::isVisible();
}

void IntroductionUI::onCloseButtonClicked()
{
    hide();
    emit introductionUIClosed();
} 