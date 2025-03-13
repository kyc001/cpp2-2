//2025年3月12日
#ifndef MAPDRAWER_H
#define MAPDRAWER_H

#include <QObject>
#include <QPainter>
#include <vector>

class MapDrawer : public QObject {
    Q_OBJECT
public:
    explicit MapDrawer(QObject *parent = nullptr);
    ~MapDrawer();

    // 绘制地图
    void drawMap(QPainter& painter, const std::vector<std::vector<int>>& map);

private:
    // 墙的图片资源
    QPixmap wall1Pixmap;
    QPixmap wall2Pixmap;
};

#endif // MAPDRAWER_H
