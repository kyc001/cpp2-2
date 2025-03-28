#ifndef SETTINGSUI_H
#define SETTINGSUI_H

#include <QWidget>
#include <QSlider>
#include <QComboBox>
#include <QRadioButton>
#include <QLabel>
#include <QPushButton>
#include <QButtonGroup>

class SettingsUI : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsUI(QWidget *parent = nullptr);
    ~SettingsUI() override;

    void show();
    void hide();
    bool isVisible() const;

signals:
    void settingsUIClosed();
    void volumeChanged(int volume);
    void screenSizeChanged(int index);
    void controlTypeChanged(int type); // 0 for WASD, 1 for Mouse

private slots:
    void onCloseButtonClicked();
    void onVolumeSliderChanged(int value);
    void onScreenSizeComboChanged(int index);
    void onControlTypeChanged(int id);
    void onSaveButtonClicked();

private:
    void setupUI();
    void saveSettings();
    void loadSettings();

    QSlider *volume_slider;
    QLabel *volume_label;
    QComboBox *screen_size_combo;
    QButtonGroup *control_type_group;
    QRadioButton *wasd_radio;
    QRadioButton *mouse_radio;
    QPushButton *close_button;
    QPushButton *save_button;
};

#endif // SETTINGSUI_H 