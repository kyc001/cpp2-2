#ifndef SHOPUI_H
#define SHOPUI_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QVector>

class GameState;
class MainScene;  // 添加MainScene的前向声明

// 商店项目类（全局强化选项）
class ShopItem {
public:
    ShopItem(QString name, QString description, int type, int base_cost, int max_level)
        : name(name), description(description), type(type), 
          base_cost(base_cost), max_level(max_level) {}
    
    QString getName() const { return name; }
    QString getDescription() const { return description; }
    int getType() const { return type; }
    int getBaseCost() const { return base_cost; }
    int getMaxLevel() const { return max_level; }
    
    // 计算升级到某一级别所需的金币
    int getCostForLevel(int level) const;
    
private:
    QString name;
    QString description;
    int type;         // 全局强化类型
    int base_cost;    // 基础花费
    int max_level;    // 最大等级
};

// 商店界面类
class ShopUI : public QWidget {
    Q_OBJECT
public:
    explicit ShopUI(GameState* state, QWidget *parent = nullptr);
    explicit ShopUI(MainScene* scene, QWidget *parent = nullptr);
    virtual ~ShopUI() override;
    
    // 显示商店
    void show();
    
    // 更新商店内容
    void updateShopItems();

    // 居中显示
    void centerUI();
    
signals:
    void shopClosed();
    
private slots:
    void onUpgradeButtonClicked(int type);
    void onCloseButtonClicked();
    
protected:
    // 事件过滤器，用于处理界面元素的悬停提示
    bool eventFilter(QObject *watched, QEvent *event) override;
    
private:
    void setupUI();
    GameState* game_state;
    
    QVBoxLayout* main_layout;
    QLabel* title_label;
    QLabel* coins_label;
    QScrollArea* items_scroll_area;
    QWidget* items_container;
    QVBoxLayout* items_layout;
    
    QVector<ShopItem> shop_items;
    QVector<QLabel*> level_labels;
    QVector<QPushButton*> upgrade_buttons;
    
    // 初始化商店项目
    void initShopItems();
    
    // 创建商店项目的UI
    QWidget* createItemWidget(const ShopItem& item, int index);
};

#endif // SHOPUI_H