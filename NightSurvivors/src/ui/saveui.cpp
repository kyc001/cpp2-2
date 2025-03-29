#include "../../include/ui/saveui.h"
#include "../../include/core/gamestate.h"
#include "../../include/utils/savefile.h"
#include <QPainter>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QScrollArea>
#include <QDateTime>
#include <QStyle>
#include <QStyleOption>

SaveUI::SaveUI(QWidget *parent) : QWidget(parent), game_state(nullptr), is_save_mode(false)
{
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAutoFillBackground(false);

    // 加载资源
    setupUI();

    // 默认隐藏
    hide();
}

SaveUI::~SaveUI()
{
}

void SaveUI::setGameState(GameState *state)
{
    game_state = state;
}

void SaveUI::setupUI()
{
    // 创建像素风背景
    const int width = 600;
    const int height = 450;

    // 居中显示
    int x = (parentWidget()->width() - width) / 2;
    int y = (parentWidget()->height() - height) / 2;
    setGeometry(x, y, width, height);

    // 创建标题
    title_label = new QLabel(this);
    title_label->setGeometry(0, 20, width, 40);
    title_label->setAlignment(Qt::AlignCenter);
    title_label->setStyleSheet("color: white; font-size: 24px; font-weight: bold;");

    // 创建存档容器
    scroll_area = new QScrollArea(this);
    scroll_area->setGeometry(50, 80, width - 100, height - 150);
    scroll_area->setStyleSheet("background-color: rgba(30, 30, 50, 200); border: 2px solid #555;");
    scroll_area->setWidgetResizable(true);

    // 存档列表容器
    save_list_widget = new QWidget();
    save_list_layout = new QVBoxLayout(save_list_widget);
    save_list_layout->setSpacing(10);
    save_list_layout->setContentsMargins(10, 10, 10, 10);

    scroll_area->setWidget(save_list_widget);

    // 按钮布局
    QWidget *button_container = new QWidget(this);
    button_container->setGeometry(50, height - 60, width - 100, 50);

    QHBoxLayout *button_layout = new QHBoxLayout(button_container);
    button_layout->setSpacing(20);
    button_layout->setContentsMargins(0, 0, 0, 0);

    // 创建按钮
    QPushButton *new_save_button = new QPushButton("新存档", button_container);
    QPushButton *delete_button = new QPushButton("删除", button_container);
    QPushButton *back_button = new QPushButton("返回", button_container);

    // 按钮样式
    QString button_style = "QPushButton { background-color: #445; color: white; border: 2px solid #667; "
                           "border-radius: 5px; padding: 5px; min-width: 80px; font-size: 14px; }"
                           "QPushButton:hover { background-color: #556; border: 2px solid #778; }"
                           "QPushButton:pressed { background-color: #334; }";

    new_save_button->setStyleSheet(button_style);
    delete_button->setStyleSheet(button_style);
    back_button->setStyleSheet(button_style);

    // 添加按钮到布局
    button_layout->addWidget(new_save_button);
    button_layout->addWidget(delete_button);
    button_layout->addWidget(back_button);

    // 连接信号
    connect(new_save_button, &QPushButton::clicked, this, &SaveUI::onNewSaveClicked);
    connect(delete_button, &QPushButton::clicked, this, &SaveUI::onDeleteClicked);
    connect(back_button, &QPushButton::clicked, this, &SaveUI::onBackClicked);

    // 初始化选中的存档槽
    selected_slot = -1;
}

void SaveUI::showSaveMode()
{
    is_save_mode = true;
    title_label->setText("保存游戏");

    // 更新存档列表
    refreshSaveList();

    // 显示界面
    show();
    raise();
}

void SaveUI::showLoadMode()
{
    is_save_mode = false;
    title_label->setText("读取游戏");

    // 更新存档列表
    refreshSaveList();

    // 显示界面
    show();
    raise();
}

