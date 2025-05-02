#include "Enemy.h"
#include <cmath>
#include "EnemyController.h"
#include <cstdlib>
#include "Weapon.h"
#include "Hero.h"
#include <iostream>
#include <QtCore/QDir>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QFile>
#include <QStringList>

Enemy::Enemy(int enemy_style, QWidget *w_parent, EnemyController *controller, GameMap *m_parent, Hero *target, double real_X, double real_Y) {
    std::cout << "[Log] Enemy 构造函数开始, 类型: " << enemy_style << std::endl;
    QSize pix_size;
    enemy_type = enemy_style;
    
    // 输出当前目录信息，帮助调试
    std::cout << "当前目录: " << QDir::currentPath().toStdString() << std::endl;
    
    // 提前声明文件检查变量
    QString imagePath;
    
    switch(enemy_style) {
        case 1:
            imagePath = ENEMY_1_PATH;
            std::cout << "尝试加载敌人1图片: " << imagePath.toStdString() << std::endl;
            
            // 检查文件是否存在
            if (QFile::exists(imagePath)) {
                std::cout << "图片文件存在，尝试加载..." << std::endl;
            } else {
                std::cout << "图片文件不存在!尝试使用其他路径加载..." << std::endl;
                // 尝试其他路径
                QStringList paths;
                paths << ":/Assets/laocai.png"
                      << "D:/Survivors/Survivors/Assets/laocai.png"
                      << "../Assets/laocai.png"
                      << "../laocai.png"
                      << "./Assets/laocai.png"
                      << "./laocai.png"
                      << "laocai.png";
                
                for (const QString& path : paths) {
                    std::cout << "尝试路径: " << path.toStdString() << std::endl;
                    if (QFile::exists(path)) {
                        std::cout << "找到文件!" << std::endl;
                        imagePath = path;
                        break;
                    }
                }
            }
            
            // 尝试加载图片
            if (!_image.load(imagePath)) {
                std::cout << "无法加载敌人1图片: " << imagePath.toStdString() << std::endl;
                
                // 尝试直接加载根目录的laocai.jpg，因为之前看到项目根目录有这个文件
                if (QFile::exists("laocai.jpg") && _image.load("laocai.jpg")) {
                    std::cout << "成功从根目录加载laocai.jpg" << std::endl;
                } else {
                    // 创建一个红色圆形作为替代
                    _image = QImage(150, 150, QImage::Format_RGBA8888);
                    _image.fill(Qt::transparent);
                    
                    // 直接绘制自定义图形
                    QPainter painter(&_image);
                    painter.setRenderHint(QPainter::Antialiasing);
                    
                    // 绘制一个红色圆形
                    painter.setPen(QPen(Qt::black, 3));
                    painter.setBrush(QColor(200, 0, 0));
                    painter.drawEllipse(20, 20, 110, 110);
                    
                    // 添加一些细节
                    painter.setBrush(QColor(255, 150, 150));
                    painter.drawEllipse(40, 40, 35, 35);
                    painter.drawEllipse(75, 40, 35, 35);
                    
                    std::cout << "已创建替代图形" << std::endl;
                }
            } else {
                std::cout << "成功加载敌人1图片，尺寸: " << _image.width() << "x" << _image.height() << std::endl;
            }
            HP_MAX = ENEMY_1_HEALTH;
            speed = ENEMY_1_SPEED;
            pix_size.setWidth(150);
            pix_size.setHeight(150);
            break;
        case 2:
            imagePath = ENEMY_1_PATH;  // 使用敌人1的图片
            std::cout << "尝试加载敌人2图片: " << imagePath.toStdString() << std::endl;
            if (!_image.load(imagePath)) {
                std::cout << "无法加载敌人2图片: " << imagePath.toStdString() << std::endl;
                // 创建一个绿色方块作为替代
                _image = QImage(150, 150, QImage::Format_RGBA8888);
                _image.fill(Qt::green);
            } else {
                std::cout << "成功加载敌人2图片，尺寸: " << _image.width() << "x" << _image.height() << std::endl;
            }
            HP_MAX = ENEMY_1_HEALTH;  // 使用敌人1的生命值
            speed = ENEMY_1_SPEED;    // 使用敌人1的速度
            pix_size.setWidth(150);
            pix_size.setHeight(150);
            break;
        case 3:
            imagePath = ENEMY_1_PATH;  // 使用敌人1的图片
            std::cout << "尝试加载敌人3图片: " << imagePath.toStdString() << std::endl;
            if (!_image.load(imagePath)) {
                std::cout << "无法加载敌人3图片: " << imagePath.toStdString() << std::endl;
                // 创建一个蓝色方块作为替代
                _image = QImage(150, 150, QImage::Format_RGBA8888);
                _image.fill(Qt::blue);
            } else {
                std::cout << "成功加载敌人3图片，尺寸: " << _image.width() << "x" << _image.height() << std::endl;
            }
            HP_MAX = ENEMY_1_HEALTH;  // 使用敌人1的生命值
            speed = ENEMY_1_SPEED;    // 使用敌人1的速度
            pix_size.setWidth(150);
            pix_size.setHeight(150);
            break;
    }
    enabled = false;
    alive = false;

    hp = HP_MAX;
    widget_parent = w_parent;
    hp_bar = nullptr;
    if (widget_parent) {
        std::cout << "[Log] Enemy 构造函数: 尝试创建 hp_bar" << std::endl;
        hp_bar = new QProgressBar();
        hp_bar->setParent(widget_parent);
        hp_bar->hide();
        std::cout << "[Log] Enemy 构造函数: hp_bar 创建成功" << std::endl;
    } else {
        std::cout << "警告: Enemy 创建时 widget_parent 为空，无法创建 hp_bar" << std::endl;
    }

    _image = _image.scaled(pix_size);

    map_parent = m_parent;
    this->target = target;

    real_pos.first = real_X;
    real_pos.second = real_Y;
    real_rect.setWidth(_image.width());
    real_rect.setHeight(_image.height());
    real_rect.moveTo((int)real_pos.first, (int)real_pos.second);

    absolute_pos.first = (int)real_pos.first + m_parent->getAbsoluteX();
    absolute_pos.second = (int)real_pos.second + m_parent->getAbsoluteY();
    absolute_rect.setWidth(_image.width());
    absolute_rect.setHeight(_image.height());
    absolute_rect.moveTo(absolute_pos.first, absolute_pos.second);

    healthChange();
    setHpBarPosition();

    this->controller = controller;
    std::cout << "[Log] Enemy 构造函数结束" << std::endl;
}

