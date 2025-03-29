#include "../../include/ui/upgradeui.h"
#include "../../include/entities/hero.h"
#include "../../include/core/gamestate.h"
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPixmap>
#include <QPainter>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QToolTip>
#include <QCursor>
#include <QEvent>

UpgradeUI::UpgradeUI(QWidget *parent) : QWidget(parent), current_hero(nullptr), is_showing(false)
{
    // 设置无边框窗口
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);

    // 设置半透明黑色背景
    setStyleSheet("background-color: rgba(0, 0, 0, 180);");

    // 初始化UI
    setupUI();

    // 设置对象名称，便于样式表选择
    setObjectName("upgradeUI");
}

UpgradeUI::~UpgradeUI()
{
    // QWidget会自动删除子控件，不需要手动删除
}

void UpgradeUI::setupUI()
{
    // 创建主布局
    QVBoxLayout *main_layout = new QVBoxLayout(this);
    main_layout->setSpacing(20);
    main_layout->setContentsMargins(50, 50, 50, 50);

    // 创建标题
    QLabel *title_label = new QLabel("选择升级", this);
    title_label->setAlignment(Qt::AlignCenter);
    title_label->setStyleSheet("color: white; font-size: 32px; font-weight: bold;");

    // 添加阴影效果
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(title_label);
    shadow->setColor(QColor(0, 0, 0, 180));
    shadow->setOffset(2, 2);
    shadow->setBlurRadius(10);
    title_label->setGraphicsEffect(shadow);

    // 添加提示标签
    QLabel *hint_label = new QLabel("选择一项属性进行升级。鼠标悬停可查看详细信息。", this);
    hint_label->setAlignment(Qt::AlignCenter);
    hint_label->setStyleSheet("color: #CCCCCC; font-size: 16px;");

    // 创建选项布局
    QHBoxLayout *options_layout = new QHBoxLayout();
    options_layout->setSpacing(30);
    options_layout->setContentsMargins(20, 20, 20, 20);

    // 创建三个选项按钮
    for (int i = 0; i < 3; i++)
    {
        QPushButton *button = new QPushButton(this);
        button->setMinimumSize(200, 240);
        button->setCursor(Qt::PointingHandCursor);
        button->setStyleSheet(
            "QPushButton {"
            "  background-color: rgba(60, 60, 80, 220);"
            "  color: white;"
            "  border: 3px solid #6A6AAA;"
            "  border-radius: 15px;"
            "  padding: 15px;"
            "  text-align: center;"
            "  font-size: 22px;"
            "  font-weight: bold;"
            "}"
            "QPushButton:hover {"
            "  background-color: rgba(80, 80, 120, 240);"
            "  border: 3px solid #8A8ACA;"
            "  transform: scale(1.05);"
            "}"
            "QPushButton:pressed {"
            "  background-color: rgba(50, 50, 70, 240);"
            "  border: 3px solid #5A5A9A;"
            "}");

        // 连接点击信号
        connect(button, &QPushButton::clicked, this, [this, i]()
                { onUpgradeOptionClicked(i); });

        // 添加鼠标悬停事件显示详细信息
        button->installEventFilter(this);

        upgrade_buttons.append(button);
        options_layout->addWidget(button);

        // 创建一个描述标签
        QLabel *desc_label = new QLabel(this);
        desc_label->setAlignment(Qt::AlignCenter);
        desc_label->setWordWrap(true);
        desc_label->setStyleSheet("color: white; font-size: 16px; background-color: transparent;");
        upgrade_descriptions.append(desc_label);

        // 创建按钮垂直布局，包含按钮和描述
        QVBoxLayout *button_layout = new QVBoxLayout();
        button_layout->addWidget(button);
        button_layout->addWidget(desc_label);
        options_layout->addLayout(button_layout);
    }

    // 创建关闭按钮
    QPushButton *closeButton = new QPushButton("✕", this);
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
        "}");
    closeButton->setCursor(Qt::PointingHandCursor);

    // 连接关闭按钮信号
    connect(closeButton, &QPushButton::clicked, this, &UpgradeUI::hideUpgradeOptions);

    // 创建右上角布局来放置关闭按钮
    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->addStretch();
    topLayout->addWidget(closeButton);

    // 添加所有元素到主布局
    main_layout->addLayout(topLayout);
    main_layout->addWidget(title_label);
    main_layout->addWidget(hint_label);
    main_layout->addSpacing(20);
    main_layout->addLayout(options_layout);
    main_layout->addStretch();

    // 默认隐藏
    hide();

    // 设置初始大小
    resize(800, 500);
}

