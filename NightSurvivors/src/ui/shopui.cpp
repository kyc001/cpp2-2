#include "../../include/ui/shopui.h"
#include "../../include/core/gamestate.h"
#include "../../include/ui/mainscene.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QGroupBox>
#include <QSpacerItem>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include <QToolTip>
#include <QCursor>
#include <QEvent>

// ShopItem 实现

int ShopItem::getCostForLevel(int level) const {
    // 每级增加基础花费的30%
    return static_cast<int>(base_cost * (1.0 + 0.3 * (level - 1)));
}

// ShopUI 实现

ShopUI::ShopUI(MainScene* scene, QWidget *parent)
    : QWidget(parent),
      game_state(scene ? scene->getGameState() : nullptr)
{
    // 设置无边框窗口
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    
    // 设置半透明黑色背景
    setStyleSheet("background-color: rgba(0, 0, 0, 180);");
    
    // 设置对象名称，便于样式表选择
    setObjectName("shopUI");
    
    // 初始化商店项目
    initShopItems();
    
    // 设置UI
    setupUI();
    
    // 默认隐藏
    hide();
}

ShopUI::~ShopUI()
{
    // QWidget会自动删除子控件，不需要手动删除
}

void ShopUI::setupUI()
{
    // 创建主布局
    main_layout = new QVBoxLayout(this);
    main_layout->setSpacing(20);
    main_layout->setContentsMargins(50, 50, 50, 50);
    
    // 创建标题和金币显示区
    QHBoxLayout* header_layout = new QHBoxLayout();
    
    title_label = new QLabel("商店", this);
    title_label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    title_label->setStyleSheet("color: white; font-size: 32px; font-weight: bold;");
    
    // 添加阴影效果
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect(title_label);
    shadow->setColor(QColor(0, 0, 0, 180));
    shadow->setOffset(2, 2);
    shadow->setBlurRadius(10);
    title_label->setGraphicsEffect(shadow);
    
    coins_label = new QLabel("金币: 0", this);
    coins_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    coins_label->setStyleSheet("color: gold; font-size: 24px; font-weight: bold;");
    
    header_layout->addWidget(title_label, 1);
    header_layout->addWidget(coins_label, 1);
    
    // 创建关闭按钮
    QPushButton* closeButton = new QPushButton("✕", this);
    closeButton->setStyleSheet(
        "QPushButton {"
        "  background-color: rgba(200, 50, 50, 200);"
        "  color: white;"
        "  border: none;"
        "  border-radius: 15px;"
        "  font-size: 16px;"
        "  min-width: 30px;"
        "  min-height: 30px;"
        "  max-width: 30px;"
        "  max-height: 30px;"
        "}"
        "QPushButton:hover {"
        "  background-color: rgba(220, 70, 70, 220);"
        "}"
    );
    closeButton->setCursor(Qt::PointingHandCursor);
    
    // 连接关闭按钮信号
    connect(closeButton, &QPushButton::clicked, this, &ShopUI::onCloseButtonClicked);
    
    // 创建右上角布局来放置关闭按钮
    QHBoxLayout* topLayout = new QHBoxLayout();
    topLayout->addStretch();
    topLayout->addWidget(closeButton);
    
    // 创建商店项目的滚动区域
    items_scroll_area = new QScrollArea(this);
    items_scroll_area->setWidgetResizable(true);
    items_scroll_area->setFrameShape(QFrame::NoFrame);
    items_scroll_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    items_scroll_area->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    items_scroll_area->setStyleSheet(
        "QScrollArea {"
        "  background-color: transparent;"
        "  border: none;"
        "}"
        "QScrollBar:vertical {"
        "  background: rgba(60, 60, 80, 100);"
        "  width: 12px;"
        "  margin: 0px;"
        "}"
        "QScrollBar::handle:vertical {"
        "  background: rgba(100, 100, 140, 200);"
        "  min-height: 20px;"
        "  border-radius: 6px;"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "  height: 0px;"
        "}"
    );
    
    items_container = new QWidget(items_scroll_area);
    items_layout = new QVBoxLayout(items_container);
    items_layout->setSpacing(20);
    items_layout->setContentsMargins(10, 10, 10, 10);
    
    // 添加商店项目
    for (int i = 0; i < shop_items.size(); ++i) {
        QWidget* item_widget = createItemWidget(shop_items[i], i);
        items_layout->addWidget(item_widget);
    }
    
    items_layout->addStretch();
    
    items_scroll_area->setWidget(items_container);
    
    // 添加所有元素到主布局
    main_layout->addLayout(topLayout);
    main_layout->addLayout(header_layout);
    main_layout->addWidget(items_scroll_area, 1);
    
    // 设置初始大小
    resize(900, 700);
    
    // 更新商店项目
    updateShopItems();
}

