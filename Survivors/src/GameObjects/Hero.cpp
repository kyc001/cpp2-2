#include <QBuffer>
#include <QKeyEvent>
#include <QDebug>
#include <cmath> // 用于计算距离

Hero::Hero(GameState *game_state, HeroStyle hero_style)
    : game_state(game_state), HP(100), MAX_HP(100), level(1), exp(0),
      attack(10), defense(5), speed(5), score(0), hero_style(hero_style),
      animation_movie(nullptr), animation_showing(false) {
  
  // 加载静态图片
  QString staticImgPath;
  if (hero_style == HeroStyle::HAQI) {
    staticImgPath = ":/img/img/laocai.png"; // 哈气耄耋静态图片
  } else {
    staticImgPath = ":/img/img/laocai2.png"; // 奔跑耄耋静态图片
  }
  static_pixmap = QPixmap(staticImgPath);
  if (static_pixmap.isNull()) {
    qWarning() << "Failed to load static image:" << staticImgPath;
    static_pixmap = QPixmap(50, 50); // 设置一个占位符
  }
  static_pixmap = static_pixmap.scaled(50, 50, Qt::KeepAspectRatio);
  
  // 加载GIF动画
  QString gifPath;
  if (hero_style == HeroStyle::HAQI) {
    gifPath = ":/img/img/maodie.gif"; // 哈气耄耋 GIF
  } else {
    gifPath = ":/img/img/maodie2.gif"; // 奔跑耄耋 GIF
  }

  animation_movie = new QMovie(gifPath);
  if (!animation_movie->isValid()) {
    qWarning() << "Failed to load hero GIF:" << gifPath;
  } else {
    animation_movie->setScaledSize(QSize(50, 50)); // 调整GIF大小
    connect(animation_movie, &QMovie::frameChanged, this, [this](int frameNumber) {
      if (animation_showing) {
        Q_UNUSED(frameNumber);
        setPixmap(animation_movie->currentPixmap());
      }
    });
    // 预先启动动画，但不显示
    animation_movie->start();
    animation_movie->setPaused(true);
  }

  // 初始显示静态图片
  setPixmap(static_pixmap);
  
  setFlag(QGraphicsItem::ItemIsFocusable);
  setFocus();
  setPos(400, 300); // 初始位置
  setTransformOriginPoint(25, 25); // 设置旋转中心为图片中心

  // 其他初始化...
  if (hero_style == HeroStyle::BENPAO) {
    giveWeapon(WeaponType::FLOATING_ORB); // 奔跑耄耋初始武器
  } else {
    giveWeapon(WeaponType::PROJECTILE); // 哈气耄耋初始武器
    initAutoFireTimer(); // 初始化哈气耄耋的自动开火定时器
  }
}

Hero::~Hero() {
  if (animation_movie) {
    animation_movie->stop();
    delete animation_movie;
  }
  if (auto_fire_timer) { // 清理自动开火定时器
    auto_fire_timer->stop();
    delete auto_fire_timer;
  }
  // ... 其他清理代码 ...
}

void Hero::showAnimation(bool show) {
  animation_showing = show;
  if (show) {
    // 显示动画
    if (animation_movie && animation_movie->isValid()) {
      animation_movie->setPaused(false);
      setPixmap(animation_movie->currentPixmap());
    }
  } else {
    // 显示静态图片
    if (animation_movie) {
      animation_movie->setPaused(true);
    }
    setPixmap(static_pixmap);
  }
}

void Hero::toggleAnimation() {
  showAnimation(!animation_showing);
}

void Hero::initAutoFireTimer() {
  auto_fire_timer = new QTimer(this);
  connect(auto_fire_timer, &QTimer::timeout, this, &Hero::autoFire);
  auto_fire_timer->start(500); // 每500毫秒自动开火一次
}

int Hero::getProjectileCount() {
  // 基础发射2个子弹，每升一级增加2个
  return 2 + (level - 1) * 2;
}

void Hero::autoFire() {
  if (weapon_type == WeaponType::PROJECTILE) { // 确保是哈气耄耋
    shoot(); // 调用现有的射击方法
  }
}

void Hero::shoot() {
  if (weapon_type != WeaponType::PROJECTILE) return;
  
  // 获取发射子弹的数量（基础2个，每级+2个）
  int projectileCount = getProjectileCount();
  qDebug() << "发射" << projectileCount << "个追踪子弹";
  
  // 查找最近的敌人们
  auto nearestEnemies = findNearestEnemies(projectileCount);
  
  // 发射子弹
  for (int i = 0; i < projectileCount && i < nearestEnemies.size(); ++i) {
    if (nearestEnemies[i]) {
      // 发射子弹追踪这个敌人
      game_state->createProjectile(this, nearestEnemies[i]);
    }
  }
}

// 查找最近的敌人
std::vector<Enemy*> Hero::findNearestEnemies(int count) {
  std::vector<Enemy*> result;
  std::vector<std::pair<double, Enemy*>> enemiesWithDistance;

  // 获取所有敌人
  auto allEnemies = game_state->getAllEnemies();
  
  // 计算每个敌人与英雄的距离
  for (auto enemy : allEnemies) {
    if (enemy && enemy->isEnabled()) {
      double dx = enemy->getRealX() - this->x();
      double dy = enemy->getRealY() - this->y();
      double distance = std::sqrt(dx*dx + dy*dy);
      
      enemiesWithDistance.push_back({distance, enemy});
    }
  }
  
  // 按距离排序
  std::sort(enemiesWithDistance.begin(), enemiesWithDistance.end(),
    [](const auto& a, const auto& b) {
      return a.first < b.first;
    });
  
  // 选择最近的count个敌人
  for (int i = 0; i < count && i < enemiesWithDistance.size(); ++i) {
    result.push_back(enemiesWithDistance[i].second);
  }
  
  return result;
} 