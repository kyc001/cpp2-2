#include "FloatingOrb.h"
#include "Hero.h"
#include "Enemy.h"
#include "GameState.h"
#include <cmath>
#include <QColor>
#include <QPainter>

// 悬浮球颜色数组
const QColor ORB_COLORS[] = {
    QColor(255, 100, 100),  // 红色
    QColor(100, 255, 100),  // 绿色
    QColor(100, 100, 255)   // 蓝色
};

FloatingOrb::FloatingOrb(Hero* hero, GameState* gameState, int index, double radius, double speed)
    : _hero(hero), _gameState(gameState), _index(index), _radius(radius), _speed(speed) {
    // 每个悬浮球初始位置的角度偏移 (按120度分布)
    _angle = index * 2.0 * M_PI / 3.0;
    
    // 创建悬浮球图像
    _image = QPixmap(20, 20);
    _image.fill(Qt::transparent);
    
    QPainter painter(&_image);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 使用不同颜色
    painter.setBrush(ORB_COLORS[index % 3]);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(0, 0, 20, 20);
    
    // 光影效果
    QRadialGradient gradient(10, 10, 10);
    gradient.setColorAt(0, QColor(255, 255, 255, 150));
    gradient.setColorAt(1, QColor(255, 255, 255, 0));
    painter.setBrush(gradient);
    painter.drawEllipse(3, 3, 14, 14);
    
    // 初始位置设置
    _position.first = 0;
    _position.second = 0;
    
    // 设置攻击力和击退力
    _damage = 3 + index * 2;  // 根据索引设置不同的攻击力
    _knockbackForce = 20 + index * 5; // 击退力度也随索引增加
}

void FloatingOrb::tick() {
    // 更新角度
    _angle += _speed;
    if (_angle > 2 * M_PI) {
        _angle -= 2 * M_PI;
    }
    
    // 计算新位置
    double x = _hero->getRealX() + _hero->real_rect.width() / 2 + _radius * cos(_angle) - _image.width() / 2;
    double y = _hero->getRealY() + _hero->real_rect.height() / 2 + _radius * sin(_angle) - _image.height() / 2;
    
    _position.first = x;
    _position.second = y;
}

PaintInfo FloatingOrb::paint() {
    // 转换为绝对坐标
    int abs_x = _position.first + _hero->getAbsoluteX() - _hero->getRealX();
    int abs_y = _position.second + _hero->getAbsoluteY() - _hero->getRealY();
    
    return {_image, abs_x, abs_y};
}

QRect FloatingOrb::getCollisionRect() const {
    return QRect(_position.first, _position.second, _image.width(), _image.height());
}

bool FloatingOrb::checkCollision(Enemy* enemy) {
    if (!enemy || !enemy->enabled) {
        return false;
    }
    
    QRect orbRect = getCollisionRect();
    
    if (orbRect.intersects(enemy->real_rect)) {
        // 计算击退方向（从悬浮球指向敌人的方向）
        double dx = enemy->real_pos.first - _position.first;
        double dy = enemy->real_pos.second - _position.second;
        
        // 标准化方向向量
        double magnitude = sqrt(dx * dx + dy * dy);
        if (magnitude > 0) {
            dx /= magnitude;
            dy /= magnitude;
        }
        
        // 计算新位置（添加击退效果）
        double newX = enemy->real_pos.first + dx * _knockbackForce;
        double newY = enemy->real_pos.second + dy * _knockbackForce;
        
        // 更新敌人位置
        enemy->setRealPosition(newX, newY);
        
        // 对敌人造成伤害
        enemy->damage(_damage);
        
        return true;
    }
    
    return false;
} 