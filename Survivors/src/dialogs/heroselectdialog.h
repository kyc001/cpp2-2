#ifndef HEROSELECTDIALOG_H
#define HEROSELECTDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMovie>

class Menu;

class HeroSelectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HeroSelectDialog(QWidget *parent = nullptr);
    ~HeroSelectDialog();

    // 获取选择的英雄类型
    int getSelectedHeroType() const { return selectedHeroType; }

private slots:
    void onHero1Selected();
    void onHero2Selected();

private:
    int selectedHeroType = 1; // 默认为英雄1

    // UI组件
    QVBoxLayout *mainLayout;
    QHBoxLayout *heroesLayout;
    
    // 英雄1相关
    QWidget *hero1Widget;
    QVBoxLayout *hero1Layout;
    QLabel *hero1ImageLabel;
    QMovie *hero1Movie;
    QLabel *hero1NameLabel;
    QLabel *hero1DescLabel;
    QPushButton *hero1SelectButton;
    
    // 英雄2相关
    QWidget *hero2Widget;
    QVBoxLayout *hero2Layout;
    QLabel *hero2ImageLabel;
    QMovie *hero2Movie;
    QLabel *hero2NameLabel;
    QLabel *hero2DescLabel;
    QPushButton *hero2SelectButton;
    
    // 标题和描述
    QLabel *titleLabel;
    QLabel *infoLabel;
};

#endif // HEROSELECTDIALOG_H 