#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <QObject>

class Obstacle : public QObject {
    Q_OBJECT
public:
    Obstacle(int x, int y, int width, int height, QObject *parent = nullptr);
    ~Obstacle() override;

    // 获取障碍物的位置和大小
    int getX() const;
    int getY() const;
    int getWidth() const;
    int getHeight() const;

private:
    int my_x;
    int my_y;
    int my_width;
    int my_height;
};

#endif
