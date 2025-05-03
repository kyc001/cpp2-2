#include "Hero.h"
#include "Enemy.h"
#include "Weapon.h"
#include <cmath>
#include "GameState.h"
#include <iostream>
#include <QTimer>
#include <QtGui/QPainter>
#include <QtGui/QPen>
#include <QtCore/QObject>

Hero::Hero() {
    _image.load(HERO_1_PATH);
    HP_MAX = 100;
    hp = HP_MAX;
    exp = 0;
    EXP_MAX = HERO_INIT_EXP;
    speed = 10;
    level = 1;
    widget_parent = nullptr;
    hp_bar = new QProgressBar();
    exp_bar = new QProgressBar();
    QSize pix_size(80, 80);
    _image.scaled(pix_size);
    absolute_pos.first = GAME_WIDTH * 0.5 - _image.width()*0.5;
    absolute_pos.second = GAME_HEIGHT * 0.5 - _image.height()*0.5;

    absolute_rect.setWidth(_image.width());
    absolute_rect.setHeight(_image.height());
    absolute_rect.moveTo(absolute_pos.first, absolute_pos.second);

    map_parent = nullptr;
    real_pos.first = 0;
    real_pos.second = 0;
    real_rect.setWidth(_image.width());
    real_rect.setHeight(_image.height());
    real_rect.moveTo((int)real_pos.first,(int)real_pos.second);

    alive = true;

    _weapon = nullptr;
    weapon_type = 1;
    
    hero_style = 1;
    _heroMovie = nullptr;
    _animTimer = nullptr;

    _game = nullptr;
    
    // 根据全局控制模式设置
    controlMode = (CONTROL_MODE == 0) ? KEYBOARD_CONTROL : MOUSE_CONTROL;
    mouseControlActive = false;
    mousePos = {0, 0};
    
    // 动画控制
    animationActive = false;
    animationDuration = 5000; // 5秒
}

Hero::~Hero() {
    if (_heroMovie) {
        _heroMovie->stop();
        delete _heroMovie;
    }
    
    if (_animTimer) {
        _animTimer->stop();
        delete _animTimer;
    }
}

