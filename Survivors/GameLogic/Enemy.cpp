#include "Enemy.h"
#include <cmath>
#include "EnemyController.h"
#include <cstdlib>
#include "Weapon.h"
#include "Hero.h"
#include <iostream>
#include <QtCore/QDir>

Enemy::Enemy(int enemy_style, QWidget *w_parent, EnemyController *controller, GameMap *m_parent, Hero *target, double real_X, double real_Y) {
    QSize pix_size;
    enemy_type = enemy_style;
    
    // 输出当前目录信息，帮助调试
    std::cout << "当前目录: " << QDir::currentPath().toStdString() << std::endl;
    
    switch(enemy_style) {
        case 1:
            std::cout << "尝试加载敌人1图片: " << ENEMY_1_PATH << std::endl;
            if (!_image.load(ENEMY_1_PATH)) {
                std::cout << "无法加载敌人1图片: " << ENEMY_1_PATH << std::endl;
                // 创建一个红色方块作为替代
                _image = QImage(40, 40, QImage::Format_RGBA8888);
                _image.fill(Qt::red);
            } else {
                std::cout << "成功加载敌人1图片，尺寸: " << _image.width() << "x" << _image.height() << std::endl;
            }
            HP_MAX = ENEMY_1_HEALTH;
            speed = ENEMY_1_SPEED;
            pix_size.setWidth(40);
            pix_size.setHeight(40);
            break;
        case 2:
            std::cout << "尝试加载敌人2图片: " << ENEMY_2_PATH << std::endl;
            if (!_image.load(ENEMY_2_PATH)) {
                std::cout << "无法加载敌人2图片: " << ENEMY_2_PATH << std::endl;
                // 创建一个绿色方块作为替代
                _image = QImage(40, 40, QImage::Format_RGBA8888);
                _image.fill(Qt::green);
            } else {
                std::cout << "成功加载敌人2图片，尺寸: " << _image.width() << "x" << _image.height() << std::endl;
            }
            HP_MAX = ENEMY_2_HEALTH;
            speed = ENEMY_2_SPEED;
            pix_size.setWidth(40);
            pix_size.setHeight(40);
            break;
        case 3:
            std::cout << "尝试加载敌人3图片: " << ENEMY_3_PATH << std::endl;
            if (!_image.load(ENEMY_3_PATH)) {
                std::cout << "无法加载敌人3图片: " << ENEMY_3_PATH << std::endl;
                // 创建一个蓝色方块作为替代
                _image = QImage(40, 40, QImage::Format_RGBA8888);
                _image.fill(Qt::blue);
            } else {
                std::cout << "成功加载敌人3图片，尺寸: " << _image.width() << "x" << _image.height() << std::endl;
            }
            HP_MAX = ENEMY_3_HEALTH;
            speed = ENEMY_3_SPEED;
            pix_size.setWidth(40);
            pix_size.setHeight(40);
            break;
    }
    enabled = false;
    alive = false;

    hp = HP_MAX;
    widget_parent = w_parent;
    hp_bar = new QProgressBar();
    hp_bar->setParent(widget_parent);
    hp_bar->hide();

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
}

std::vector<PaintInfo> Enemy::paint() {
    std::vector<PaintInfo> temp;
    
    // 创建一个永久存在的静态QPixmap对象作为备用
    static QPixmap defaultPixmapRed(40, 40);
    static QPixmap defaultPixmapGreen(40, 40);
    static bool initialized = false;
    
    if (!initialized) {
        defaultPixmapRed.fill(Qt::red);
        defaultPixmapGreen.fill(Qt::green);
        initialized = true;
    }
    
    if (_image.isNull()) {
        std::cout << "警告：敌人" << enemy_type << "的图像为空" << std::endl;
        // 使用静态备用QPixmap
        temp.push_back(PaintInfo(defaultPixmapRed, absolute_pos.first, absolute_pos.second));
    } else {
        // 创建一个新的持久存在的QPixmap
        static QPixmap pixmap;
        pixmap = QPixmap::fromImage(_image);
        
        if (pixmap.isNull()) {
            std::cout << "警告：无法从敌人图像创建QPixmap" << std::endl;
            temp.push_back(PaintInfo(defaultPixmapGreen, absolute_pos.first, absolute_pos.second));
        } else {
            temp.push_back(PaintInfo(pixmap, absolute_pos.first, absolute_pos.second));
        }
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
        hp_bar->show();
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
    alive = true;
    enabled = true;
    hp = HP_MAX;
    hp_bar->show();
    setHpBarPosition();
    healthChange();
    hp_bar->hide();
}

void Enemy::disable() {
    enabled = false;
    hp_bar->hide();
}

void Enemy::healthChange() {
    hp_bar->setRange(0, HP_MAX);
    hp_bar->setValue(hp);
}

void Enemy::setHpBarPosition() {
    hp_bar->setGeometry(absolute_pos.first, absolute_pos.second - 20, absolute_rect.width(), 40);
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
            power = ENEMY_2_POWER;
            CD = ENEMY_2_CD;
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