void SaveUI::refreshSaveList()
{
    // 首先清空现有列表
    QLayoutItem *item;
    while ((item = save_list_layout->takeAt(0)) != nullptr)
    {
        if (QWidget *widget = item->widget())
        {
            widget->setParent(nullptr);
            delete widget;
        }
        delete item;
    }

    save_widgets.clear();

    // 获取存档列表
    QStringList save_slots;
    if (game_state && game_state->getSaveFile())
    {
        save_slots = game_state->getSaveFile()->getSaveSlots();
    }

    // 对存档槽进行排序
    QList<int> slot_numbers;
    for (const QString &slot_str : save_slots)
    {
        slot_numbers.append(slot_str.toInt());
    }
    std::sort(slot_numbers.begin(), slot_numbers.end());

    // 创建空存档位
    for (int i = 0; i < 10; i++)
    {
        if (!slot_numbers.contains(i))
        {
            slot_numbers.append(i);
        }
    }

    std::sort(slot_numbers.begin(), slot_numbers.end());

    // 为每个存档创建一个条目
    for (int slot : slot_numbers)
    {
        QWidget *save_item = createSaveItem(slot);
        save_list_layout->addWidget(save_item);
        save_widgets.append(save_item);
    }
}

QWidget *SaveUI::createSaveItem(int slot)
{
    QWidget *save_item = new QWidget();
    save_item->setObjectName(QString("save_slot_%1").arg(slot));
    save_item->setProperty("slot", slot);
    save_item->setMinimumHeight(70);
    save_item->setMaximumHeight(70);

    // 像素风格样式
    QString base_style = "background-color: rgba(50, 50, 70, 180); border: 2px solid #555;";
    QString selected_style = "background-color: rgba(70, 70, 100, 220); border: 2px solid #99a;";

    save_item->setStyleSheet(base_style);

    // 布局
    QHBoxLayout *layout = new QHBoxLayout(save_item);
    layout->setContentsMargins(10, 5, 10, 5);
    layout->setSpacing(10);

    // 存档槽标签
    QLabel *slot_label = new QLabel(QString("存档 #%1").arg(slot + 1));
    slot_label->setMinimumWidth(70);
    slot_label->setStyleSheet("color: white; font-weight: bold; border: none;");

    // 右侧信息布局
    QVBoxLayout *info_layout = new QVBoxLayout();
    info_layout->setSpacing(5);

    // 存档元数据
    QString metadata;
    bool has_save = false;

    if (game_state && game_state->getSaveFile())
    {
        metadata = game_state->getSaveFile()->getSaveMetadata(slot);
        has_save = !metadata.isEmpty();
    }

    QLabel *info_label = new QLabel(has_save ? metadata : "-- 空存档槽 --");
    info_label->setStyleSheet("color: " + QString(has_save ? "#bef" : "#777") + "; border: none;");

    // 时间标签
    QLabel *time_label = new QLabel(has_save ? QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm") : "");
    time_label->setStyleSheet("color: #999; font-size: 12px; border: none;");

    // 添加到信息布局
    info_layout->addWidget(info_label);
    info_layout->addWidget(time_label);

    // 添加到主布局
    layout->addWidget(slot_label);
    layout->addLayout(info_layout, 1);

    // 添加事件过滤器以处理点击
    save_item->installEventFilter(this);

    return save_item;
}

bool SaveUI::eventFilter(QObject *obj, QEvent *event)
{
    // 检查是否点击了存档项
    if (event->type() == QEvent::MouseButtonPress)
    {
        for (QWidget *widget : save_widgets)
        {
            if (obj == widget)
            {
                selectSaveSlot(widget->property("slot").toInt());
                return true;
            }
        }
    }

    return QWidget::eventFilter(obj, event);
}

void SaveUI::selectSaveSlot(int slot)
{
    // 更新所选存档槽
    selected_slot = slot;

    // 更新UI
    for (QWidget *widget : save_widgets)
    {
        int widget_slot = widget->property("slot").toInt();

        QString style_base = "background-color: rgba(50, 50, 70, 180); border: 2px solid #555;";
        QString style_selected = "background-color: rgba(70, 70, 100, 220); border: 2px solid #99a;";

        widget->setStyleSheet(widget_slot == selected_slot ? style_selected : style_base);
    }

    // 触发信号
    emit saveSlotSelected(slot);
}

void SaveUI::paintEvent(QPaintEvent *event)
{
    // 像素风格的透明背景
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 半透明背景
    QColor bg_color(30, 30, 50, 230);
    painter.fillRect(rect(), bg_color);

    // 像素风格边框
    QPen pen(QColor(100, 100, 140), 3);
    painter.setPen(pen);
    painter.drawRect(rect().adjusted(5, 5, -5, -5));

    // 像素风格装饰
    int pixel_size = 5;

    // 左上角装饰
    painter.fillRect(0, 0, pixel_size * 3, pixel_size, QColor(130, 130, 180));
    painter.fillRect(0, 0, pixel_size, pixel_size * 3, QColor(130, 130, 180));

    // 右上角装饰
    painter.fillRect(width() - pixel_size * 3, 0, pixel_size * 3, pixel_size, QColor(130, 130, 180));
    painter.fillRect(width() - pixel_size, 0, pixel_size, pixel_size * 3, QColor(130, 130, 180));

    // 左下角装饰
    painter.fillRect(0, height() - pixel_size, pixel_size * 3, pixel_size, QColor(130, 130, 180));
    painter.fillRect(0, height() - pixel_size * 3, pixel_size, pixel_size * 3, QColor(130, 130, 180));

    // 右下角装饰
    painter.fillRect(width() - pixel_size * 3, height() - pixel_size, pixel_size * 3, pixel_size, QColor(130, 130, 180));
    painter.fillRect(width() - pixel_size, height() - pixel_size * 3, pixel_size, pixel_size * 3, QColor(130, 130, 180));

    QWidget::paintEvent(event);
}

void SaveUI::onNewSaveClicked()
{
    if (!game_state)
        return;

    if (selected_slot >= 0)
    {
        if (is_save_mode)
        {
            // 保存游戏
            bool success = game_state->saveGameToSlot(selected_slot);

            if (success)
            {
                QMessageBox::information(this, "保存成功", "游戏已成功保存到存档槽 " + QString::number(selected_slot + 1));
                refreshSaveList();
            }
            else
            {
                QMessageBox::warning(this, "保存失败", "无法保存游戏到存档槽 " + QString::number(selected_slot + 1));
            }
        }
    }
    else
    {
        QMessageBox::information(this, "提示", "请先选择一个存档槽!");
    }
}

void SaveUI::onDeleteClicked()
{
    if (!game_state)
        return;

    if (selected_slot >= 0)
    {
        // 确认是否删除
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "确认删除",
                                      "确定要删除存档槽 " + QString::number(selected_slot + 1) + " 吗?",
                                      QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes)
        {
            // 删除存档
            bool success = game_state->getSaveFile()->deleteSaveSlot(selected_slot);

            if (success)
            {
                QMessageBox::information(this, "删除成功", "存档槽 " + QString::number(selected_slot + 1) + " 已删除");
                selected_slot = -1;
                refreshSaveList();
            }
            else
            {
                QMessageBox::warning(this, "删除失败", "无法删除存档槽 " + QString::number(selected_slot + 1));
            }
        }
    }
    else
    {
        QMessageBox::information(this, "提示", "请先选择一个存档槽!");
    }
}

void SaveUI::onLoadClicked()
{
    if (!game_state)
        return;

    if (selected_slot >= 0)
    {
        if (!is_save_mode)
        {
            // 加载游戏
            bool success = game_state->loadGameFromSlot(selected_slot);

            if (success)
            {
                QMessageBox::information(this, "加载成功", "游戏已成功从存档槽 " + QString::number(selected_slot + 1) + " 加载");
                hide();
                emit loadGameRequested();
            }
            else
            {
                QMessageBox::warning(this, "加载失败", "无法从存档槽 " + QString::number(selected_slot + 1) + " 加载游戏");
            }
        }
    }
    else
    {
        QMessageBox::information(this, "提示", "请先选择一个存档槽!");
    }
}

void SaveUI::onBackClicked()
{
    // 隐藏界面
    hide();

    // 触发返回信号
    emit backRequested();
}

// 添加方法实现
void SaveUI::centerUI()
{
    // 如果这是一个 QWidget，可以这样居中
    if (parentWidget())
    {
        move((parentWidget()->width() - width()) / 2,
             (parentWidget()->height() - height()) / 2);
    }
}