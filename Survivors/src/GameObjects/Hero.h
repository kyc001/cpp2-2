#include <QObject> // 需要包含QObject以使用信号和槽
#include <QTimer>  // 需要包含QTimer
#include <QGraphicsPixmapItem>
#include <QMovie>
#include <QLabel>
#include <QPixmap>
#include <vector>
#include <functional>

class Enemy; // 前向声明

enum class HeroStyle {
    HAQI,  // 哈气耄耋
    BENPAO // 奔跑耄耋
};

enum class WeaponType {
    PROJECTILE,  // 投射物
    FLOATING_ORB // 悬浮球
};

class GameState;

class Hero : public QObject, public QGraphicsPixmapItem {
  Q_OBJECT // 添加 Q_OBJECT 宏以支持信号和槽

public:
  Hero(GameState *game_state, HeroStyle hero_style = HeroStyle::HAQI);
  ~Hero() override;
  
  void setHP(int hp);
  int getHP() const;
  int getMaxHP() const;
  void takeDamage(int damage);
  void gainExp(int exp_gain);
  int getLevel() const;
  int getExp() const;
  int getNextLevelExp() const;
  int getAttack() const;
  int getDefense() const;
  int getSpeed() const;
  int getScore() const;
  void addScore(int score_gain);
  HeroStyle getHeroStyle() const { return hero_style; }
  WeaponType getWeaponType() const { return weapon_type; }
  
  void keyPressEvent(QKeyEvent *event) override;
  void keyReleaseEvent(QKeyEvent *event) override;
  void shoot();
  
  // 新增：控制动画显示/隐藏
  void showAnimation(bool show);
  void toggleAnimation();
  bool isAnimationShowing() const { return animation_showing; }
  
  std::vector<std::pair<QString, std::function<void()>>> getUpgradeOptions();
  void upgrade(int option_index);

private slots:
  void autoFire(); // 新增的自动开火槽函数

private:
  GameState *game_state;
  int HP;
  int MAX_HP;
  int level;
  int exp;
  int attack;
  int defense;
  int speed;
  int score;
  HeroStyle hero_style;
  WeaponType weapon_type;
  
  bool up_pressed = false;
  bool down_pressed = false;
  bool left_pressed = false;
  bool right_pressed = false;
  
  QMovie *animation_movie = nullptr;
  QLabel pixmap_label; // 用于显示 Pixmap 的 QLabel
  QTimer *auto_fire_timer = nullptr; // 自动开火定时器
  QPixmap static_pixmap; // 静态图片
  bool animation_showing = false; // 是否显示动画
  
  void updateDirection(int key);
  void resetDirection(int key);
  void giveWeapon(WeaponType type);
  void initAutoFireTimer(); // 初始化自动开火定时器的方法
  int getProjectileCount(); // 获取哈气耄耋发射的子弹数量
  std::vector<Enemy*> findNearestEnemies(int count); // 查找最近的敌人
}; 