Hero::Hero(int hero_style, QWidget *w_parent, GameMap *m_parent) {
    bool load_success = false;
    QString hero_path;
    QString gif_path;

    switch(hero_style){
        case 1: // 哈气耄耋
            hero_path = ":/Assets/maodie.gif"; // 使用GIF的第一帧作为静态图片
            gif_path = ":/Assets/maodie.gif";  // 动态GIF
            HP_MAX = HERO_1_HEALTH;
            weapon_type = 1; // AOE攻击
            reduce = HERO_1_REDUCE;
            speed = HERO_1_SPEED;
            break;
        case 2: // 奔跑耄耋
            hero_path = ":/Assets/maodie2.gif"; // 使用GIF的第一帧作为静态图片
            gif_path = ":/Assets/maodie2.gif"; // 动态GIF
            HP_MAX = HERO_2_HEALTH;
            weapon_type = 2; // 悬浮球攻击
            reduce = HERO_2_REDUCE;
            speed = HERO_2_SPEED;
            break;
        default: // 添加默认情况处理
            hero_path = ":/Assets/maodie.gif";
            gif_path = ":/Assets/maodie.gif";
            HP_MAX = HERO_1_HEALTH;
            weapon_type = 1;
            reduce = HERO_1_REDUCE;
            speed = HERO_1_SPEED;
            std::cerr << "[警告] Hero: 未知的英雄类型 " << hero_style << "，使用默认类型 1" << std::endl;
            break;
    }

    // 保存英雄类型，用于后续动画显示
    this->hero_style = hero_style;

    // 初始化基本属性
    hp = HP_MAX;
    exp = 0;
    level = 1;
    EXP_MAX = HERO_INIT_EXP;
    widget_parent = w_parent;

    if(GAME_HALT){
        hp = HALT_HP;
        HP_MAX = HALT_HPM;
        exp = HALT_EXP;
        EXP_MAX = HALT_EXPM;
        level = GAME_LEVEL;
        speed = HERO_SPEED;
        reduce = HERO_REDUCE;
    }

    hp_bar = new QProgressBar();
    exp_bar = new QProgressBar();
    hp_bar->setParent(widget_parent);
    hp_bar->show();
    exp_bar->setParent(widget_parent);
    exp_bar->show();

    QSize pix_size(80, 80);

    // 加载静态图片作为备用
    std::cout << "[Log] Hero: 尝试加载英雄图像: " << hero_path.toStdString() << std::endl;
    QImage tempImage;
    load_success = tempImage.load(hero_path);

    if (!load_success) {
        std::cerr << "[错误] Hero: 无法加载英雄图像: " << hero_path.toStdString() << "，创建备用图像" << std::endl;
        // 创建备用图像 - 在临时的 QImage 上绘制
        tempImage = QImage(pix_size, QImage::Format_ARGB32_Premultiplied);
        tempImage.fill(Qt::transparent);
        QPainter painter(&tempImage);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(QPen(Qt::black, 2));
        painter.setBrush(Qt::blue);
        painter.drawEllipse(5, 5, pix_size.width() - 10, pix_size.height() - 10);
        painter.end();
        _image = QPixmap::fromImage(tempImage);
    } else {
         std::cout << "[Log] Hero: 英雄图像加载成功!" << std::endl;
         // 成功加载后，从 QImage 创建 QPixmap 并缩放
         _image = QPixmap::fromImage(tempImage).scaled(pix_size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    // 检查最终的 _image 是否为空 (以防 fromImage 失败等极端情况)
    if (_image.isNull()) {
        std::cerr << "[错误] Hero: _image 在构造后仍为空，创建最终备用图像" << std::endl;
        _image = QPixmap(pix_size);
        _image.fill(Qt::darkBlue);
    }

    // 初始化动画
    _heroMovie = nullptr;
    _animTimer = nullptr;
    animationActive = false;
    animationDuration = 5000; // 5秒
    
    // 创建但暂不激活动画
    initAnimation(false);

    std::cout << "[Log] Hero: 图像准备完成, 尺寸: " << _image.width() << "x" << _image.height()
              << ", isNull: " << (_image.isNull() ? "是" : "否") << std::endl;

    absolute_pos.first = GAME_WIDTH * 0.5 - _image.width()*0.5;
    absolute_pos.second = GAME_HEIGHT * 0.5 - _image.height()*0.5;

    absolute_rect.setWidth(_image.width());
    absolute_rect.setHeight(_image.height());
    absolute_rect.moveTo(absolute_pos.first, absolute_pos.second);

    map_parent = m_parent;
    real_pos.first = map_parent->getPosRangeX() * 0.5 - _image.width()*0.5;
    real_pos.second = map_parent->getPosRangeY() * 0.5 - _image.height()*0.5;
    if(GAME_HALT){
        real_pos.first = HERO_X;
        real_pos.second = HERO_Y;
    }
    real_rect.setWidth(_image.width());
    real_rect.setHeight(_image.height());
    real_rect.moveTo((int)real_pos.first,(int)real_pos.second);

    healthChange();
    expChange();
    setHpBarPosition();
    setExpBarPosition();

    alive = true;

    _weapon = nullptr;

    map_parent->setAbsolutePos(absolute_pos.first - (int)real_pos.first,
                               absolute_pos.second - (int)real_pos.second);

    _game = nullptr;
    
    // 根据全局控制模式设置
    controlMode = (CONTROL_MODE == 0) ? KEYBOARD_CONTROL : MOUSE_CONTROL;
    mouseControlActive = false;
    mousePos = {0, 0};
}

void Hero::initAnimation(bool autoStart) {
    // 清理可能存在的旧资源
    if (_heroMovie) {
        _heroMovie->stop();
        delete _heroMovie;
        _heroMovie = nullptr;
    }
    
    if (_animTimer) {
        _animTimer->stop();
        delete _animTimer;
        _animTimer = nullptr;
    }
    
    // 创建新的动画资源
    QString gifPath;
    
    switch(hero_style) {
        case 1: // 哈气耄耋
            gifPath = ":/Assets/maodie.gif";
            break;
        case 2: // 奔跑耄耋
            gifPath = ":/Assets/maodie2.gif";
            break;
        default:
            gifPath = ":/Assets/maodie.gif";
            break;
    }
    
    // 如果使用资源文件路径加载失败，尝试使用绝对路径
    _heroMovie = new QMovie(gifPath);
    if (!_heroMovie->isValid()) {
        delete _heroMovie;
        
        // 尝试使用绝对路径
        if (hero_style == 1) {
            gifPath = "D:/Survivors/Survivors/Assets/maodie.gif";
        } else {
            gifPath = "D:/Survivors/Survivors/Assets/maodie2.gif";
        }
        
        _heroMovie = new QMovie(gifPath);
        std::cout << "[Log] Hero: 尝试使用绝对路径加载GIF: " << gifPath.toStdString() << std::endl;
    }
    
    // 设置动画尺寸
    _heroMovie->setScaledSize(QSize(80, 80));
    
    // 检查动画是否有效
    if (!_heroMovie->isValid()) {
        std::cerr << "[错误] Hero: 无法加载GIF动画: " << gifPath.toStdString() << std::endl;
        delete _heroMovie;
        _heroMovie = nullptr;
        return;
    }
    
    // 创建定时器，用于更新动画帧
    _animTimer = new QTimer();
    QObject::connect(_animTimer, &QTimer::timeout, this, &Hero::updateAnimationFrame);
    
    // 如果需要自动启动动画
    if (autoStart) {
        _heroMovie->start();
        _animTimer->start(33); // 大约30FPS
        animationActive = true;
        
        // 创建一个额外的定时器来停止动画
        QTimer* stopTimer = new QTimer(this);
        QObject::connect(stopTimer, &QTimer::timeout, [this, stopTimer]() {
            this->stopAnimation();
            stopTimer->deleteLater();
        });
        stopTimer->setSingleShot(true);
        stopTimer->start(animationDuration);
        
        std::cout << "[Log] Hero: GIF动画加载成功并启动，将在 " << (animationDuration/1000) << " 秒后停止" << std::endl;
    } else {
        std::cout << "[Log] Hero: GIF动画加载成功但未启动" << std::endl;
    }
}

void Hero::startAnimation() {
    if (!_heroMovie || !_animTimer) {
        initAnimation(true);
        return;
    }
    
    // 启动动画和计时器
    _heroMovie->start();
    _animTimer->start(33); // 大约30FPS
    animationActive = true;
    
    // 创建一个计时器来在一段时间后停止动画
    QTimer* stopTimer = new QTimer(this);
    QObject::connect(stopTimer, &QTimer::timeout, [this, stopTimer]() {
        this->stopAnimation();
        stopTimer->deleteLater();
    });
    stopTimer->setSingleShot(true);
    stopTimer->start(animationDuration);
    
    std::cout << "[Log] Hero: 启动动画显示，将在 " << (animationDuration/1000) << " 秒后停止" << std::endl;
}

void Hero::stopAnimation() {
    if (!_heroMovie || !_animTimer) {
        return;
    }
    
    // 停止动画和计时器
    _heroMovie->stop();
    _animTimer->stop();
    animationActive = false;
    
    std::cout << "[Log] Hero: 停止动画显示" << std::endl;
}

void Hero::updateAnimationFrame() {
    if (!_heroMovie || !_heroMovie->isValid() || !animationActive) {
        return;
    }
    
    // 获取当前帧
    QPixmap currentFrame = _heroMovie->currentPixmap();
    if (!currentFrame.isNull()) {
        _image = currentFrame;
    }
}

void Hero::setWidgetParent(QWidget *parent) {
    widget_parent = parent;
    hp_bar->setParent(widget_parent);
    exp_bar->setParent(widget_parent);
    setHpBarPosition();
    setExpBarPosition();
}

void Hero::healthChange() {
    hp_bar->setRange(0,HP_MAX);
    hp_bar->setValue(hp);
}

void Hero::expChange() {
    exp_bar->setRange(0, EXP_MAX);
    exp_bar->setValue(exp);
}

void Hero::setExpBarPosition() {
    exp_bar->setGeometry(150,0,GAME_WIDTH-150,60);
    // 设置经验条样式
    exp_bar->setStyleSheet("QProgressBar { border: 2px solid gray; border-radius: 5px; text-align: center; }"
                          "QProgressBar::chunk { background-color: #5DA5DA; width: 10px; }");
}

void Hero::setHpBarPosition() {
    hp_bar->setGeometry(absolute_pos.first - 10, absolute_pos.second - 30, absolute_rect.width() + 20, 30);
    // 设置血条样式
    hp_bar->setStyleSheet("QProgressBar { border: 2px solid gray; border-radius: 5px; text-align: center; }"
                         "QProgressBar::chunk { background-color: #C44E52; width: 10px; }");
}

void Hero::tick() {
    // 根据当前控制模式选择控制逻辑
    if (controlMode == KEYBOARD_CONTROL) {
        handleKeyboardControl();
    } else {
        handleMouseControl();
    }

    real_rect.moveTo((int)real_pos.first, (int)real_pos.second);
    map_parent->setAbsolutePos(absolute_pos.first - (int)real_pos.first,
                               absolute_pos.second - (int)real_pos.second);
    _weapon->tick();

    exp += _game->countExp(real_pos);
    std::cout << exp << std::endl;
    if(exp >= EXP_MAX){
        waiting_upgrade = true;
    }
    expChange();
}

// 处理键盘控制逻辑
void Hero::handleKeyboardControl() {
    double x_bias = 0;
    double y_bias = 0;
    for(auto key : keys_pressed){
        switch (key) {
            case Qt::Key_W:
                y_bias -= 1;
                break;
            case Qt::Key_A:
                x_bias -= 1;
                break;
            case Qt::Key_S:
                y_bias += 1;
                break;
            case Qt::Key_D:
                x_bias += 1;
                break;
            default:
                break;
        }
    }
    double mod = sqrt(x_bias * x_bias + y_bias * y_bias);
    if(mod != 0){
        x_bias /= mod; y_bias /= mod;
    }
    x_bias *= speed;
    y_bias *= speed;

    if(attemptMove(x_bias, 0)){
        real_pos.first += x_bias;
    }
    if(attemptMove(0, y_bias)){
        real_pos.second += y_bias;
    }
}

// 处理鼠标控制逻辑
void Hero::handleMouseControl() {
    if (!mouseControlActive) {
        return;
    }
    
    // 计算人物与鼠标之间的方向向量
    int targetX = mousePos.first - map_parent->getAbsoluteX();
    int targetY = mousePos.second - map_parent->getAbsoluteY();
    
    double dx = targetX - real_pos.first;
    double dy = targetY - real_pos.second;
    
    // 计算距离
    double distance = sqrt(dx * dx + dy * dy);
    
    // 如果距离小于一定值，不移动
    if (distance < 10) {
        return;
    }
    
    // 标准化方向向量
    dx /= distance;
    dy /= distance;
    
    // 计算移动增量
    double x_bias = dx * speed;
    double y_bias = dy * speed;
    
    // 如果太近就直接移动到目标位置
    if (distance < speed) {
        x_bias = dx * distance;
        y_bias = dy * distance;
    }
    
    // 尝试移动
    if(attemptMove(x_bias, 0)){
        real_pos.first += x_bias;
    }
    if(attemptMove(0, y_bias)){
        real_pos.second += y_bias;
    }
}

void Hero::mouseMoveTick(QMouseEvent *event) {
    mousePos.first = event->position().x();
    mousePos.second = event->position().y();
    mouseControlActive = true;
}

void Hero::keyPressTick(QKeyEvent *event) {
    if(!event->isAutoRepeat()) {  // 判断如果不是长按时自动触发的按下,就将key值加入容器
        keys_pressed.push_back(event->key());
        
        // 检测Ctrl+M组合键，用于切换动画
        if (event->key() == Qt::Key_M && (event->modifiers() & Qt::ControlModifier)) {
            if (animationActive) {
                stopAnimation();
            } else {
                startAnimation();
            }
        }
    }
}

void Hero::keyReleaseTick(QKeyEvent *event) {
    if(!event->isAutoRepeat()) {  //判断如果不是长按时自动触发的释放,就将key值从容器中删除
        keys_pressed.remove(event->key());
    }
}

std::vector<PaintInfo> Hero::paint() {
    std::vector<PaintInfo> temp;

    // 确保 _image (QPixmap) 有效才使用
    if (_image.isNull()) {
         std::cerr << "[错误] Hero::paint - _image (QPixmap) 为空，无法绘制英雄!" << std::endl;
         QPixmap errorPixmap(60, 60);
         errorPixmap.fill(Qt::magenta); // 品红色表示错误
         temp.push_back(PaintInfo(errorPixmap, absolute_pos.first, absolute_pos.second));
    } else {
         // 使用当前的图像（可能是动画的当前帧）
         temp.push_back(PaintInfo(_image, absolute_pos.first, absolute_pos.second));
    }

    // 绘制武器（如果存在）
    if (_weapon) {
        std::vector<PaintInfo> weapon_info = _weapon->paint();
        // 现在 PaintInfo 可以复制/移动了，insert 可以正常工作
        temp.insert(temp.end(), weapon_info.begin(), weapon_info.end());
    }
    return temp;
}

void Hero::setRealPosition(double x, double y) {
    real_pos.first = x;
    real_pos.second = y;
    real_rect.moveTo((int)real_pos.first, (int)real_pos.second);
    //TODO：这里之后要加上使地图移动的部分
}

void Hero::giveWeapon() {
    switch(weapon_type){
        case 1: // 哈气耄耋 - AOE攻击
            _weapon = new HeroStaticAOEWeapon(map_parent, (Hero *)this,
                                              WEAPON_1_DEFAULT_RANGE, (unsigned)WEAPON_1_BULLET_TYPE, WEAPON_1_DAMAGE);
            break;
        case 2: // 奔跑耄耋 - 悬浮球攻击
            _weapon = new HeroDynamicWeapon(map_parent, (Hero *) this, WEAPON_2_CD,
                                            WEAPON_2_BULLET_TYPE, WEAPON_2_DAMAGE);
            break;
    }
}

bool Hero::judgeDamage(Enemy *e) {
    return _weapon->judgeDamage(e);
}

void Hero::damage(int h) {
    // 防止多次处理相同的伤害或重复处理
    static bool processing_damage = false;
    
    // 如果当前正在处理伤害，直接返回，防止递归调用
    if (processing_damage) {
        return;
    }
    
    // 设置处理标志
    processing_damage = true;
    
    // 实际处理伤害
    if (alive) {
        // 计算实际伤害 (考虑减伤)
        int actual_damage = (int)(h * (1.0 - reduce));
        hp -= actual_damage;
        // 记录日志
        std::cout << "英雄受到伤害: " << actual_damage << ", 当前血量: " << hp << "/" << HP_MAX << std::endl;
        healthChange();
        if (hp <= 0) {
            hp = 0;
            std::cout << "英雄死亡！" << std::endl;
            alive = false;
        }
    }
    
    // 清除处理标志
    processing_damage = false;
}

bool Hero::attemptMove(double x_bias, double y_bias) {
    QRect test(real_rect);
    test.moveTo(int(real_pos.first + x_bias), int(real_pos.second + y_bias));
    // 由于舍弃障碍物配置，只需检查地图边界
    if(map_parent->checkPosition(test)){
        return true;
    } else {
        return false;
    }
}

void Hero::upgrade(int type) {
    std::cout << "[Log] Hero::upgrade 开始，类型: " << type << std::endl;
    
    try {
        switch(type){
            case 1: { // 提升 HP（通用）
                std::cout << "[Log] 升级类型 1: 提升 HP" << std::endl;
                int bias = HP_MAX - hp;
                HP_MAX = (int)((double) HP_MAX * HP_INC_RATE);
                hp = HP_MAX - bias;
                healthChange();
                std::cout << "[Log] HP 上限提升至: " << HP_MAX << std::endl;
                break;
            }
            case 2: { // 提升速度（通用）
                std::cout << "[Log] 升级类型 2: 提升速度" << std::endl;
                speed = (int)((double) speed * SPEED_INC_RATE);
                std::cout << "[Log] 速度提升至: " << speed << std::endl;
                break;
            }
            case 3: { 
                if (weapon_type == 1) { // 哈气耄耋 - 提升AOE范围
                    std::cout << "[Log] 升级类型 3: 提升AOE范围" << std::endl;
                    if (_weapon) {
                        _weapon->upgrade();
                        std::cout << "[Log] AOE范围已增加" << std::endl;
                    }
                } else if (weapon_type == 2) { // 奔跑耄耋 - 提高悬浮球转速
                    std::cout << "[Log] 升级类型 3: 提升悬浮球转速" << std::endl;
                    if (_game) { // 确保 _game 指针有效
                        for (auto& orb : _game->floating_orbs) { // 访问 GameState 中的悬浮球列表
                            if (orb) { // 确保 orb 指针有效
                                try {
                                    orb->increaseSpeed(); // 调用悬浮球的提速方法
                                } catch(const std::exception& e) {
                                    std::cerr << "[错误] Hero::upgrade: 调用 increaseSpeed 时异常: " << e.what() << std::endl;
                                }
                            }
                        }
                    } else {
                        std::cerr << "[错误] Hero::upgrade: _game 指针为空，无法升级悬浮球！" << std::endl;
                    }
                }
                break;
            }
            default: // 处理未知类型
                std::cerr << "[错误] Hero::upgrade: 未知的升级类型 " << type << std::endl;
                break;
        }
        
        // 更新状态
        keys_pressed.clear();
        waiting_upgrade = false;
        EXP_MAX *= 2;
        exp = 0;
        level++;
        
        // 触发UI更新
        try {
            expChange();
        } catch(const std::exception& e) {
            std::cerr << "[错误] Hero::upgrade: 调用 expChange 时异常: " << e.what() << std::endl;
        }
        
        std::cout << "[Log] Hero::upgrade 完成，等级提升至: " << level << std::endl;
    } catch(const std::exception& e) {
        std::cerr << "[严重错误] Hero::upgrade: 升级过程中发生异常: " << e.what() << std::endl;
        // 确保升级状态正确，即使出现异常
        waiting_upgrade = false;
    } catch(...) {
        std::cerr << "[严重错误] Hero::upgrade: 升级过程中发生未知异常" << std::endl;
        waiting_upgrade = false;
    }
}

int Hero::getHpMax() const {
    return HP_MAX;
}

int Hero::getHp() const {
    return hp;
}

int Hero::getExp() const {
    return exp;
}

int Hero::getExpMax() const {
    return EXP_MAX;
}

int Hero::getSpeed() const {
    return speed;
}

double Hero::getReduce() const {
    return reduce;
}

const std::pair<double, double> &Hero::getRealPos() const {
    return real_pos;
}

int Hero::getDamage() {
    return _weapon->getDamage();
}

int Hero::getCD() {
    return _weapon->getCD();
}

// 返回包含描述和图标路径的升级选项
QVector<QPair<QString, QString>> Hero::getUpgradeOptions() const {
    QVector<QPair<QString, QString>> options;
    
    // 添加基本的升级选项及其图标路径（所有英雄通用）
    options.append(qMakePair(QString("提升生命上限 (+%1%)").arg(int((HP_INC_RATE - 1.0) * 100)),
                           QString(":/Assets/shengming.png")));
    options.append(qMakePair(QString("提升移动速度 (+%1%)").arg(int((SPEED_INC_RATE - 1.0) * 100)),
                           QString(":/Assets/sudu.png")));
    
    // 根据武器类型添加不同的升级选项及其图标路径
    if (_weapon) {
        QString weaponUpgradeDesc;
        
        // 使用文件系统路径直接访问图标文件
        QString weaponUpgradeIcon = "D:/Survivors/Survivors/Assets/gongji.png";
        
        if (weapon_type == 1) { // 哈气耄耋
            weaponUpgradeDesc = QString("提升AOE技能范围 (+20%)");
        } else if (weapon_type == 2) { // 奔跑耄耋
            weaponUpgradeDesc = QString("提升悬浮球转速 (+30%)");
        } else {
            weaponUpgradeDesc = QString("提升武器伤害"); // 默认或未知武器类型的升级
        }
        options.append(qMakePair(weaponUpgradeDesc, weaponUpgradeIcon));
    }
    
    return options;
}

// 实现鼠标双击事件处理
void Hero::mouseDoubleClickEvent(QMouseEvent *event) {
    // 双击启动动画
    if (!animationActive) {
        startAnimation();
    } else {
        stopAnimation();
    }
}





