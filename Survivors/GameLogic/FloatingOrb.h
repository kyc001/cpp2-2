#ifndef SURVIVORS_FLOATINGORB_H
#define SURVIVORS_FLOATINGORB_H

#include <QtGui/QPixmap>
#include <utility>
#include <cmath>
#include "PaintInfo.h"
#include "config.h"
// #include "Hero.h"
class Hero;
class Enemy;

class GameState;

class FloatingOrb {
private:
    QPixmap _image;
    Hero* _hero;
    GameState* _gameState;
    double _angle;           // 当前角度
    double _radius;          // 旋转半径
    double rotation_speed;    // 新的旋转速度成员
    int _index;              // 悬浮球索引，用于确定颜色和角度偏移
    std::pair<double, double> _position; // 悬浮球的位置
    int _damage;             // 悬浮球攻击力
    int _knockbackForce;     // 击退力度

public:
    FloatingOrb(Hero* hero, GameState* gameState, int index, double radius = 60.0, double initial_speed = 0.05);
    
    void tick();                     // 更新悬浮球位置
    PaintInfo paint();               // 获取绘制信息
    bool checkCollision(Enemy* enemy); // 检查与敌人的碰撞并施加击退效果
    
    QRect getCollisionRect() const;  // 获取碰撞检测矩形
    
    void increaseSpeed(double factor = 1.2);  // 新增方法：提高转速
};

#endif //SURVIVORS_FLOATINGORB_H 