#include "../../include/ui/saveui.h"
#include "../../include/core/gamestate.h"
#include "../../include/utils/savefile.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QDateTime>
#include <QSettings>
#include <QMessageBox>

SaveUI::SaveUI(QWidget *parent) : QWidget(parent), game_state(nullptr), is_showing(false)
{
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(400, 500);
    hide();
    
    setupUI();
}

SaveUI::~SaveUI()
{
}

void SaveUI::setupUI()
{
    QVBoxLayout *main_layout = new QVBoxLayout(this);
    
    // 创建标题
    QLabel *title_label = new QLabel("游戏存档", this);
    title_label->setAlignment(Qt::AlignCenter);
    title_label->setStyleSheet("color: white; font-size: 24px; font-weight: bold;");
    
    // 创建存档槽位网格
    QGridLayout *slots_layout = new QGridLayout();
    slots_layout->setSpacing(15);
    
    // 创建5个存档槽
    for (int i = 0; i < 5; i++) {
        // 创建存档按钮
        QPushButton *slot_button = new QPushButton(QString("存档 %1").arg(i + 1), this);
        slot_button->setMinimumSize(350, 60);
        slot_button->setStyleSheet("background-color: #4a90e2; color: white; border: none; border-radius: 5px; text-align: left; padding-left: 10px; font-size: 16px;");
        connect(slot_button, &QPushButton::clicked, this, [this, i]() { onSaveSlotClicked(i); });
        
        // 创建存档信息标签
        QLabel *info_label = new QLabel("空存档", this);
        info_label->setStyleSheet("color: #aaaaaa; font-size: 14px; padding-left: 10px;");
        
        // 添加到网格
        slots_layout->addWidget(slot_button, i * 2, 0);
        slots_layout->addWidget(info_label, i * 2 + 1, 0);
        
        save_slots.append(slot_button);
        slot_info_labels.append(info_label);
    }
    
    // 创建关闭按钮
    close_button = new QPushButton("关闭", this);
    close_button->setMinimumSize(120, 40);
    close_button->setStyleSheet("background-color: #e24a4a; color: white; border: none; border-radius: 5px; font-size: 16px;");
    connect(close_button, &QPushButton::clicked, this, &SaveUI::onCloseButtonClicked);
    
    // 添加到主布局
    main_layout->addWidget(title_label);
    main_layout->addLayout(slots_layout);
    main_layout->addWidget(close_button, 0, Qt::AlignCenter);
    main_layout->addStretch();
    
    setLayout(main_layout);
    
    // 设置样式
    this->setStyleSheet("QWidget { background-color: rgba(40, 40, 40, 220); border-radius: 10px; padding: 20px; }");
}

void SaveUI::showSaveUI(GameState *state)
{
    game_state = state;
    updateSlotInfo();
    centerUI();
    
    is_showing = true;
    show();
}

void SaveUI::hideSaveUI()
{
    is_showing = false;
    hide();
    emit saveUIClosed();
}

void SaveUI::updateSlotInfo()
{
    // 从配置文件加载存档信息
    QSettings settings("VampireSurvivors", "Saves");
    
    for (int i = 0; i < 5; i++) {
        QString save_key = QString("save_%1").arg(i);
        if (settings.contains(save_key + "/timestamp")) {
            // 读取存档信息
            QDateTime timestamp = settings.value(save_key + "/timestamp").toDateTime();
            int score = settings.value(save_key + "/score", 0).toInt();
            int level = settings.value(save_key + "/level", 1).toInt();
            QString character = settings.value(save_key + "/character", "未知").toString();
            
            // 更新UI
            QString info = QString("角色: %1 | 等级: %2 | 分数: %3 | %4")
                            .arg(character)
                            .arg(level)
                            .arg(score)
                            .arg(timestamp.toString("yyyy-MM-dd hh:mm"));
            
            slot_info_labels[i]->setText(info);
            save_slots[i]->setStyleSheet("background-color: #4a90e2; color: white; border: none; border-radius: 5px; text-align: left; padding-left: 10px; font-size: 16px;");
        } else {
            // 空存档
            slot_info_labels[i]->setText("空存档");
            save_slots[i]->setStyleSheet("background-color: #5a6268; color: white; border: none; border-radius: 5px; text-align: left; padding-left: 10px; font-size: 16px;");
        }
    }
}

void SaveUI::centerUI()
{
    if (parentWidget()) {
        QRect parent_rect = parentWidget()->geometry();
        move(parent_rect.center() - rect().center());
    }
}

void SaveUI::onSaveSlotClicked(int slot)
{
    if (!game_state) return;
    
    // 确认存档
    QMessageBox msgBox;
    msgBox.setWindowTitle("确认存档");
    
    // 检查是否覆盖存档
    QSettings settings("VampireSurvivors", "Saves");
    QString save_key = QString("save_%1").arg(slot);
    
    if (settings.contains(save_key + "/timestamp")) {
        msgBox.setText(QString("确定要覆盖存档 %1 吗?").arg(slot + 1));
    } else {
        msgBox.setText(QString("确定要保存到存档 %1 吗?").arg(slot + 1));
    }
    
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    
    if (msgBox.exec() == QMessageBox::Yes) {
        // 保存游戏状态
        game_state->saveGameToSlot(slot);
        
        // 更新显示
        updateSlotInfo();
        
        QMessageBox::information(this, "保存成功", QString("游戏已成功保存到存档 %1").arg(slot + 1));
    }
}

void SaveUI::onCloseButtonClicked()
{
    hideSaveUI();
}

void SaveUI::showMessage(const QString& title, const QString& message) {
    QMessageBox msgBox;
    msgBox.setWindowTitle(title);
    msgBox.setText(message);
    msgBox.exec();
} 
