#include "../../include/ui/shopui.h"
#include "../../include/core/gamestate.h"
#include "../../include/ui/mainscene.h"
#include <QMessageBox>

// ShopItem 实现

int ShopItem::getCostForLevel(int level) const {
    // 每级升级费用递增
    return base_cost * level * level;
}

// ShopUI 实现

ShopUI::ShopUI(GameState* state, QWidget *parent)
    : QWidget(parent), game_state(state) {
    // 设置窗口样式
    setStyleSheet("background-color: rgba(20, 20, 30, 230);");
    
    // 创建主布局
    main_layout = new QVBoxLayout(this);
    main_layout->setSpacing(20);
    main_layout->setContentsMargins(30, 30, 30, 30);
    
    // 创建标题
    title_label = new QLabel("全局强化商店", this);
    title_label->setAlignment(Qt::AlignCenter);
    title_label->setStyleSheet("color: white; font-size: 28px; font-weight: bold;");
    
    // 创建金币显示
    coins_label = new QLabel(this);
    coins_label->setAlignment(Qt::AlignCenter);
    coins_label->setStyleSheet("color: gold; font-size: 20px;");
    
    // 创建滚动区域显示商店项目
    items_scroll_area = new QScrollArea(this);
    items_scroll_area->setWidgetResizable(true);
    items_scroll_area->setStyleSheet("QScrollArea { border: none; }");
    
    items_container = new QWidget(items_scroll_area);
    items_layout = new QVBoxLayout(items_container);
    items_layout->setSpacing(15);
    
    items_scroll_area->setWidget(items_container);
    
    // 创建关闭按钮
    close_button = new QPushButton("关闭商店", this);
    close_button->setStyleSheet(
        "QPushButton {"
        "   background-color: #3D3D5C;"
        "   color: white;"
        "   border-radius: 10px;"
        "   padding: 10px 20px;"
        "   font-size: 16px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #4D4D7C;"
        "}"
    );
    connect(close_button, &QPushButton::clicked, this, &ShopUI::onCloseButtonClicked);
    
    // 添加组件到主布局
    main_layout->addWidget(title_label);
    main_layout->addWidget(coins_label);
    main_layout->addWidget(items_scroll_area, 1); // 1为拉伸因子，让滚动区域占据剩余空间
    main_layout->addWidget(close_button, 0, Qt::AlignCenter);
    
    // 初始化商店项目
    initShopItems();
    
    // 创建商店项目的UI
    for (int i = 0; i < shop_items.size(); i++) {
        QWidget* item_widget = createItemWidget(shop_items[i], i);
        items_layout->addWidget(item_widget);
    }
    
    // 初始默认隐藏
    hide();
}

ShopUI::ShopUI(MainScene* scene, QWidget *parent)
    : ShopUI(scene->getGameState(), parent) {
}

ShopUI::~ShopUI() {
    // QWidget会自动删除子控件
    delete main_layout;
    delete title_label;
    delete coins_label;
    delete items_scroll_area;
    delete items_container;
    delete items_layout;
    delete close_button;
}

void ShopUI::initShopItems() {
    // 添加各种全局强化选项
    shop_items.append(ShopItem("最大生命值", "所有角色的最大生命值提高10%/级", 0, 100, 10));
    shop_items.append(ShopItem("基础攻击力", "所有角色的基础攻击力提高10%/级", 1, 150, 10));
    shop_items.append(ShopItem("武器伤害", "所有武器的伤害提高10%/级", 2, 200, 10));
    shop_items.append(ShopItem("金币获取", "金币获取量提高20%/级", 3, 250, 5));
    shop_items.append(ShopItem("拾取范围", "物品拾取范围提高1格/级", 4, 300, 3));
}

