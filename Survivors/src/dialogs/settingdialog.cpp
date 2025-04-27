//
// 游戏设置对话框实现文件
//

#include "settingdialog.h"
#include <QDebug>
#include <QSlider>
#include <QButtonGroup>
#include <QRadioButton>

SettingDialog::SettingDialog(QWidget *parent) : QDialog(parent)
{
    // 设置窗口标题
    setWindowTitle("游戏设置");

    // 初始化临时变量为当前全局设置
    tempVolume = GAME_VOLUME;
    tempControlMode = CONTROL_MODE;

    // 设置UI
    setupUI();

    // 连接信号和槽
    connect(volumeSlider, &QSlider::valueChanged, this, &SettingDialog::onVolumeChanged);
    connect(controlModeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &SettingDialog::onControlModeChanged);
    connect(applyButton, &QPushButton::clicked, this, &SettingDialog::onApplyClicked);
    connect(cancelButton, &QPushButton::clicked, this, &SettingDialog::onCancelClicked);
}

SettingDialog::~SettingDialog()
{
}

void SettingDialog::setupUI()
{
    // 主布局
    auto mainLayout = new QVBoxLayout(this);
    
    // 音量设置组
    auto volumeGroup = new QGroupBox("音量设置");
    auto volumeLayout = new QHBoxLayout;
    volumeSlider = new QSlider(Qt::Horizontal);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(tempVolume);
    volumeLabel = new QLabel(QString::number(tempVolume) + "%");
    volumeLayout->addWidget(volumeSlider);
    volumeLayout->addWidget(volumeLabel);
    volumeGroup->setLayout(volumeLayout);
    
    // 控制模式设置组
    auto controlGroup = new QGroupBox("控制模式");
    auto controlLayout = new QHBoxLayout;
    controlModeCombo = new QComboBox();
    controlModeCombo->addItem("键盘WASD控制");
    controlModeCombo->addItem("鼠标跟随控制");
    controlModeCombo->setCurrentIndex(tempControlMode);
    controlLayout->addWidget(new QLabel("选择控制方式："));
    controlLayout->addWidget(controlModeCombo);
    controlGroup->setLayout(controlLayout);
    
    // 按钮组
    auto buttonLayout = new QHBoxLayout;
    applyButton = new QPushButton("应用");
    cancelButton = new QPushButton("取消");
    buttonLayout->addStretch();
    buttonLayout->addWidget(applyButton);
    buttonLayout->addWidget(cancelButton);
    
    // 添加到主布局
    mainLayout->addWidget(volumeGroup);
    mainLayout->addWidget(controlGroup);
    mainLayout->addStretch();
    mainLayout->addLayout(buttonLayout);
    
    // 设置窗口大小
    setMinimumSize(400, 250);
    resize(400, 250);
}

void SettingDialog::onVolumeChanged(int value)
{
    tempVolume = value;
    volumeLabel->setText(QString::number(value) + "%");
}

void SettingDialog::onControlModeChanged(int index)
{
    tempControlMode = index;
}

void SettingDialog::onApplyClicked()
{
    // 应用设置到全局变量
    GAME_VOLUME = tempVolume;
    CONTROL_MODE = tempControlMode;
    
    // 输出日志
    qDebug() << "应用设置: 音量=" << GAME_VOLUME << ", 控制模式=" << CONTROL_MODE;
    
    // 关闭对话框
    accept();
}

void SettingDialog::onCancelClicked()
{
    // 直接关闭对话框，不保存设置
    reject();
}