#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <QObject>
#include "hero.h"

class GameState : public QObject {
    Q_OBJECT

public:
    explicit GameState(QObject *parent = nullptr);
    ~GameState() override;

    //初始化
    void init();
    //更新状态
    void update();
    //获取英雄对象
    Hero* getHero();
private:
    Hero *my_hero;
    int my_screenCenterX;
    int my_screenCenterY;

};

#endif // GAMESTATE_H
