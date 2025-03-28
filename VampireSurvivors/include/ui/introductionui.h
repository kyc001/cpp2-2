#ifndef INTRODUCTIONUI_H
#define INTRODUCTIONUI_H

#include <QWidget>
#include <QTabWidget>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>

class IntroductionUI : public QWidget
{
    Q_OBJECT

public:
    explicit IntroductionUI(QWidget *parent = nullptr);
    ~IntroductionUI() override;

    void show();
    void hide();
    bool isVisible() const;

signals:
    void introductionUIClosed();

private slots:
    void onCloseButtonClicked();

private:
    void setupUI();
    QWidget* createControlsTab();
    QWidget* createHeroesTab();
    QWidget* createEnemiesTab();

    QTabWidget *tab_widget;
    QPushButton *close_button;
};

#endif // INTRODUCTIONUI_H 