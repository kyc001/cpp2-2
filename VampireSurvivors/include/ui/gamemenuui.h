#ifndef GAMEMENUUI_H
#define GAMEMENUUI_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>

class GameMenuUI : public QWidget
{
    Q_OBJECT

public:
    explicit GameMenuUI(QWidget *parent = nullptr);
    ~GameMenuUI() override;

    void show();
    void hide();
    bool isVisible() const;

signals:
    void resumeGameSignal();
    void settingsOpenSignal();
    void saveGameSignal();
    void mainMenuSignal();
    void quitGameSignal();

private slots:
    void onResumeButtonClicked();
    void onSettingsButtonClicked();
    void onSaveButtonClicked();
    void onMainMenuButtonClicked();
    void onQuitButtonClicked();

private:
    void setupUI();

    QPushButton *resume_button;
    QPushButton *settings_button;
    QPushButton *save_button;
    QPushButton *main_menu_button;
    QPushButton *quit_button;
};

#endif // GAMEMENUUI_H 