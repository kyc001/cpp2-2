#ifndef GAMEMAP_H
#define GAMEMAP_H

#include <QObject>
#include <QVector>
#include <QPair>

class GameMap : public QObject
{
    Q_OBJECT
    // 在 GameMap 类的 public: 部分添加
public:
    const QVector<QRect> &getObstacles() const { return obstacles; }

private:
    QVector<QRect> obstacles; // 确保这个成员变量存在
public:
    explicit GameMap(QObject *parent = nullptr);
    virtual ~GameMap() override;

    // 地图生成
    void generateMap();

    // 获取地图信息
    int getWidth() const;
    int getHeight() const;
    QVector<QVector<int>> getBarries() const;

    // 碰撞检测
    bool isBarrier(int x, int y) const;

    // 路径计算
    QVector<QPair<int, int>> calculatePath(int startX, int startY, int endX, int endY) const;

private:
    int width;
    int height;
    QVector<QVector<int>> barriers;
};

#endif // GAMEMAP_H
