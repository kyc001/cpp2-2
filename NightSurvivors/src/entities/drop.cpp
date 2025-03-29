#include "entities/drop.h"
#include "entities/hero.h"
#include "core/gamestate.h"
#include <QColor>
#include <QDebug>

Drop::Drop(DropType type, int value, int x, int y, GameState *state, QObject *parent)
    : QObject(parent),
      type(type),
      value(value),
      x(x),
      y(y),
      active(true),
      gameState(state),
      lifeTime(600), // 设置默认生命周期，例如10秒（如果以60FPS计算）
      animationFrame(0),
      animationCounter(0),
      collected(false),
      collectedAnimationCounter(0)
{
    // 根据类型设置不同的颜色
    switch (type)
    {
    case DropType::HEALTH:
        dropColor = QColor(255, 0, 0); // 红色
        break;
    case DropType::EXPERIENCE:
        dropColor = QColor(0, 255, 0); // 绿色
        break;
    case DropType::COIN:
        dropColor = QColor(255, 215, 0); // 金色
        break;
    case DropType::POWER_UP:
        dropColor = QColor(0, 0, 255); // 蓝色
        break;
    default:
        dropColor = QColor(255, 255, 255); // 白色
    }

    // 加载对应类型的精灵图
    // 这里可以根据不同类型设置不同的图像
    // sprite = ResourceManager::getInstance().getDropSprite(type);
}

Drop::~Drop()
{
    // 清理资源
}

bool Drop::isActive() const
{
    return active;
}

DropType Drop::getType() const
{
    return type; // 返回枚举值
}

int Drop::getValue() const
{
    return value;
}

int Drop::getX() const
{
    return x;
}

int Drop::getY() const
{
    return y;
}

void Drop::applyEffect(Hero *hero)
{
    if (!hero || !active)
        return;

    switch (type)
    {
    case DropType::HEALTH:
        hero->heal(value);
        break;
    case DropType::EXPERIENCE:
        hero->addExperience(value);
        break;
    case DropType::COIN:
        // 处理金币逻辑
        if (gameState)
        {
            // gameState->addCoins(value);
            qDebug() << "Player collected" << value << "coins";
        }
        break;
    case DropType::POWER_UP:
        hero->applyPowerUp(value);
        break;
    }

    active = false;
    collected = true;
    collectedAnimationCounter = 30; // 设置收集动画帧数
}

void Drop::update()
{
    if (!active && !collected)
        return;

    if (collected)
    {
        collectedAnimationCounter--;
        if (collectedAnimationCounter <= 0)
        {
            collected = false;
        }
        return;
    }

    // 更新生命周期
    if (lifeTime > 0)
    {
        lifeTime--;
    }
    else
    {
        active = false;
    }

    // 更新动画
    animationCounter++;
    if (animationCounter >= 10)
    { // 每10帧更新一次
        animationCounter = 0;
        animationFrame = (animationFrame + 1) % 4; // 假设有4帧动画
    }
}

void Drop::render(QPainter *painter)
{
    if (!active && !collected)
        return;

    painter->save();

    // 如果有精灵图则使用精灵图渲染
    if (!sprite.isNull())
    {
        // 根据animationFrame计算精灵图上的位置
        // painter->drawPixmap(x, y, sprite, sx, sy, width, height);
    }
    else
    {
        // 没有精灵图则使用简单的几何形状
        painter->setPen(Qt::NoPen);
        painter->setBrush(dropColor);

        if (collected)
        {
            // 收集动画：逐渐缩小并上升
            int size = 10 * collectedAnimationCounter / 30;
            painter->drawEllipse(x - size / 2, y - size / 2 - (30 - collectedAnimationCounter), size, size);
        }
        else
        {
            // 正常显示：上下浮动的效果
            int yOffset = qSin(animationFrame * M_PI / 2) * 3; // 垂直偏移量
            painter->drawEllipse(x - 5, y - 5 + yOffset, 10, 10);
        }
    }

    painter->restore();
}

QRect Drop::getCollisionRect() const
{
    // 返回一个碰撞检测用的矩形区域
    return QRect(x - 10, y - 10, 20, 20);
}