#include "playscene2.h"
#include <QPainter>

PlayScene2::PlayScene2(QWidget *parent) : baseplayscene(parent) {}

PlayScene2::~PlayScene2() {}

void PlayScene2::paintEvent(QPaintEvent *event) {
    baseplayscene::paintEvent(event);
    // 可以在这里添加PlayScene2的绘制逻辑
}