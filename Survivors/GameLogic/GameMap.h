#ifndef SURVIVORS_GAMEMAP_H
#define SURVIVORS_GAMEMAP_H
#include "config.h"
#include "PaintInfo.h"
#include <QPixmap>
#include <QRect>
#include <utility>
#include <vector>
#include <exception>

class GameMap {
    unsigned map_style{};
    QPixmap map_bg;
    std::pair<int, int> pos_range;
    std::pair<int, int> absolute_pos;
    std::pair<int, int> lattice;
    std::pair<int, int> lattice_scale;

public:
    explicit GameMap(unsigned map_style);

    std::vector<PaintInfo> paint();

    int getPosRangeX();
    int getPosRangeY();

    void setAbsolutePos(int x, int y);

    [[nodiscard]] int getAbsoluteX() const { return absolute_pos.first; }
    [[nodiscard]] int getAbsoluteY() const { return absolute_pos.second; }

    void loadBarriers();

    bool checkPosition(const QRect & object);
};


#endif //VSCPROJECT_GAMEMAP_H
