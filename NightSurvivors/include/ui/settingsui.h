#ifndef SETTINGSUI_H
#define SETTINGSUI_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QSlider>
#include <QComboBox>
#include <QRadioButton>
#include <QButtonGroup>

// 设置界面
class SettingsUI : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsUI(QWidget *parent = nullptr);
    ~SettingsUI();

    // 居中显示设置界面
    void centerUI();

    void show();
    void hide();
    bool isVisible() const;

signals:
    // 关闭设置界面的信号
    void settingsUIClosed();

    // 设置更新的信号
    void volumeChanged(int volume);
    void screenSizeChanged(int index);
    void controlTypeChanged(int type); // 0 for WASD, 1 for Mouse

private slots:
    // 按钮槽函数
    void onCloseButtonClicked();
    void onVolumeSliderChanged(int value);
    void onScreenSizeComboChanged(int index);
    void onControlTypeChanged();
    void onSaveButtonClicked();

private:
    void setupUI();
    void saveSettings();
    void loadSettings();

    // UI组件
    QSlider *volume_slider;
    QLabel *volume_label;
    QComboBox *screen_size_combo;
    QButtonGroup *control_type_group;
    QRadioButton *wasd_radio;
    QRadioButton *mouse_radio;
    QPushButton *close_button;
    QPushButton *save_button;

    // 设置值
    int current_volume;
    int current_screen_index;
    int current_control_type;
};

#endif // SETTINGSUI_H 