std::vector<PaintInfo> Enemy::paint() {
    std::vector<PaintInfo> temp;
    
    // 使用敌人自身的QImage创建QPixmap
    QPixmap pixmap = QPixmap::fromImage(_image);
    
    if (pixmap.isNull()) {
        std::cout << "警告：敌人" << enemy_type << "的图像创建失败" << std::endl;
        // 根据敌人类型创建不同颜色的备用图像
        QPixmap defaultPixmap(150, 150);
        switch(enemy_type) {
            case 1: 
                defaultPixmap.fill(Qt::transparent);
                {
                    QPainter painter(&defaultPixmap);
                    painter.setRenderHint(QPainter::Antialiasing);
                    // 绘制红色圆形
                    painter.setPen(QPen(Qt::black, 3));
                    painter.setBrush(QColor(200, 0, 0));
                    painter.drawEllipse(20, 20, 110, 110);
                }
                break;
            case 2: 
                defaultPixmap.fill(Qt::transparent);
                {
                    QPainter painter(&defaultPixmap);
                    painter.setRenderHint(QPainter::Antialiasing);
                    // 绘制绿色圆形
                    painter.setPen(QPen(Qt::black, 3));
                    painter.setBrush(QColor(0, 200, 0));
                    painter.drawEllipse(20, 20, 110, 110);
                }
                break;
            case 3: 
                defaultPixmap.fill(Qt::transparent);
                {
                    QPainter painter(&defaultPixmap);
                    painter.setRenderHint(QPainter::Antialiasing);
                    // 绘制蓝色圆形
                    painter.setPen(QPen(Qt::black, 3));
                    painter.setBrush(QColor(0, 0, 200));
                    painter.drawEllipse(20, 20, 110, 110);
                }
                break;
            default: 
                defaultPixmap.fill(Qt::transparent);
                {
                    QPainter painter(&defaultPixmap);
                    painter.setRenderHint(QPainter::Antialiasing);
                    // 绘制灰色圆形
                    painter.setPen(QPen(Qt::black, 3));
                    painter.setBrush(Qt::gray);
                    painter.drawEllipse(20, 20, 110, 110);
                }
                break;
        }
        temp.push_back(PaintInfo(defaultPixmap, absolute_pos.first, absolute_pos.second));
        std::cout << "绘制了备用敌人图像，尺寸: 150x150" << std::endl;
    } else {
        temp.push_back(PaintInfo(pixmap, absolute_pos.first, absolute_pos.second));
    }
    
    return temp;
}

