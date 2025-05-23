#ifndef SURVIVORS_PAINTINFO_H
#define SURVIVORS_PAINTINFO_H
#include <QtGui/QPixmap>


class PaintInfo {
    QPixmap target;
    int m_X;
    int m_Y;

public:
    PaintInfo(const QPixmap &target, int mX, int mY);

    const QPixmap &getTarget() const;

    int getMX() const;

    int getMY() const;
};



#endif //VSCPROJECT_PAINTINFO_H
