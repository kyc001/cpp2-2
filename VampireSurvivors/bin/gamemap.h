#ifndef GAMEMAP_H
#define GAMEMAP_H

#include <QObject>

class GameMap : public QObject {
    Q_OBJECT
public:
    explicit GameMap(QObject *parent = nullptr);
    ~GameMap() override;

    QVector<QVector<int>> getBarries() const;
    void mapLoading();
    void loadBarries();

private:
    QVector<QVector<int>> barries;

};

#endif // GAMEMAP_H