void UpgradeUI::showUpgradeOptions(Hero *hero)
{
    if (!hero)
    {
        return;
    }

    current_hero = hero;
    is_showing = true;

    // 获取可用的升级选项
    const std::vector<UpgradeOption *> &options = hero->getAvailableUpgrades();

    // 最多显示3个选项
    int num_options = std::min(options.size(), static_cast<size_t>(3));

    // 设置按钮文本和显示状态
    for (int i = 0; i < upgrade_buttons.size(); ++i)
    {
        if (i < num_options)
        {
            UpgradeOption *option = options[i];

            // 设置按钮文本为升级类型
            QString upgradeTypeText = getUpgradeTypeText(option->getType());
            QString valueText = QString::number(option->getValue());

            // 根据升级类型设置不同样式
            QString buttonStyle =
                "QPushButton {"
                "  background-color: %1;"
                "  color: white;"
                "  border: 3px solid %2;"
                "  border-radius: 15px;"
                "  padding: 15px;"
                "  text-align: center;"
                "  font-size: 22px;"
                "  font-weight: bold;"
                "}"
                "QPushButton:hover {"
                "  background-color: %3;"
                "  border: 3px solid %4;"
                "  transform: scale(1.05);"
                "}"
                "QPushButton:pressed {"
                "  background-color: %5;"
                "  border: 3px solid %6;"
                "}";

            QString bgColor, borderColor, hoverBgColor, hoverBorderColor, pressedBgColor, pressedBorderColor;
            QString iconPath;

            switch (option->getType())
            {
            case UpgradeType::HEALTH:
                bgColor = "rgba(180, 50, 50, 220)";
                borderColor = "#cc5555";
                hoverBgColor = "rgba(200, 70, 70, 240)";
                hoverBorderColor = "#ee6565";
                pressedBgColor = "rgba(160, 40, 40, 240)";
                pressedBorderColor = "#aa4545";
                iconPath = ":/resources/ui/icons/icon_health.png";
                break;
            case UpgradeType::ATTACK:
                bgColor = "rgba(200, 120, 30, 220)";
                borderColor = "#cc8833";
                hoverBgColor = "rgba(220, 140, 50, 240)";
                hoverBorderColor = "#ee9943";
                pressedBgColor = "rgba(180, 100, 20, 240)";
                pressedBorderColor = "#aa7723";
                iconPath = ":/resources/ui/icons/icon_attack.png";
                break;
            case UpgradeType::SPEED:
                bgColor = "rgba(50, 160, 50, 220)";
                borderColor = "#55aa55";
                hoverBgColor = "rgba(70, 180, 70, 240)";
                hoverBorderColor = "#65cc65";
                pressedBgColor = "rgba(40, 140, 40, 240)";
                pressedBorderColor = "#459945";
                iconPath = ":/resources/ui/icons/icon_speed.png";
                break;
            case UpgradeType::PICKUP_RANGE:
                bgColor = "rgba(50, 160, 160, 220)";
                borderColor = "#55aaaa";
                hoverBgColor = "rgba(70, 180, 180, 240)";
                hoverBorderColor = "#65cccc";
                pressedBgColor = "rgba(40, 140, 140, 240)";
                pressedBorderColor = "#459999";
                iconPath = ":/resources/ui/icons/icon_range.png";
                break;
            case UpgradeType::WEAPON_LEVEL:
                bgColor = "rgba(80, 80, 180, 220)";
                borderColor = "#5555cc";
                hoverBgColor = "rgba(100, 100, 200, 240)";
                hoverBorderColor = "#6565ee";
                pressedBgColor = "rgba(70, 70, 160, 240)";
                pressedBorderColor = "#4545aa";
                iconPath = ":/resources/ui/icons/icon_weapon.png";
                break;
            default:
                bgColor = "rgba(100, 100, 100, 220)";
                borderColor = "#888888";
                hoverBgColor = "rgba(120, 120, 120, 240)";
                hoverBorderColor = "#999999";
                pressedBgColor = "rgba(80, 80, 80, 240)";
                pressedBorderColor = "#777777";
                iconPath = ":/resources/ui/icons/icon_default.png";
                break;
            }

            // 设置按钮样式
            upgrade_buttons[i]->setStyleSheet(
                buttonStyle.arg(bgColor, borderColor, hoverBgColor, hoverBorderColor,
                                pressedBgColor, pressedBorderColor));

            // 设置按钮文本（包括图标和数值）
            upgrade_buttons[i]->setText(upgradeTypeText + "\n+" + valueText);

            // 设置按钮图标
            QPixmap icon(iconPath);
            if (!icon.isNull())
            {
                upgrade_buttons[i]->setIcon(icon);
                upgrade_buttons[i]->setIconSize(QSize(48, 48));
            }

            upgrade_buttons[i]->show();

            // 设置描述文本
            upgrade_descriptions[i]->setText(option->getDescription());
            upgrade_descriptions[i]->show();

            // 存储升级选项信息用于工具提示
            upgrade_buttons[i]->setProperty("upgrade_description", option->getDescription());
        }
        else
        {
            upgrade_buttons[i]->hide();
            upgrade_descriptions[i]->hide();
        }
    }

    // 居中显示UI
    centerUI();

    // 显示升级选择界面，带有淡入效果
    setWindowOpacity(0.0);
    show();

    QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
    animation->setDuration(300);
    animation->setStartValue(0.0);
    animation->setEndValue(1.0);
    animation->start(QPropertyAnimation::DeleteWhenStopped);
}

