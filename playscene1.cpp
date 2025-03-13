#include "playscene1.h"

PlayScene1::PlayScene1(QWidget *parent) : baseplayscene(parent)
{
    // 可以在这里添加初始化代码
}

PlayScene1::~PlayScene1()
{
    // 可以在这里添加清理代码
}

void PlayScene1::paintEvent(QPaintEvent *event)
{
    // 实现绘制事件
    baseplayscene::paintEvent(event);
    // 可以在这里添加自定义绘制代码
}
