#include "Enemy.h"
#include <cmath>
#include "EnemyController.h"
#include <cstdlib>
#include "Weapon.h"
#include "Hero.h"
#include <iostream>
#include <QtCore/QDir>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtGui/QPen>
#include <QtCore/QFile>
#include <QtCore/QStringList>
#include <QtCore/QResource>

Enemy::Enemy(int enemy_style, QWidget *w_parent, EnemyController *controller, GameMap *m_parent, Hero *target, double real_X, double real_Y) {
    std::cout << "[Log] Enemy 构造函数开始, 类型: " << enemy_style << std::endl;
    QSize pix_size;
    
    // 初始化基本属性
    enabled = false;
    alive = false;
    widget_parent = w_parent;
    hp_bar = nullptr;
    map_parent = m_parent;
    this->target = target;
    this->controller = controller;
    enemy_type = enemy_style; // 保存敌人类型，die()函数需要使用
    
    // 根据敌人类型设置差异化参数
    switch(enemy_style) {
        case 1:
            HP_MAX = ENEMY_1_HEALTH;
            speed = ENEMY_1_SPEED;
            break;
        case 2:
            HP_MAX = ENEMY_2_HEALTH;
            speed = ENEMY_2_SPEED;
            break;
        case 3:
            HP_MAX = ENEMY_3_HEALTH;
            speed = ENEMY_3_SPEED;
            break;
    }
    pix_size = QSize(150,150);
    
    // 加载敌人图像 - 优化加载逻辑
    loadEnemyImage(enemy_style, pix_size); // 添加敌人类型参数
    
    // 初始化位置和碰撞盒
    hp = HP_MAX;
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

    // 仅当父窗口有效时创建血条 (直接调用，不使用QMetaObject::invokeMethod)
    if (widget_parent) {
        createHpBar();
    }
    
    std::cout << "[Log] Enemy 构造函数结束" << std::endl;
}

// 新增的辅助方法用于创建HP血条
void Enemy::createHpBar() {
    try {
        if (widget_parent && !hp_bar) {
            hp_bar = new QProgressBar(widget_parent);
            hp_bar->setRange(0, HP_MAX);
            hp_bar->setValue(hp);
            hp_bar->setGeometry(absolute_pos.first, absolute_pos.second - 20, absolute_rect.width(), 10);
            hp_bar->setTextVisible(false); // 不显示文本，减少渲染负担
            hp_bar->setStyleSheet("QProgressBar {background-color: transparent; border: 1px solid white;}"
                                 "QProgressBar::chunk {background-color: red;}");
            healthChange();
            if (!enabled) {
                hp_bar->hide();
            }
        }
    } catch (const std::exception& e) {
        std::cout << "创建敌人血条时发生异常: " << e.what() << std::endl;
    }
}