QString UpgradeUI::getUpgradeTypeText(UpgradeType type)
{
    switch (type)
    {
    case UpgradeType::HEALTH:
        return "生命值";
    case UpgradeType::ATTACK:
        return "攻击力";
    case UpgradeType::SPEED:
        return "移动速度";
    case UpgradeType::PICKUP_RANGE:
        return "拾取范围";
    case UpgradeType::WEAPON_LEVEL:
        return "武器等级";
    default:
        return "未知升级";
    }
}

void UpgradeUI::hideUpgradeOptions()
{
    // 淡出效果
    QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
    animation->setDuration(200);
    animation->setStartValue(1.0);
    animation->setEndValue(0.0);

    connect(animation, &QPropertyAnimation::finished, this, [this]()
            {
        is_showing = false;
        hide(); });

    animation->start(QPropertyAnimation::DeleteWhenStopped);
}

void UpgradeUI::onUpgradeOptionClicked(int index)
{
    if (current_hero)
    {
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
    if (parentWidget())
    {
        // QRect parent_rect = parentWidget()->rect();
        move(parentWidget()->width() / 2 - width() / 2,
             parentWidget()->height() / 2 - height() / 2);
    }
}

bool UpgradeUI::eventFilter(QObject *watched, QEvent *event)
{
    // 处理按钮的鼠标悬停事件，显示详细提示
    for (int i = 0; i < upgrade_buttons.size(); ++i)
    {
        if (watched == upgrade_buttons[i])
        {
            if (event->type() == QEvent::Enter)
            {
                QString desc = upgrade_buttons[i]->property("upgrade_description").toString();
                if (!desc.isEmpty())
                {
                    QToolTip::showText(QCursor::pos(), desc, upgrade_buttons[i]);
                }
            }
            else if (event->type() == QEvent::Leave)
            {
                QToolTip::hideText();
            }
        }
    }

    return QWidget::eventFilter(watched, event);
}