QWidget* ShopUI::createItemWidget(const ShopItem& item, int index) {
    QWidget* widget = new QWidget(items_container);
    widget->setStyleSheet("background-color: rgba(60, 60, 80, 150); border-radius: 10px;");
    
    QHBoxLayout* layout = new QHBoxLayout(widget);
    layout->setContentsMargins(20, 15, 20, 15);
    
    // 左侧信息区域
    QVBoxLayout* info_layout = new QVBoxLayout();
    
    QLabel* name_label = new QLabel(item.getName(), widget);
    name_label->setStyleSheet("color: white; font-size: 18px; font-weight: bold; background: transparent;");
    
    QLabel* desc_label = new QLabel(item.getDescription(), widget);
    desc_label->setStyleSheet("color: #BBBBBB; font-size: 14px; background: transparent;");
    
    QLabel* level_label = new QLabel(widget);
    level_label->setStyleSheet("color: cyan; font-size: 14px; background: transparent;");
    level_labels.append(level_label);
    
    info_layout->addWidget(name_label);
    info_layout->addWidget(desc_label);
    info_layout->addWidget(level_label);
    
    // 右侧升级按钮
    QPushButton* upgrade_button = new QPushButton("升级", widget);
    upgrade_button->setFixedSize(100, 40);
    upgrade_button->setStyleSheet(
        "QPushButton {"
        "   background-color: #506080;"
        "   color: white;"
        "   border-radius: 5px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #607090;"
        "}"
        "QPushButton:disabled {"
        "   background-color: #404040;"
        "   color: #888888;"
        "}"
    );
    
    connect(upgrade_button, &QPushButton::clicked, this, [this, index]() {
        onUpgradeButtonClicked(index);
    });
    
    upgrade_buttons.append(upgrade_button);
    
    layout->addLayout(info_layout, 1);
    layout->addWidget(upgrade_button);
    
    return widget;
}

void ShopUI::showShop() {
    updateShopItems();
    
    // 调整窗口大小以适应父窗口
    if (parentWidget()) {
        resize(parentWidget()->size());
    }
    
    show();
    raise();
}

void ShopUI::updateShopItems() {
    if (!game_state) {
        return;
    }
    
    // 更新金币显示
    coins_label->setText(QString("金币: %1").arg(game_state->getTotalCoins()));
    
    // 更新每个项目的等级和升级按钮状态
    for (int i = 0; i < shop_items.size(); i++) {
        ShopItem& item = shop_items[i];
        int current_level = game_state->getGlobalUpgradeLevel(item.getType());
        int next_level = current_level + 1;
        
        // 更新等级显示
        if (current_level >= item.getMaxLevel()) {
            level_labels[i]->setText(QString("已达最大等级: %1").arg(current_level));
        } else {
            level_labels[i]->setText(QString("当前等级: %1 / %2").arg(current_level).arg(item.getMaxLevel()));
        }
        
        // 更新升级按钮
        QPushButton* button = upgrade_buttons[i];
        
        if (current_level >= item.getMaxLevel()) {
            button->setText("满级");
            button->setEnabled(false);
        } else {
            int cost = item.getCostForLevel(next_level);
            button->setText(QString("升级\n(%1金币)").arg(cost));
            
            // 检查金币是否足够
            button->setEnabled(game_state->getTotalCoins() >= cost);
        }
    }
}

void ShopUI::onUpgradeButtonClicked(int index) {
    if (index < 0 || index >= shop_items.size() || !game_state) {
        return;
    }
    
    ShopItem& item = shop_items[index];
    int current_level = game_state->getGlobalUpgradeLevel(item.getType());
    int next_level = current_level + 1;
    
    if (next_level > item.getMaxLevel()) {
        return; // 已达最大等级
    }
    
    int cost = item.getCostForLevel(next_level);
    
    // 检查金币是否足够
    if (game_state->getTotalCoins() < cost) {
        QMessageBox::warning(this, "金币不足", "你没有足够的金币购买这个升级！");
        return;
    }
    
    // 扣除金币并应用升级
    game_state->applyGlobalUpgrade(item.getType(), next_level);
    
    // 更新商店显示
    updateShopItems();
}

void ShopUI::onCloseButtonClicked() {
    hide();
    emit shopClosed();
}