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

void UpgradeUI::showUpgradeOptions(Hero *hero) {
    if (!hero) {
        return;
    }
    
    current_hero = hero;
    is_showing = true;
    
    // 获取可用的升级选项
    const std::vector<UpgradeOption*>& options = hero->getAvailableUpgrades();
    
    // 最多显示3个选项
    int num_options = std::min(options.size(), static_cast<size_t>(3));
    
    // 设置按钮文本和显示状态
    for (int i = 0; i < upgrade_buttons.size(); ++i) {
        if (i < num_options) {
            UpgradeOption* option = options[i];
            
            // 设置按钮文本为升级类型
            QString upgradeTypeText = getUpgradeTypeText(option->getType());
            
            // 根据升级类型设置不同颜色
            QString buttonStyle;
            
            switch (option->getType()) {
                case UpgradeType::HEALTH:
                    buttonStyle = "background-color: #ff5555; color: white; font-weight: bold;";
                    break;
                case UpgradeType::ATTACK:
                    buttonStyle = "background-color: #ff9955; color: white; font-weight: bold;";
                    break;
                case UpgradeType::SPEED:
                    buttonStyle = "background-color: #55ff55; color: white; font-weight: bold;";
                    break;
                case UpgradeType::PICKUP_RANGE:
                    buttonStyle = "background-color: #55ffff; color: white; font-weight: bold;";
                    break;
                case UpgradeType::WEAPON_LEVEL:
                    buttonStyle = "background-color: #5555ff; color: white; font-weight: bold;";
                    break;
                default:
                    buttonStyle = "background-color: #aaaaaa; color: white; font-weight: bold;";
                    break;
            }
            
            upgrade_buttons[i]->setText(upgradeTypeText);
            upgrade_buttons[i]->setStyleSheet(buttonStyle);
            upgrade_buttons[i]->show();
            
            // 设置描述文本
            upgrade_descriptions[i]->setText(option->getDescription());
            upgrade_descriptions[i]->show();
        } else {
            upgrade_buttons[i]->hide();
            upgrade_descriptions[i]->hide();
        }
    }
    
    // 居中显示UI
    centerUI();
    
    // 显示升级选择界面
    show();
}

QString UpgradeUI::getUpgradeTypeText(UpgradeType type) {
    switch (type) {
        case UpgradeType::HEALTH:
            return "生命值 +";
        case UpgradeType::ATTACK:
            return "攻击力 +";
        case UpgradeType::SPEED:
            return "移动速度 +";
        case UpgradeType::PICKUP_RANGE:
            return "拾取范围 +";
        case UpgradeType::WEAPON_LEVEL:
            return "武器等级 +";
        default:
            return "未知升级";
    }
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