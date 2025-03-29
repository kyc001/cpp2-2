#ifndef DROP_H
#define DROP_H

#include <QObject>
#include <QRect>

class GameState;
class Hero;

// 掉落物类型
enum class DropType {
    HEALTH,       // 回复生命值
    EXPERIENCE,   // 经验值
    COIN,         // 金币
    POWER_UP      // 临时增强
};

class Drop : public QObject {
    Q_OBJECT
public:
    explicit Drop(DropType type, int value, int x, int y, GameState* state, QObject* parent = nullptr);
    ~Drop() override;
    
    // 位置相关
    int getX() const;
    int getY() const;
    
    // 状态相关
    bool isActive() const;
    void setActive(bool active);
    
    // 类型和价值
    DropType getType() const;
    int getValue() const;
    
    // 碰撞检测
    QRect getCollisionRect() const;
    
    // 被拾取时的效果
    void applyEffect(Hero* hero);
    
private:
    DropType drop_type;
    int drop_value;
    
    int x_pos;
    int y_pos;
    
    bool is_active;
    
    GameState* game_state;
};

#endif // DROP_H 