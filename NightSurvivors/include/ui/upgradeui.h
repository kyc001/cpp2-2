#ifndef UPGRADEUI_H
#define UPGRADEUI_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVector>
#include "../entities/hero.h" // 包含 Hero 类以获取 UpgradeType 定义

class Hero;

// 升级选择界面
class UpgradeUI : public QWidget {
    Q_OBJECT
    
public:
    explicit UpgradeUI(QWidget *parent = nullptr);
    ~UpgradeUI() override;
    
    // 显示升级选项
    void showUpgradeOptions(Hero *hero);
    void hideUpgradeOptions();
    void centerUI();
    
signals:
    // 升级完成的信号
    void upgradeSelected();
    
private slots:
    // 点击升级选项
    void onUpgradeOptionClicked(int index);
    
protected:
    // 事件过滤器，用于处理按钮的悬停提示
    bool eventFilter(QObject *watched, QEvent *event) override;
    
private:
    void setupUI();
    QString getUpgradeTypeText(UpgradeType type);
    
    QVector<QPushButton*> upgrade_buttons;
    QVector<QLabel*> upgrade_descriptions;
    
    Hero *current_hero;
    bool is_showing;
};

#endif // UPGRADEUI_H 