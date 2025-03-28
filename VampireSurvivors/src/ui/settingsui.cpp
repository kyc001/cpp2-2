#include "../../include/ui/settingsui.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QSettings>

SettingsUI::SettingsUI(QWidget *parent) : QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(400, 400);
    hide();
    
    setupUI();
    loadSettings();
}

SettingsUI::~SettingsUI()
{
}

void SettingsUI::setupUI()
{
    QVBoxLayout *main_layout = new QVBoxLayout(this);
    
    // 创建标题
    QLabel *title_label = new QLabel("设置", this);
    title_label->setAlignment(Qt::AlignCenter);
    title_label->setStyleSheet("color: white; font-size: 24px; font-weight: bold;");
    
    // 创建音量设置组
    QGroupBox *volume_group = new QGroupBox("音量设置", this);
    volume_group->setStyleSheet("color: white; font-size: 16px;");
    
    QHBoxLayout *volume_layout = new QHBoxLayout();
    QLabel *volume_text = new QLabel("音量: ", this);
    volume_text->setStyleSheet("color: white;");
    
    volume_slider = new QSlider(Qt::Horizontal, this);
    volume_slider->setRange(0, 100);
    volume_slider->setValue(100);
    
    volume_label = new QLabel("100", this);
    volume_label->setStyleSheet("color: white;");
    
    volume_layout->addWidget(volume_text);
    volume_layout->addWidget(volume_slider);
    volume_layout->addWidget(volume_label);
    
    volume_group->setLayout(volume_layout);
    
    // 创建界面大小设置组
    QGroupBox *size_group = new QGroupBox("界面大小", this);
    size_group->setStyleSheet("color: white; font-size: 16px;");
    
    QHBoxLayout *size_layout = new QHBoxLayout();
    QLabel *size_text = new QLabel("窗口大小: ", this);
    size_text->setStyleSheet("color: white;");
    
    screen_size_combo = new QComboBox(this);
    screen_size_combo->addItem("800 x 600");
    screen_size_combo->addItem("1024 x 768");
    screen_size_combo->addItem("1280 x 720");
    screen_size_combo->addItem("1600 x 900");
    screen_size_combo->addItem("1920 x 1080");
    
    size_layout->addWidget(size_text);
    size_layout->addWidget(screen_size_combo);
    
    size_group->setLayout(size_layout);
    
    // 创建控制类型设置组
    QGroupBox *control_group = new QGroupBox("控制方式", this);
    control_group->setStyleSheet("color: white; font-size: 16px;");
    
    QVBoxLayout *control_layout = new QVBoxLayout();
    
    control_type_group = new QButtonGroup(this);
    wasd_radio = new QRadioButton("键盘控制 (WASD)", this);
    wasd_radio->setStyleSheet("color: white;");
    mouse_radio = new QRadioButton("鼠标控制", this);
    mouse_radio->setStyleSheet("color: white;");
    
    control_type_group->addButton(wasd_radio, 0);
    control_type_group->addButton(mouse_radio, 1);
    wasd_radio->setChecked(true);
    
    control_layout->addWidget(wasd_radio);
    control_layout->addWidget(mouse_radio);
    
    control_group->setLayout(control_layout);
    
    // 创建按钮
    QHBoxLayout *button_layout = new QHBoxLayout();
    
    save_button = new QPushButton("保存", this);
    save_button->setMinimumSize(100, 30);
    save_button->setStyleSheet("background-color: #4a90e2; color: white; border: none; border-radius: 5px;");
    
    close_button = new QPushButton("关闭", this);
    close_button->setMinimumSize(100, 30);
    close_button->setStyleSheet("background-color: #e24a4a; color: white; border: none; border-radius: 5px;");
    
    button_layout->addWidget(save_button);
    button_layout->addWidget(close_button);
    
    // 添加所有组件到主布局
    main_layout->addWidget(title_label);
    main_layout->addWidget(volume_group);
    main_layout->addWidget(size_group);
    main_layout->addWidget(control_group);
    main_layout->addStretch(1);
    main_layout->addLayout(button_layout);
    
    setLayout(main_layout);
    
    // 设置样式
    this->setStyleSheet("QWidget { background-color: rgba(40, 40, 40, 220); border-radius: 10px; padding: 10px; }");
    
    // 连接信号
    connect(volume_slider, &QSlider::valueChanged, this, &SettingsUI::onVolumeSliderChanged);
    connect(screen_size_combo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SettingsUI::onScreenSizeComboChanged);
    connect(control_type_group, QOverload<int>::of(&QButtonGroup::idClicked), this, &SettingsUI::onControlTypeChanged);
    connect(close_button, &QPushButton::clicked, this, &SettingsUI::onCloseButtonClicked);
    connect(save_button, &QPushButton::clicked, this, &SettingsUI::onSaveButtonClicked);
}

void SettingsUI::show()
{
    // 更新UI中的设置值
    loadSettings();
    QWidget::show();
}

void SettingsUI::hide()
{
    QWidget::hide();
}

bool SettingsUI::isVisible() const
{
    return QWidget::isVisible();
}

void SettingsUI::onCloseButtonClicked()
{
    hide();
    emit settingsUIClosed();
}

void SettingsUI::onVolumeSliderChanged(int value)
{
    volume_label->setText(QString::number(value));
    emit volumeChanged(value);
}

void SettingsUI::onScreenSizeComboChanged(int index)
{
    emit screenSizeChanged(index);
}

void SettingsUI::onControlTypeChanged(int id)
{
    emit controlTypeChanged(id);
}

void SettingsUI::onSaveButtonClicked()
{
    saveSettings();
    hide();
    emit settingsUIClosed();
}

void SettingsUI::saveSettings()
{
    QSettings settings("VampireSurvivors", "Settings");
    settings.setValue("volume", volume_slider->value());
    settings.setValue("screenSize", screen_size_combo->currentIndex());
    settings.setValue("controlType", control_type_group->checkedId());
}

void SettingsUI::loadSettings()
{
    QSettings settings("VampireSurvivors", "Settings");
    
    // 加载音量设置
    int volume = settings.value("volume", 100).toInt();
    volume_slider->setValue(volume);
    volume_label->setText(QString::number(volume));
    
    // 加载窗口大小设置
    int screenSize = settings.value("screenSize", 0).toInt();
    screen_size_combo->setCurrentIndex(screenSize);
    
    // 加载控制类型设置
    int controlType = settings.value("controlType", 0).toInt();
    if (controlType == 0) {
        wasd_radio->setChecked(true);
    } else {
        mouse_radio->setChecked(true);
    }
}