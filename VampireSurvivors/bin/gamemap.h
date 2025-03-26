#ifndef GAMEMAP_H
#define GAMEMAP_H

#include <QObject>
#include <QVector>

class GameMap : public QObject {
    Q_OBJECT
public:
    explicit GameMap(QObject *parent = nullptr);
    ~GameMap() override;

    QVector<QVector<int>> getBarries() const;
    void mapLoading();
    void loadBarriers();
    
    // New methods for map dimensions and barrier checking
    int getWidth() const;
    int getHeight() const;
    bool isBarrier(int x, int y) const;

private:
    QVector<QVector<int>> barriers;
    int map_width;   // Width of the map
    int map_height;  // Height of the map
};

#endif // GAMEMAP_H