// 新增的辅助方法用于加载敌人图像
void Enemy::loadEnemyImage(int enemy_style, const QSize& size) {
    std::cout << "[Log] 开始加载敌人图像，类型: " << enemy_style << std::endl;
    
    // 1. 先尝试从resource加载
    QString resourcePath;
    QString filePath; // 用于尝试从文件系统加载
    
    switch(enemy_style) {
        case 1:
            resourcePath = ENEMY_1_PATH; // 从config.h获取资源路径
            filePath = "Assets/laocai.png"; // 尝试直接从文件系统加载
            break;
        case 2:
            resourcePath = ENEMY_2_PATH;
            filePath = "Assets/enemy2.png";
            break;
        case 3:
            resourcePath = ENEMY_3_PATH;
            filePath = "Assets/enemy3.png";
            break;
        default:
            resourcePath = ENEMY_1_PATH;
            filePath = "Assets/laocai.png";
            break;
    }
    
    std::cout << "[Log] 尝试加载敌人图像(资源): " << resourcePath.toStdString() << std::endl;
    
    // 检查资源是否可用
    bool resourceExists = QFile::exists(resourcePath);
    std::cout << "[Log] 资源存在检查: " << (resourceExists ? "存在" : "不存在") << std::endl;
    
    // 尝试从资源加载
    bool imageLoaded = _image.load(resourcePath);
    
    // 如果从资源加载失败，尝试从文件系统加载
    if (!imageLoaded) {
        std::cout << "[警告] 从资源加载失败，尝试从文件系统加载: " << filePath.toStdString() << std::endl;
        
        // 检查文件是否存在
        bool fileExists = QFile::exists(filePath);
        std::cout << "[Log] 文件存在检查: " << (fileExists ? "存在" : "不存在") << std::endl;
        
        if (fileExists) {
            imageLoaded = _image.load(filePath);
        }
    }
    
    // 如果所有加载尝试都失败，创建备用图像
    if (!imageLoaded) {
        std::cout << "[错误] 无法加载敌人图像，创建备用图像" << std::endl;
        // 创建备用图像 - 简单的灰色矩形
        _image = QImage(size.width(), size.height(), QImage::Format_ARGB32_Premultiplied);
        _image.fill(Qt::transparent);
        
        QPainter painter(&_image);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(QPen(Qt::black, 3));
        painter.setBrush(Qt::gray);
        
        // 根据不同类型创建不同形状，便于区分
        switch(enemy_style) {
            case 1:
                painter.drawEllipse(10, 10, size.width()-20, size.height()-20); // 圆形
                break;
            case 2:
                painter.drawRect(10, 10, size.width()-20, size.height()-20); // 方形
                break;
            case 3: {
                QPolygon triangle;
                triangle << QPoint(size.width()/2, 10) 
                        << QPoint(10, size.height()-10) 
                        << QPoint(size.width()-10, size.height()-10);
                painter.drawPolygon(triangle); // 三角形
                break;
            }
            default:
                painter.drawEllipse(10, 10, size.width()-20, size.height()-20);
                break;
        }
        
        std::cout << "[Log] 已创建备用几何图形" << std::endl;
    } else {
        std::cout << "[Log] 敌人图像加载成功!" << std::endl;
        // 成功加载后调整大小
        _image = _image.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    
    std::cout << "[Log] 敌人图像准备完成, 尺寸: " << _image.width() << "x" << _image.height() 
              << ", isNull: " << (_image.isNull() ? "是" : "否") << std::endl;
}

std::vector<PaintInfo> Enemy::paint() {
    std::vector<PaintInfo> temp;
    
    // 绘制敌人图像
    QPixmap enemyPixmap = QPixmap::fromImage(_image);
    if (enemyPixmap.isNull()) {
        std::cout << "[错误] Enemy::paint - 无法从_image创建QPixmap，创建备用图像" << std::endl;
        
        // 如果从_image创建QPixmap失败，直接创建一个新的QPixmap
        QPixmap backupPixmap(150, 150);
        backupPixmap.fill(Qt::transparent);
        QPainter p(&backupPixmap);
        p.setPen(QPen(Qt::black, 3));
        p.setBrush(Qt::gray);
        p.drawEllipse(10, 10, 130, 130);
        p.end();
        
        temp.push_back(PaintInfo(backupPixmap, absolute_pos.first, absolute_pos.second));
    } else {
        temp.push_back(PaintInfo(enemyPixmap, absolute_pos.first, absolute_pos.second));
    }
    
    // 绘制血条（直接使用QPixmap，不依赖于_image）
    int barWidth = 100; // 使用一个固定宽度替代hp_width
    QPixmap hpBarPixmap(barWidth + 20, 30);
    hpBarPixmap.fill(Qt::transparent);
    
    QPainter painter(&hpBarPixmap);
    painter.setPen(QPen(Qt::black, 3));
    painter.drawRect(0, 0, barWidth + 20, 20);
    painter.fillRect(10, 10, 1.0 * hp / HP_MAX * barWidth, 10, Qt::red); // 使用HP_MAX代替hp_max
    
    temp.push_back(PaintInfo(hpBarPixmap, absolute_pos.first - 10, absolute_pos.second - 30));
    
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
        controller->reportDeath(enemy_type, real_pos.first, real_pos.second); // 使用 enemy_type
}

void Enemy::enable() {
    std::cout << "[Log] Enemy::enable 开始" << std::endl;
    alive = true;
    enabled = true;
    hp = HP_MAX;
    
    // 输出图像信息，以便调试
    std::cout << "[Log] 敌人图像状态: " 
              << (_image.isNull() ? "空" : "有效") 
              << ", 尺寸: " << _image.width() << "x" << _image.height() 
              << ", 位置: (" << real_pos.first << "," << real_pos.second << ")" 
              << std::endl;
              
    if (hp_bar) {
        hp_bar->show();
        setHpBarPosition();
        healthChange();
    } else {
        std::cout << "[Log] 警告: hp_bar为空" << std::endl;
        // 如果血条为空，尝试重新创建
        createHpBar();
    }
    std::cout << "[Log] Enemy::enable 结束, enabled=" << enabled << ", alive=" << alive << std::endl;
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
    power = ENEMY_1_POWER;
    CD = ENEMY_1_CD;
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
            std::cout << "敌人与玩家发生碰撞!" << std::endl;
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

WeaponEnemy::~WeaponEnemy() {
    delete _weapon; // 析构函数实现
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
    // 现在可以安全地访问_weapon了
    if (_weapon) { 
    temp = _weapon->paint();
    for(auto &item : temp) {
        buffer.push_back(item);
        }
    }
    return buffer;
}
