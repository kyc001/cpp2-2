#ifndef UPGRADEUI_H
#define UPGRADEUI_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVector>

class Hero;
class UpgradeOption;

class UpgradeUI : public QWidget {
    Q_OBJECT
public:
    explicit UpgradeUI(QWidget *parent = nullptr);
    ~UpgradeUI() override;
    
    // 显示升级选项
    void showUpgradeOptions(Hero* hero);
    
private slots:
    void onOptionSelected(int index);
    
private:
    QVBoxLayout* main_layout;
    QLabel* title_label;
    QHBoxLayout* options_layout;
    
    QVector<QPushButton*> option_buttons;
    
    Hero* current_hero;
};

#endif // UPGRADEUI_H 