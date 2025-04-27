//
// 游戏设置对话框头文件
//

#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QDialog>
#include <QSlider>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include "../GameLogic/config.h"

class SettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingDialog(QWidget *parent = nullptr);
    ~SettingDialog();

private slots:
    void onVolumeChanged(int value);
    void onControlModeChanged(int index);
    void onApplyClicked();
    void onCancelClicked();

private:
    // 组件定义
    QSlider *volumeSlider;
    QLabel *volumeLabel;
    QComboBox *controlModeCombo;
    QPushButton *applyButton;
    QPushButton *cancelButton;
    
    // 临时保存设置，点击应用后才真正修改全局变量
    int tempVolume;
    int tempControlMode;
    
    // 初始化UI
    void setupUI();
};

#endif // SETTINGDIALOG_H 