void Enemy::setRealPosition(double x, double y) {
    real_pos.first = x;
    real_pos.second = y;
    real_rect.moveTo((int)real_pos.first, (int)real_pos.second);

    absolute_pos.first = (int)real_pos.first + map_parent->getAbsoluteX();
    absolute_pos.second = (int)real_pos.second + map_parent->getAbsoluteY();
    absolute_rect.moveTo(absolute_pos.first, absolute_pos.second);
    setHpBarPosition();
}

void Enemy::damage(int d) {
    if(alive) {
        hp -= d;
        if (hp_bar) {
            hp_bar->show();
        }
        healthChange();
        if(hp <= 0)
            die();
    }
}

void Enemy::die() {
    alive = false;
    disable();
    if(controller)
        controller->reportDeath(enemy_type, real_pos.first, real_pos.second);
}

void Enemy::enable() {
    std::cout << "[Log] Enemy::enable 开始, 类型: " << enemy_type << std::endl;
    alive = true;
    enabled = true;
    hp = HP_MAX;
    if (hp_bar) {
        hp_bar->show();
        setHpBarPosition();
        healthChange();
        hp_bar->hide();
    }
    std::cout << "[Log] Enemy::enable 结束" << std::endl;
}

void Enemy::disable() {
    enabled = false;
    if (hp_bar) {
        hp_bar->hide();
    }
}

void Enemy::healthChange() {
    if (hp_bar) {
        hp_bar->setRange(0, HP_MAX);
        hp_bar->setValue(hp);
    }
}

void Enemy::setHpBarPosition() {
    if (hp_bar) {
        hp_bar->setGeometry(absolute_pos.first, absolute_pos.second - 20, absolute_rect.width(), 40);
    }
}

NoWeaponEnemy::NoWeaponEnemy(int enemy_style, QWidget *w_parent, EnemyController *controller, GameMap *m_parent, Hero *target,
                             double real_X, double real_Y) :
                             Enemy(enemy_style, w_parent, controller, m_parent, target, real_X, real_Y) {
    switch(enemy_style) {
        case 1:
            power = ENEMY_1_POWER;
            CD = ENEMY_1_CD;
            break;
        case 2:
            power = ENEMY_1_POWER;  // 使用敌人1的伤害值
            CD = ENEMY_1_CD;       // 使用敌人1的冷却时间
            break;
        case 3:
            power = 0;
            CD = 100;
            break;
    }
    cdn = CD;
}

