#ifndef SURVIVORS_HERO_H
#define SURVIVORS_HERO_H
#include <utility>
#include <QWidget>
#include <QProgressBar>
#include <QRect>
#include <QKeyEvent>
#include <QMouseEvent>
#include <list>
#include "PaintInfo.h"
#include "config.h"
#include "GameMap.h"
class Enemy;
// #include "Weapon.h"  // 改为前向声明

class Weapon;  // 前向声明
class GameState;

// 定义控制模式枚举
enum ControlMode {
    KEYBOARD_CONTROL,  // 使用键盘WASD控制
    MOUSE_CONTROL      // 跟随鼠标移动
};

class Hero {
    //基本属性
    int HP_MAX;
    int hp;
    int exp;
    int level;
    int EXP_MAX;
    bool alive;
    int speed;
    double reduce;
    
    // 控制模式 (键盘/鼠标)
    ControlMode controlMode;
    
    // 鼠标控制相关
    std::pair<int, int> mousePos;        // 鼠标位置
    bool mouseControlActive;             // 鼠标控制是否激活
    
    //两个可视化显示条
    QProgressBar * hp_bar;
    QProgressBar * exp_bar;
    //用于设置显示进度条的指针
    QWidget * widget_parent;
    //绝对窗口坐标
    std::pair<int, int> absolute_pos;
    //相对地图坐标
    std::pair<double, double> real_pos;
    QPixmap _image;
    GameMap * map_parent;
    //武器部分
    int weapon_type;
    Weapon * _weapon;

    //处理键盘问题
    std::list<int> keys_pressed;
    GameState * _game;

    //处理生成在障碍物上的问题
    bool init_interact = true;

    bool waiting_upgrade = false;

public:
    QRect absolute_rect; //碰撞箱
    QRect real_rect;

    Hero();
    explicit Hero(int hero_style, QWidget * w_parent, GameMap * m_parent); //一般使用这个
    void setGame(GameState * t) {_game = t;}

    void setWidgetParent(QWidget * parent);
    void giveWeapon();

    void tick(); //无条件tick
    void keyPressTick(QKeyEvent * event);
    void keyReleaseTick(QKeyEvent * event);
    
    // 鼠标事件处理
    void mouseMoveTick(QMouseEvent * event);
    
    // 切换控制模式
    void switchControlMode() { 
        controlMode = (controlMode == KEYBOARD_CONTROL) ? 
                        MOUSE_CONTROL : KEYBOARD_CONTROL; 
    }
    
    // 获取当前控制模式
    ControlMode getControlMode() const { return controlMode; }
    
    // 设置控制模式
    void setControlMode(ControlMode mode) { controlMode = mode; }
    
    bool judgeDamage(Enemy * e);
    std::vector<PaintInfo> paint();

    void damage(int h);

    [[nodiscard]] bool isGameStop() const { return !alive; }
    [[nodiscard]] bool isWaiting() const { return waiting_upgrade; }

    void setRealPosition(double x, double y);

    [[nodiscard]] double getRealX() const { return real_pos.first; };
    [[nodiscard]] double getRealY() const { return real_pos.second; };
    [[nodiscard]] int getAbsoluteX() const { return absolute_pos.first; };
    [[nodiscard]] int getAbsoluteY() const { return absolute_pos.second; };
    [[nodiscard]] GameState * getGame() const { return _game; }
    [[nodiscard]] int getLevel() const { return level; }
    [[nodiscard]] double get_hp_percent() const {
        int temp = hp;
        if(temp < 0){
            temp = 0;
        }
        return (double)temp / HP_MAX;
    }

    void upgrade(int type);

    [[nodiscard]] int getHpMax() const;

    [[nodiscard]] int getHp() const;

    [[nodiscard]] int getExp() const;

    [[nodiscard]] int getExpMax() const;

    [[nodiscard]] int getSpeed() const;

    [[nodiscard]] double getReduce() const;

    [[nodiscard]] const std::pair<double, double> &getRealPos() const;
    
    // 提供获取实际位置的公共方法
    [[nodiscard]] std::pair<double, double> getRealPosition() const { return real_pos; }

    int getDamage();
    int getCD();


private:
    void healthChange();
    void expChange();
    void setExpBarPosition();
    void setHpBarPosition();

    bool attemptMove(double x_bias, double y_bias);
    
    // 处理键盘控制逻辑
    void handleKeyboardControl();
    
    // 处理鼠标控制逻辑
    void handleMouseControl();
};


#endif //VSCPROJECT_HERO_H