QWidget* ShopUI::createItemWidget(const ShopItem& item, int index)
{
    // 创建项目容器
    QGroupBox* item_group = new QGroupBox(this);
    item_group->setStyleSheet(
        "QGroupBox {"
        "  background-color: rgba(40, 40, 60, 200);"
        "  border: 2px solid #4A4A6A;"
        "  border-radius: 10px;"
        "  padding: 10px;"
        "}"
    );
    
    // 创建项目布局
    QGridLayout* item_layout = new QGridLayout(item_group);
    item_layout->setSpacing(15);
    
    // 创建项目标题
    QLabel* name_label = new QLabel(item.getName(), item_group);
    name_label->setStyleSheet("color: white; font-size: 22px; font-weight: bold; background: transparent;");
    
    // 创建项目描述
    QLabel* desc_label = new QLabel(item.getDescription(), item_group);
    desc_label->setStyleSheet("color: #CCCCCC; font-size: 16px; background: transparent;");
    desc_label->setWordWrap(true);
    
    // 创建等级标签
    QLabel* level_label = new QLabel("等级: 0/5", item_group);
    level_label->setStyleSheet("color: #AAAAFF; font-size: 18px; background: transparent;");
    level_labels.append(level_label);
    
    // 创建升级按钮
    QPushButton* upgrade_button = new QPushButton("升级 (100金币)", item_group);
    upgrade_button->setMinimumSize(180, 40);
    upgrade_button->setCursor(Qt::PointingHandCursor);
    upgrade_button->setStyleSheet(
        "QPushButton {"
        "  background-color: #5A5AA0;"
        "  color: white;"
        "  border: none;"
        "  border-radius: 5px;"
        "  padding: 8px;"
        "  font-size: 16px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #6A6AC0;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #4A4A80;"
        "}"
        "QPushButton:disabled {"
        "  background-color: #555555;"
        "  color: #AAAAAA;"
        "}"
    );
    
    // 添加图标
    QString iconPath;
    switch (item.getType()) {
        case 0: iconPath = ":/resources/ui/icons/icon_health.png"; break;
        case 1: iconPath = ":/resources/ui/icons/icon_attack.png"; break;
        case 2: iconPath = ":/resources/ui/icons/icon_speed.png"; break;
        case 3: iconPath = ":/resources/ui/icons/icon_range.png"; break;
        case 4: iconPath = ":/resources/ui/icons/icon_coin.png"; break;
        default: iconPath = ":/resources/ui/icons/icon_default.png"; break;
    }
    
    QLabel* icon_label = new QLabel(item_group);
    QPixmap pixmap(iconPath);
    if (!pixmap.isNull()) {
        icon_label->setPixmap(pixmap.scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    icon_label->setMinimumSize(48, 48);
    icon_label->setMaximumSize(48, 48);
    icon_label->setStyleSheet("background: transparent;");
    
    // 连接按钮的点击信号
    connect(upgrade_button, &QPushButton::clicked, this, [this, index]() {
        onUpgradeButtonClicked(shop_items[index].getType());
    });
    
    upgrade_buttons.append(upgrade_button);
    
    // 添加进度条
    QWidget* progress_container = new QWidget(item_group);
    QHBoxLayout* progress_layout = new QHBoxLayout(progress_container);
    progress_layout->setSpacing(5);
    progress_layout->setContentsMargins(0, 0, 0, 0);
    
    // 创建5个进度点，表示等级
    for (int i = 0; i < 5; i++) {
        QLabel* dot = new QLabel(progress_container);
        dot->setMinimumSize(16, 16);
        dot->setMaximumSize(16, 16);
        dot->setProperty("level_dot", true);
        dot->setProperty("level_index", i);
        dot->setProperty("item_index", index);
        dot->setStyleSheet(
            "QLabel[level_dot=\"true\"] {"
            "  background-color: #555555;"
            "  border-radius: 8px;"
            "}"
        );
        progress_layout->addWidget(dot);
    }
    
    progress_layout->addStretch();
    
    // 放置所有元素
    item_layout->addWidget(icon_label, 0, 0, 2, 1, Qt::AlignTop);
    item_layout->addWidget(name_label, 0, 1);
    item_layout->addWidget(desc_label, 1, 1, 1, 2);
    item_layout->addWidget(level_label, 0, 2, Qt::AlignRight);
    item_layout->addWidget(progress_container, 2, 1, 1, 2);
    item_layout->addWidget(upgrade_button, 3, 2, Qt::AlignRight);
    
    item_layout->setColumnStretch(1, 1);
    item_layout->setRowStretch(1, 1);
    
    // 添加鼠标悬停事件显示详细信息
    item_group->setProperty("tooltip_text", item.getDescription());
    item_group->installEventFilter(this);
    
    return item_group;
}

void ShopUI::updateShopItems()
{
    if (!game_state) {
        return;
    }
    
    // 更新金币显示
    coins_label->setText(QString("金币: %1").arg(game_state->getTotalCoins()));
    
    // 更新每个项目的等级和升级按钮状态
    for (int i = 0; i < shop_items.size(); ++i) {
        const ShopItem& item = shop_items[i];
        int current_level = game_state->getGlobalUpgradeLevel(item.getType());
        int max_level = item.getMaxLevel();
        
        // 更新等级标签
        level_labels[i]->setText(QString("等级: %1/%2").arg(current_level).arg(max_level));
        
        // 更新升级按钮
        QPushButton* button = upgrade_buttons[i];
        
        if (current_level >= max_level) {
            // 已达到最高等级
            button->setText("已达最高等级");
            button->setEnabled(false);
        } else {
            // 计算升级费用
            int cost = item.getCostForLevel(current_level + 1);
            button->setText(QString("升级 (%1金币)").arg(cost));
            
            // 检查是否有足够的金币
            button->setEnabled(game_state->getTotalCoins() >= cost);
        }
        
        // 更新等级进度点
        QWidget* container = items_container->findChild<QWidget*>();
        if (container) {
            QList<QLabel*> dots = container->findChildren<QLabel*>("level_dot=true");
            for (QLabel* dot : dots) {
                if (dot->property("item_index").toInt() == i) {
                    int dot_level = dot->property("level_index").toInt();
                    
                    if (dot_level < current_level) {
                        // 已达成的等级
                        dot->setStyleSheet(
                            "QLabel[level_dot=\"true\"] {"
                            "  background-color: #5A5AFF;"
                            "  border-radius: 8px;"
                            "}"
                        );
                    } else {
                        // 未达成的等级
                        dot->setStyleSheet(
                            "QLabel[level_dot=\"true\"] {"
                            "  background-color: #555555;"
                            "  border-radius: 8px;"
                            "}"
                        );
                    }
                }
            }
        }
    }
}

void ShopUI::initShopItems()
{
    // 清空现有项目
    shop_items.clear();
    
    // 添加商店项目
    shop_items.append(ShopItem("生命提升", "增加英雄的最大生命值", 0, 100, 5));
    shop_items.append(ShopItem("攻击强化", "增加英雄的基础攻击力", 1, 150, 5));
    shop_items.append(ShopItem("移动速度", "提高英雄的移动速度", 2, 120, 5));
    shop_items.append(ShopItem("拾取范围", "增加道具的拾取范围", 3, 80, 5));
    shop_items.append(ShopItem("金币收益", "增加从敌人身上获得的金币", 4, 200, 5));
}

void ShopUI::show()
{
    // 居中显示
    centerUI();
    
    // 刷新商店内容
    updateShopItems();
    
    // 显示带有淡入效果
    setWindowOpacity(0.0);
    QWidget::show();
    
    QPropertyAnimation* animation = new QPropertyAnimation(this, "windowOpacity");
    animation->setDuration(300);
    animation->setStartValue(0.0);
    animation->setEndValue(1.0);
    animation->start(QPropertyAnimation::DeleteWhenStopped);
}

void ShopUI::centerUI()
{
    if (parentWidget()) {
        move(parentWidget()->width()/2 - width()/2,
             parentWidget()->height()/2 - height()/2);
    }
}

void ShopUI::onUpgradeButtonClicked(int type)
{
    if (!game_state) {
        return;
    }
    
    // 获取当前等级和升级费用
    int current_level = game_state->getGlobalUpgradeLevel(type);
    
    // 查找对应的商店项目
    int itemIndex = -1;
    for (int i = 0; i < shop_items.size(); ++i) {
        if (shop_items[i].getType() == type) {
            itemIndex = i;
            break;
        }
    }
    
    if (itemIndex == -1) {
        return;
    }
    
    // 计算升级费用
    int cost = shop_items[itemIndex].getCostForLevel(current_level + 1);
    
    // 检查是否有足够的金币
    if (game_state->getTotalCoins() >= cost) {
        // 扣除金币
        game_state->addCoins(-cost);
        
        // 应用升级
        game_state->applyGlobalUpgrade(type, current_level + 1);
        
        // 更新商店显示
        updateShopItems();
    }
}

void ShopUI::onCloseButtonClicked()
{
    // 淡出效果
    QPropertyAnimation* animation = new QPropertyAnimation(this, "windowOpacity");
    animation->setDuration(200);
    animation->setStartValue(1.0);
    animation->setEndValue(0.0);
    
    connect(animation, &QPropertyAnimation::finished, this, [this]() {
        hide();
        emit shopClosed();
    });
    
    animation->start(QPropertyAnimation::DeleteWhenStopped);
}

bool ShopUI::eventFilter(QObject *watched, QEvent *event)
{
    // 处理按钮的悬停提示
    for (int i = 0; i < upgrade_buttons.size(); ++i) {
        if (watched == upgrade_buttons[i]) {
            if (event->type() == QEvent::Type::Enter) {
                // 显示详细提示
                if (game_state) {
                    int current_level = game_state->getGlobalUpgradeLevel(shop_items[i].getType());
                    if (current_level < shop_items[i].getMaxLevel()) {
                        QString tooltip = QString("%1\n当前等级: %2/%3\n下一级效果: +%4%\n花费: %5金币")
                            .arg(shop_items[i].getDescription())
                            .arg(current_level)
                            .arg(shop_items[i].getMaxLevel())
                            .arg(10)
                            .arg(shop_items[i].getCostForLevel(current_level + 1));
                        QToolTip::showText(QCursor::pos(), tooltip, upgrade_buttons[i]);
                    }
                }
                return true;
            } else if (event->type() == QEvent::Type::Leave) {
                QToolTip::hideText();
                return true;
            }
        }
    }
    
    return QWidget::eventFilter(watched, event);
}

ShopUI::ShopUI(GameState* state, QWidget *parent)
    : QWidget(parent),
      game_state(state)
{
    // 调用另一个构造函数的代码，保留向后兼容性
    // 设置无边框窗口
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    
    // 设置半透明黑色背景
    setStyleSheet("background-color: rgba(0, 0, 0, 180);");
    
    // 设置对象名称，便于样式表选择
    setObjectName("shopUI");
    
    // 初始化商店项目
    initShopItems();
    
    // 设置UI
    setupUI();
    
    // 默认隐藏
    hide();
}