bool NoWeaponEnemy::judgeDamage() {
    if(cdn > 0) {
        cdn--;
        return false;
    } else {
        cdn = CD;
    }
    
    // 添加安全检查
    if(!target || target->isGameStop()) {
        return false;
    }
    
    // 增加日志输出，便于调试
    std::cout << "敌人位置: (" << real_pos.first << "," << real_pos.second 
              << "), 玩家位置: (" << target->getRealX() << "," << target->getRealY() << ")" << std::endl;
    
    try {
        if(real_rect.intersects(target->real_rect)) {
            std::cout << "敌人" << enemy_type << "与玩家发生碰撞!" << std::endl;
            target->damage(power);
            // 不再立即禁用敌人，而是给它一个冷却时间
            // disable();
            return true;
        } else {
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "碰撞检测出现异常: " << e.what() << std::endl;
        return false;
    }
}

void NoWeaponEnemy::tick() {
    auto direction = getDirectionVector();
    double bias = (rand() % (int)speed) * 0.6;
    direction.first *= speed;
    direction.second *= speed;
    direction.first += bias;
    direction.second += bias;
    setRealPosition(real_pos.first + direction.first,
                    real_pos.second + direction.second);
    judgeDamage();
}

std::pair<double, double> NoWeaponEnemy::getDirectionVector() {
    std::pair<double, double> temp;
    temp.first = target->getRealX() - real_pos.first;
    temp.second = target->getRealY() - real_pos.second;
    if(abs(temp.first) < speed / 2.0 && abs(temp.second) < speed / 2.0) {
        return {0, 0};
    }
    double mod = sqrt(temp.first * temp.first + temp.second * temp.second);
    temp.first /= mod;
    temp.second /= mod;
    return temp;
}

NoWeaponEnemyGround::NoWeaponEnemyGround(int enemy_style, QWidget *w_parent, EnemyController *controller,
                                         GameMap *m_parent, Hero *target, double real_X, double real_Y) : NoWeaponEnemy(
        enemy_style, w_parent, controller, m_parent, target, real_X, real_Y) {}

void NoWeaponEnemyGround::tick() {
    auto direction = getDirectionVector();
    double bias = (rand() % (int)speed) * 0.6;
    direction.first *= speed;
    direction.second *= speed;
    direction.first += bias;
    direction.second += bias;
    if(!attemptMove(direction.first, 0)) {
        direction.first = 0;
    }
    if(!attemptMove(0, direction.second)) {
        direction.second = 0;
    }
    setRealPosition(real_pos.first + direction.first, real_pos.second + direction.second);
    judgeDamage();
}

bool NoWeaponEnemyGround::attemptMove(double x_bias, double y_bias) {
    QRect test(real_rect);
    test.moveTo(int(real_pos.first + x_bias), int(real_pos.second + y_bias));
    if(map_parent->checkPosition(test)) {
        init_interact = false;
        return true;
    } else {
        return init_interact;
    }
}

WeaponEnemy::WeaponEnemy(int enemy_style, QWidget *w_parent, EnemyController *controller, GameMap *m_parent,
                         Hero *target, double real_X, double real_Y) :
        NoWeaponEnemy(enemy_style, w_parent, controller, m_parent, target, real_X, real_Y) {
    _weapon = new EnemyDynamicWeapon(m_parent, this, target, E_WEAPON_CD, E_WEAPON_DAMAGE);
    x_move_range.first = real_X - 200;
    direction = true;
    x_move_range.second = real_X + 200;

    if(x_move_range.first < 0) {
        x_move_range.first = 0;
    }
    if(x_move_range.second > map_parent->getPosRangeX()) {
        x_move_range.second = map_parent->getPosRangeX();
    }
}

void WeaponEnemy::tick() {
    if(direction) {
        setRealPosition(real_pos.first + speed, real_pos.second);
        if(real_pos.first > x_move_range.second) {
            real_pos.first = x_move_range.second;
            direction = false;
        }
    } else {
        setRealPosition(real_pos.first - speed, real_pos.second);
        if(real_pos.first < x_move_range.first) {
            real_pos.first = x_move_range.first;
            direction = true;
        }
    }
    _weapon->tick();
}

std::vector<PaintInfo> WeaponEnemy::paint() {
    std::vector<PaintInfo> buffer;
    std::vector<PaintInfo> temp = Enemy::paint();
    buffer.reserve(temp.size());
    for(auto &item : temp) {
        buffer.push_back(item);
    }
    temp = _weapon->paint();
    for(auto &item : temp) {
        buffer.push_back(item);
    }
    return buffer;
}
