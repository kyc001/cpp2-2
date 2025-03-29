#ifndef DROP_H
#define DROP_H

#include <QObject>
#include <QPainter>
#include <QRect>

class Hero;
class GameState;

// 定义掉落物类型枚举 - 移到类外以便全局访问
enum class DropType
{
    HEALTH,     // 生命值回复
    EXPERIENCE, // 经验值
    COIN,       // 金币
    POWER_UP    // 能力提升
};

class Drop : public QObject
{
    Q_OBJECT

public:
    explicit Drop(DropType type, int value, int x, int y, GameState *state, QObject *parent = nullptr);
    ~Drop();

    bool isActive() const;
    DropType getType() const;
    int getValue() const;
    int getX() const;
    int getY() const;
    void applyEffect(Hero *hero);
    void update();
    void render(QPainter *painter);
    QRect getCollisionRect() const;

private:
    DropType type;
    int value;
    int x;
    int y;
    bool active;
    GameState *gameState;

    // 新增生命周期计数器
    int lifeTime;

    // 渲染相关
    QPixmap sprite;
    int animationFrame;
    int animationCounter;
    bool collected;
    int collectedAnimationCounter;

    // 颜色
    QColor dropColor;
};

#endif // DROP_H