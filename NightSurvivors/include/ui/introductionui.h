#ifndef INTRODUCTIONUI_H
#define INTRODUCTIONUI_H

#include <QWidget>
#include <QTabWidget>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>

// 游戏介绍界面
class IntroductionUI : public QWidget
{
    Q_OBJECT

public:
    explicit IntroductionUI(QWidget *parent = nullptr);
    ~IntroductionUI() override;

    void show();
    void hide();
    bool isVisible() const;
    void centerUI();

signals:
    // 关闭游戏介绍界面的信号
    void introductionUIClosed();
    void introClosed();

private slots:
    // 关闭按钮点击槽函数
    void onCloseButtonClicked();

private:
    void setupUI();
    QWidget *createGameplayTab();
    QWidget *createControlsTab();
    QWidget *createHeroesTab();
    QWidget *createEnemiesTab();

    QTabWidget *tab_widget;
    QPushButton *close_button;
};

#endif // INTRODUCTIONUI_H