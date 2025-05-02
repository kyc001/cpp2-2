#ifndef SURVIVORS_EXPBALL_H
#define SURVIVORS_EXPBALL_H
#include <utility>
#include <QtCore/QRect>
#include "PaintInfo.h"
#include <QtGui/QPixmap>
#include <exception>

class GameMap;

class ExpBall {
    int value;
    int cdn;
    bool enabled;
    std::pair<double, double> absolute_pos;
    QPixmap _image;
    GameMap * map_parent;
public:
    QRect real_rect;
    std::pair<double, double> real_pos;
    ExpBall(double x, double y, int value, GameMap * m_parent);
    void disable() { 
        enabled = false; 
        // 确保经验球一被禁用就不再显示
        absolute_pos.first = -1000;  // 移出屏幕
        absolute_pos.second = -1000; // 移出屏幕
    }
    void enable(double x, double y, int value);
    [[nodiscard]] bool isEnabled() const { return enabled; }
    [[nodiscard]] int getValue() const { return value; }
    PaintInfo paint();

    void tick();
};

class TimeLimitExceeded : public std::exception{
    int type;
public:
    explicit TimeLimitExceeded(int t){ type = t; };
};

#endif //VSCPROJECT_EXPBALL_H
