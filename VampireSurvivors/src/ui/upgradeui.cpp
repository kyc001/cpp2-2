#include "../../include/ui/upgradeui.h"
#include "../../include/entities/hero.h"
#include "../../include/core/gamestate.h"
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>

UpgradeUI::UpgradeUI(QWidget *parent) : QWidget(parent), current_hero(nullptr), is_showing(false) {
    // 设置半透明黑色背景
    setStyleSheet("background-color: rgba(0, 0, 0, 180);");
    
    setupUI();
}

UpgradeUI::~UpgradeUI() {
    // QWidget会自动删除子控件，不需要手动删除
}

void UpgradeUI::setupUI() {
    // 创建主布局
    QVBoxLayout *main_layout = new QVBoxLayout(this);
    
    // 创建标题
    QLabel *title_label = new QLabel("升级选择", this);
    title_label->setAlignment(Qt::AlignCenter);
    title_label->setStyleSheet("color: white; font-size: 24px; font-weight: bold;");
    
    // 创建选项布局
    QHBoxLayout *options_layout = new QHBoxLayout();
    
    // 创建三个选项按钮
    for (int i = 0; i < 3; i++) {
        QPushButton* button = new QPushButton(this);
        button->setMinimumSize(150, 200);
        button->setStyleSheet(
            "QPushButton {"
            "  background-color: rgba(60, 60, 80, 180);"
            "  color: white;"
            "  border: 2px solid #5A5A8A;"
            "  border-radius: 10px;"
            "  padding: 10px;"
            "  text-align: center;"
            "}"
            "QPushButton:hover {"
            "  background-color: rgba(80, 80, 100, 200);"
            "  border: 2px solid #7A7AAA;"
            "}"
        );
        
        // 连接点击信号
        connect(button, &QPushButton::clicked, this, [this, i]() {
            onUpgradeOptionClicked(i);
        });
        
        upgrade_buttons.append(button);
        options_layout->addWidget(button);
        
        // 创建一个描述标签，但现在不显示它
        QLabel* desc_label = new QLabel(this);
        desc_label->setVisible(false);
        upgrade_descriptions.append(desc_label);
    }
    
    // 添加标题和选项到主布局
    main_layout->addStretch();
    main_layout->addWidget(title_label);
    main_layout->addSpacing(20);
    main_layout->addLayout(options_layout);
    main_layout->addStretch();
    
    // 默认隐藏
    hide();
}

void UpgradeUI::showUpgradeOptions(Hero* hero) {
    if (!hero) {
        return;
    }
    
    current_hero = hero;
    
    // 获取可用升级选项
    const std::vector<UpgradeOption*>& upgrades = hero->getAvailableUpgrades();
    
    // 如果没有可用选项则不显示
    if (upgrades.empty()) {
        hide();
        return;
    }
    
    // 更新按钮内容
    for (int i = 0; i < qMin(static_cast<int>(upgrades.size()), upgrade_buttons.size()); i++) {
        QPushButton* button = upgrade_buttons[i];
        UpgradeOption* upgrade = upgrades[i];
        
        // 设置按钮文本显示升级信息
        QString buttonText = QString("<h3>%1</h3><p>%2</p>")
                                .arg(upgrade->getName())
                                .arg(upgrade->getDescription());
        
        // 根据升级类型设置不同颜色
        if (upgrade->getType() == UpgradeType::CHARACTER) {
            buttonText = QString("<div style='color:#8AE234;'>%1</div>").arg(buttonText);
        } else { // WEAPON
            buttonText = QString("<div style='color:#729FCF;'>%1</div>").arg(buttonText);
        }
        
        button->setText(buttonText);
    }
    
    // 调整窗口大小以适应父窗口
    if (parentWidget()) {
        resize(parentWidget()->size());
    }
    
    is_showing = true;
    
    // 显示升级界面
    show();
    raise(); // 确保显示在最前面
}

void UpgradeUI::hideUpgradeOptions() {
    is_showing = false;
    hide();
}

void UpgradeUI::onUpgradeOptionClicked(int index) {
    if (current_hero) {
        // 应用选择的升级
        current_hero->applyUpgrade(index);
        
        // 隐藏升级界面
        hideUpgradeOptions();
        
        // 发送升级完成信号
        emit upgradeSelected();
    }
}

void UpgradeUI::centerUI()
{
    if (parentWidget()) {
        QRect parent_rect = parentWidget()->rect();
        move(parentWidget()->mapToGlobal(QPoint(
            parent_rect.width()/2 - width()/2,
            parent_rect.height()/2 - height()/2
        )));
    }
} 